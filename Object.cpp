#include "Object.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <QElapsedTimer>

Object::Object(char *filename){

        Import(filename);

}
Object::Object(char *filename,char* colorImg, char* heightImg)
{
    numBones=0;
    std::vector<glm::vec3>  vertexList;
    std::vector<glm::vec3>  normalList;
    std::vector<glm::vec2>  textureList;
    std::vector<Triangle>   faceList;

    std::ifstream objFile;
    objFile.open(filename);

 //read the data into de Object

    if (objFile.is_open()){

        std::string line;
        std::vector<int> group(1,-1);
        while(std::getline(objFile,line)){
            std::istringstream numbers (line.substr(2));

            if(line.compare(0,2,"v ")==0){
                glm::vec3   p1; numbers>>p1.x;numbers>>p1.y;numbers>>p1.z;
                vertexList. push_back(p1);}
            if(line.compare(0,2,"vn")==0){
                glm::vec3   p2; numbers>>p2.x;numbers>>p2.y;numbers>>p2.z;
                normalList. push_back(p2);
            }
            if(line.compare(0,2,"vt")==0){
                glm::vec2   p3; numbers>>p3.x;numbers>>p3.y;
                textureList.push_back(p3);
            }
            if(line.compare(0,2,"f ")==0){
                Triangle    f; f.g=group; //    v/t/n
                f.vd=new glm::vec3[3];
                char aux;
                numbers>>f.vd[0].x;numbers>>aux;numbers>>f.vd[0].y;numbers>>aux;numbers>>f.vd[0].z;
                numbers>>f.vd[1].x;numbers>>aux;numbers>>f.vd[1].y;numbers>>aux;numbers>>f.vd[1].z;
                numbers>>f.vd[2].x;numbers>>aux;numbers>>f.vd[2].y;numbers>>aux;numbers>>f.vd[2].z;
                faceList.push_back(f);
            }
            if(line.compare(0,2,"g ")==0){
                group.clear();
                int aux;
                while(numbers>>aux){
                    if(aux+1>numBones)    numBones=aux+1;
                    group.push_back(aux);
                }
                if(group.size()==0){
                    std::cout<<"here";
                }
            }

        }
        objFile.close();
    } else std::cout<<"no abre\n";

    numFaces=faceList.size();
    std::cout<<"#faces "<<faceList.size()<<"\n";

    for(auto f : faceList){
        for(int vertexNum=0;vertexNum<3;vertexNum++){
            vertices.push_back(vertexList[ (int)(0.5+f.vd[vertexNum].x) -1]);
        }
    }

    for(auto f : faceList){
        for(int vertexNum=0;vertexNum<3;vertexNum++){
        normals.push_back(normalList[(int)(0.5+ f.vd[vertexNum].z) -1]);
        }
    }

    for(auto f : faceList){
        for(int vertexNum=0;vertexNum<3;vertexNum++){
        texture.push_back(textureList[f.vd[vertexNum].y-1]);
        }
    }

    //deform.resize(vertices.size(),glm::vec3(0,0,0));

    for(auto f : faceList){

        glm::vec2 t1 = textureList[(int)(0.5+ f.vd[1].y) -1]-textureList[(int)(0.5+  f.vd[0].y)-1];
        glm::vec2 t2 = textureList[(int)(0.5+ f.vd[2].y) -1]-textureList[(int)(0.5+  f.vd[0].y)-1];

        glm::vec3 V0 = vertexList[(int)(0.5+  f.vd[0].x) -1];
        glm::vec3 V1 = vertexList[(int)(0.5+  f.vd[1].x) -1]-V0;
        glm::vec3 V2 = vertexList[(int)(0.5+  f.vd[3].x) -1]-V0; //f.vd[3].x ?????

        //Udirection = (v2/(v2-v1)) * P1 + (v1/(v1-v2)) * P2
//grad(u)=
        glm::vec3 T= (t2.y/(t2.y*t1.x-t1.y*t2.x)) * V1 - (t1.y/(t2.y*t1.x-t1.y*t2.x)) * V2;

        for(int i=0;i<3;i++){ //ortho
        glm::vec3 N = glm::normalize( normalList[(int)(0.5+  f.vd[i].z) -1] );
        glm::vec3 nT= T - glm::dot(N,T) * N;
        glm::normalize(nT);
        tangents.push_back(nT); //save
        }
    }
    if(numBones>0){
    weights         .assign(numFaces*3, glm::vec4(-1,-1,-1,-1));
    weightindices   .assign(numFaces*3, glm::vec4(-1,-1,-1,-1));
    std::vector<std::vector<int>> jointGroups;          //contiene los indices de las caras por cada joint
    std::vector<std::vector<int>> jointSignatures;      //contiene la signature de cada joint
    std::vector<std::vector<int>> boneGroups(numBones); //contiene  los indices de los vertices por bone
    for(int f=0;f<faceList.size();f++){
        bool is_joint   =   (faceList[f].g.size()>1) ? true : false;
        if(is_joint){           //register

            bool alreadyRegistered =false;
            int i;
            for(i=0;i<jointSignatures.size()&&(!alreadyRegistered);i++) //compare with existing joints
                if(jointSignatures[i]==faceList[f].g) alreadyRegistered =true;

            if(alreadyRegistered){jointGroups[i-1].push_back(f);}
            else{
                jointSignatures.push_back(faceList[f].g);
                std::vector<int> tmp(1,f);
                jointGroups.push_back(tmp);
            }
        }
        if(!is_joint){
            for(int i=0;i<3;i++){
            boneGroups[faceList[f].g[0]].push_back(faceList[f].vd[i].x);
            weights         [3*f+i]=glm::vec4(1,0,0,0);
            weightindices   [3*f+i]=glm::vec4(faceList[f].g[0],-1,-1,-1);
            }
        }

    }
    std::cout<<"pesos\n";
    // pone los pesos de los vertices en joints
    for(int joint=0;joint<jointGroups.size();joint++){
        std::cout<<"joint";for(auto j: jointSignatures[joint]) std::cout<<" "<<j;
        std::cout<<"\nbuscando bordes...\n";
        //busca los bordes de la joint:vector de bordes, borde:vector de vertices

        std::vector<std::vector<int>> borders(jointSignatures[joint].size());

        for(auto fi : jointGroups[joint])   //  for each vertex in each joint face
            for(int vn=0;vn<3;vn++)         //
                for(int b=0; b<jointSignatures[joint].size();b++)           // for each vertex in each bone
                    for(auto vi: boneGroups[jointSignatures[joint].at(b)])  //
                        if( glm::distance(vertexList[faceList[fi].vd[vn].x] , vertexList[vi])<0.01)//esto no me gusta
                            borders[b].push_back(vi);


std::cout<<"calculando pesos...\n";
        //para cada vertice:

        for(auto fi : jointGroups[joint])
            for(int vn=0;vn<3;vn++){

        //calcula distancia minima a cada borde
        // QElapsedTimer timer;timer.start();


                std::vector<float> minDistances(jointSignatures[joint].size(),-1);
                for(int border=0;border<jointSignatures[joint].size();border++){
                    for(auto border_vertex : borders[border]){
                        float distance =glm::length(vertexList[border_vertex-1]-vertexList[faceList[fi].vd[vn].x-1]);
                        if(distance<minDistances[border]|| minDistances[border]<0)
                            minDistances[border]=distance;
                    }
                }

  //      std::cout<<"\n1:"<<timer.nsecsElapsed();timer.restart();

        //peso bone i:producto de distancias excepto i / suma producto de distancias
                std::vector<float> boneWeights(jointSignatures[joint].size(),1);
                for(int border=0;border<jointSignatures[joint].size();border++){
                    for(int dist=0;dist<jointSignatures[joint].size();dist++)
                        boneWeights[border]*= (border!=dist) ? minDistances[dist] : 1 ;
//                    boneWeights[border]=3*boneWeights[border]*boneWeights[border]-2*boneWeights[border]*boneWeights[border]*boneWeights[border];//maybe this makes it look better
                }
                float sum=0; for(auto w : boneWeights) sum+=w;
                if(sum > 0.0001){
                    for(int border=0;border<jointSignatures[joint].size();border++) boneWeights[border]/=sum;
                }else{
                    for(int border=0;border<jointSignatures[joint].size();border++)
                        boneWeights[border]= (minDistances[border]<0.0001) ? 1 : 0;
                    float sum=0; for(auto w : boneWeights) sum+=w;
                    for(int border=0;border<jointSignatures[joint].size();border++) boneWeights[border]/=sum;
                }

    //            std::cout<<" 2:"<<timer.nsecsElapsed();timer.restart();

        //peso e indices finales
                std::vector<int>    boneIndices(jointSignatures[joint].begin(),jointSignatures[joint].end());
                if(jointSignatures[joint].size()>4){
                    std::vector<float> orderedWeights(boneWeights.begin(),boneWeights.end());
                    std::sort(orderedWeights.begin(),orderedWeights.end());
                    for(int border=0;border<jointSignatures[joint].size();border++)
                        if(orderedWeights[orderedWeights.size()-4]-boneWeights[border]>0.0001){
                            boneWeights.erase(boneWeights.begin()+border);
                            boneIndices.erase(boneIndices.begin()+border);
                        }
                }else{
                    for(int i=0;i<4-jointSignatures[joint].size();i++){
                        boneWeights.push_back(0);
                        boneIndices.push_back(-1);
                    }
                }

                weights         [3*fi+vn]=glm::vec4(boneWeights[0],boneWeights[1],boneWeights[2],boneWeights[3]);
                weightindices   [3*fi+vn]=glm::vec4(boneIndices[0],boneIndices[1],boneIndices[2],boneIndices[3]);


    //            std::cout<<" 3:"<<timer.nsecsElapsed()<<"\n";
            }

    }
}

    normalize();
    toBuffers();

    color   = new Texture(colorImg);    colorFile.assign(colorImg);
    height  = new Texture(heightImg);   heightFile.assign(heightImg);


}

