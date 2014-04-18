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

void Object::Export(std::string filename){
    std::ofstream myObj;
    myObj.open(filename);
    myObj<<colorFile+"\n";
    myObj<<heightFile+"\n";
    myObj<<numFaces<<"\n";
    for(auto v:vertices)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:normals)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:tangents)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:texture)
        myObj<<v.x<<" "<<v.y<<" "<<"\n";
    myObj.close();
}
void Object::Import(std::string filename){
    std::ifstream myObj;
    myObj.open(filename);
    setColorFile(filename+"Tex.png");
    setColorFile(filename+"Tex.png");
    setHeightFile(filename+"Bump.png");
    myObj>>numFaces;
    vertices        .clear();
    normals         .clear();
    tangents        .clear();
    texture         .clear();

#define readVec2(v) numbers>>v.x;numbers>>v.y;
#define readVec3(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;
#define readVec4(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;numbers>>v.w;
    std::string m;          //por qué hace falta esto??
    std::getline(myObj, m);

    for(int i=0;i<numFaces*3;i++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        glm::vec3 X;    readVec3(X);
        glm::vec3 N;    readVec3(N);
        glm::vec3 Tn;   readVec3(Tn);
        glm::vec2 T;    readVec2(T);

        vertices        .push_back(X);
        normals         .push_back(N);
        tangents        .push_back(Tn);
        texture         .push_back(T);
    }
    myObj.close();

    normalize();
    toBuffers();

}

void Object::draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture, int unifColorTex, int unifHeightTex)
{
    sh->disableAttributes();
    sh->enableAttributes(attrVertex);
    sh->enableAttributes(attrNormal);
    sh->enableAttributes(attrTangent);
    sh->enableAttributes(attrTexture);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(sh->properties[attrVertex], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glVertexAttribPointer(sh->properties[attrNormal], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
    glVertexAttribPointer(sh->properties[attrTangent],3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glVertexAttribPointer(sh->properties[attrTexture],2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);

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
    for(int i=0;i<numFaces*3;i++)   if(glm::length(vertices[i])>largestCoordinate) largestCoordinate=glm::length(vertices[i]);
    for(int i=0;i<numFaces*3;i++)   vertices[i]/=largestCoordinate;
}
void Object::traslate(float x, float y, float z){
    for(int i=0;i<numFaces*3;i++)   vertices[i] +=glm::vec3(x,y,z);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Object::traslate(glm::vec3 v){   traslate(v.x,v.y,v.z);}

void Object::scale  (float s){
    for(int i=0;i<numFaces*3;i++)   vertices[i] *=s;
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
}

void Object::setColorFile(std::string filename){
    colorFile   =filename; color = new Texture(colorFile.c_str(),0);
}
void Object::setHeightFile(std::string filename){
    heightFile   =filename;  height = new Texture(heightFile.c_str(),1);
}



AnimatedObject::AnimatedObject(char *filename){

        Import(filename);

}

void AnimatedObject::Export(std::string filename){
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
void AnimatedObject::Import(std::string filename){
    std::ifstream myObj;
    myObj.open(filename);
    setColorFile(filename+"Tex.png");
    setHeightFile(filename+"Bump.png");
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
        glm::vec3 c(0,0,0);
        readVec3(c)
        bone.setCenter(c);
        while(!numbers.eof()){
            glm::vec3 v;
            readVec3(v);
            bone.axis.push_back(v);
            bone.parameters.push_back(0);
        }
        bone.setIndex(skeleton.size());
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

void AnimatedObject::draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,
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

void AnimatedObject::normalize(){
    glm::vec3 mean(0,0,0);
    float largestCoordinate=0;
    for(int i=0;i<numFaces*3;i++)   mean+=vertices[i];

    mean/=numFaces*3;

    for(int i=0;i<numFaces*3;i++)   vertices[i]-=mean;
    if(numBones>0) for(int j=0;j<skeleton.size();j++) skeleton[j].setCenter(skeleton[j].center-mean);
    for(int i=0;i<numFaces*3;i++)   if(glm::length(vertices[i])>largestCoordinate) largestCoordinate=glm::length(vertices[i]);
    for(int i=0;i<numFaces*3;i++)   vertices[i]/=largestCoordinate;
    if(numBones>0) for(int j=0;j<skeleton.size();j++) skeleton[j].setCenter(skeleton[j].center/largestCoordinate);
}
void AnimatedObject::traslate(float x, float y, float z){
    for(int i=0;i<numFaces*3;i++)   vertices[i] +=glm::vec3(x,y,z);
    if(numBones>0)
        for(int i=0;i<skeleton.size();i++) skeleton[i].center+=glm::vec3(x,y,z);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void AnimatedObject::traslate(glm::vec3 v){
    traslate(v.x,v.y,v.z);
}

void AnimatedObject::scale  (float s){
    for(int i=0;i<numFaces*3;i++)   vertices[i] *=s;
    if(numBones>0)
        for(int i=0;i<skeleton.size();i++) skeleton[i].center *=s;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void AnimatedObject::toBuffers(){
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

void AnimatedObject::animate(){
    time = (time==animation.back().time) ? animation.front().time : time+1;

    int keyframe=0;
    while(time>animation[keyframe].time)
        keyframe++;

    std::vector<float> parameters;


//myslerp
    for(int i=0; i<animation[keyframe].parameters.size();i++){
        if(keyframe>0){
            float before = animation[keyframe-1].parameters[i];
            float after  = animation[keyframe]  .parameters[i];
            float wb     = animation[keyframe]  .time-time;
            float wa     = time-animation[keyframe-1].time;
            float sum    = wb+wa;
            wb/=sum;
            wa/=sum;
            parameters.push_back(glm::sin(wb*3.14159/2)*before+glm::sin(wa*3.14159/2)*after);
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

void AnimatedObject::setColorFile(std::string filename){
    colorFile   =filename; color = new Texture(colorFile.c_str(),0);
}
void AnimatedObject::setHeightFile(std::string filename){
    heightFile   =filename;  height = new Texture(heightFile.c_str(),1);
}