void Object::Export(std::string filename){
    std::ofstream myObj;
    myObj.open(filename);
    myObj<<colorFile+"\n";
    myObj<<heightFile+"\n";
    myObj<<numBones<<"\n";
    myObj<<numFaces<<"\n";
    for(auto v:vertices)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:normals)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:tangents)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:texture)
        myObj<<v.x<<" "<<v.y<<" "<<"\n";
    for(auto v: weights)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<"\n";
    for(auto v: weightindices)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<"\n";
    myObj.close();
}
void Object::Import(std::string filename){
    std::ifstream myObj;
    myObj.open(filename);
    colorFile   =filename+"Tex.png";   color = new Texture(colorFile.c_str());
    heightFile  =filename+"Bump.png";  height= new Texture(heightFile.c_str());
    myObj>>numFaces;
    myObj>>numBones;
    myObj>>numKeyframes;
    vertices        .clear();
    normals         .clear();
    tangents        .clear();
    texture         .clear();
    weights         .clear();
    weightindices   .clear();

#define readVec2(v) numbers>>v.x;numbers>>v.y;
#define readVec3(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;
#define readVec4(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;numbers>>v.w;
    std::string m;          //por qué hace falta esto??
    std::getline(myObj, m);

    for(int b=0;b<numBones;b++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        Bone bone;
        numbers>>bone.parent;
        readVec3(bone.center)
        while(!numbers.eof()){
            glm::vec3 v;
            readVec3(v);
            bone.axis.push_back(v);
            bone.parameters.push_back(0);
        }
        skeleton.push_back(bone);
    }

    time=0;
    for(int b=0;b<numKeyframes;b++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        Keyframe keyframe;
        numbers>>keyframe.time;
        while(!numbers.eof()){
            float p;
            numbers>>p;
            keyframe.parameters.push_back(p);
        }
        animation.push_back(keyframe);
    }

    for(int i=0;i<numFaces*3;i++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        glm::vec3 X;    readVec3(X);
        glm::vec3 N;    readVec3(N);
        glm::vec3 Tn;   readVec3(Tn);
        glm::vec2 T;    readVec2(T);
        glm::vec4 W;    readVec4(W);
        glm::vec4 I;    readVec4(I);

        vertices        .push_back(X);
        normals         .push_back(N);
        tangents        .push_back(Tn);
        texture         .push_back(T);
        weights         .push_back(W);
        weightindices   .push_back(I);
    }
    myObj.close();

    normalize();
    toBuffers();

}

void Object::draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,
                  int attrDeform,int unifColorTex, int unifHeightTex, int attrWeights, int attrIndices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(sh->properties[attrVertex], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glVertexAttribPointer(sh->properties[attrNormal], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
    glVertexAttribPointer(sh->properties[attrTangent],3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glVertexAttribPointer(sh->properties[attrTexture],2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_weights);
    glVertexAttribPointer(sh->properties[attrWeights],4, GL_FLOAT, GL_FALSE, 4*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_weightindices);
    glVertexAttribPointer(sh->properties[attrIndices],4, GL_FLOAT, GL_FALSE, 4*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_deform);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &deform[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(sh->properties[attrDeform],3, GL_FLOAT, GL_FALSE, 3*sizeof(float),0);

  //  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    color->bind(sh->properties[unifColorTex]);
    height->bind(sh->properties[unifHeightTex]);

    glDrawArrays(GL_TRIANGLES, 0, numFaces*3);
}

void Object::normalize(){
    glm::vec3 mean(0,0,0);
    float largestCoordinate=0;
    for(int i=0;i<numFaces*3;i++)   mean+=vertices[i];

    mean/=numFaces*3;

    for(int i=0;i<numFaces*3;i++)   vertices[i]-=mean;
    if(numBones>0) for(int j=0;j<skeleton.size();j++) skeleton[j].center-=mean;
    for(int i=0;i<numFaces*3;i++)   if(glm::length(vertices[i])>largestCoordinate) largestCoordinate=glm::length(vertices[i]);
    for(int i=0;i<numFaces*3;i++)   vertices[i]/=largestCoordinate;
    if(numBones>0) for(int j=0;j<skeleton.size();j++) skeleton[j].center/=largestCoordinate;
}
void Object::traslate(float x, float y, float z){
    for(int i=0;i<numFaces*3;i++)   vertices[i] +=glm::vec3(x,y,z);
    if(numBones>0)
        for(int i=0;i<skeleton.size();i++) skeleton[i].center+=glm::vec3(x,y,z);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Object::scale  (float s){
    for(int i=0;i<numFaces*3;i++)   vertices[i] *=s;
    if(numBones>0)
        for(int i=0;i<skeleton.size();i++) skeleton[i].center *=s;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Object::toBuffers(){
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_tangents);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &tangents[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*numFaces*3, &texture[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_weights);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_weights);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*numFaces*3, &weights[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_weightindices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_weightindices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*numFaces*3, &weightindices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_deform);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_deform);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &deform[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Object::animate(){
    time = (time==animation.back().time) ? animation.front().time : time+1;

    int keyframe=0;
    while(time>animation[keyframe].time)
        keyframe++;

    std::vector<float> parameters;

    for(int i=0; i<animation[keyframe].parameters.size();i++){
        if(keyframe>0){
            float before = animation[keyframe-1].parameters[i];
            float after  = animation[keyframe]  .parameters[i];
            float wb     = animation[keyframe]  .time-time;
            float wa     = time-animation[keyframe-1].time;
            float sum    = wb+wa;
            wb/=sum;
            wa/=sum;
            parameters.push_back(wb*before+wa*after);
        }
        else parameters.push_back(animation[0].parameters[i]);
    }
    int paramindex=0;
    for(int b=0;b<skeleton.size();b++)
        for(int p=0;p<skeleton[b].parameters.size();p++){
            skeleton[b].parameters[p]=parameters[paramindex];
            paramindex++;
        }

}
