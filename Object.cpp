#include "Object.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <QElapsedTimer>
#include <QDebug>
#include "Utilities.h"

Timer timer4;

Object::Object(){}
Object::Object(char *filename,bool NORMALIZE){

    vertices     = new std::vector<glm::vec3>[1];
    normals      = new std::vector<glm::vec3>[1];
    tangents     = new std::vector<glm::vec3>[1];
    texture      = new std::vector<glm::vec2>[1];
    position     = new glm::vec3             [1];
    vbo_vertices = new unsigned int          [1];
    vbo_normals  = new unsigned int          [1];
    vbo_tangents = new unsigned int          [1];
    vbo_texture  = new unsigned int          [1];
    numFaces     = new int                   [1];
    model_matrix = new glm::mat4             [1];

        Import(filename,NORMALIZE);
}

void Object::Export(std::string filename){
    std::ofstream myObj;
    myObj.open(filename);
    myObj<<*colorFile+"\n";
    myObj<<*heightFile+"\n";
    myObj<<*numFaces<<"\n";
    for(auto v:*vertices)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:*normals)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:*tangents)
        myObj<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
    for(auto v:*texture)
        myObj<<v.x<<" "<<v.y<<" "<<"\n";
    myObj.close();
}
void Object::Import(std::string filename, bool NORMALIZE){
    std::ifstream myObj;
    myObj.open(filename);
    setColorFile(filename+"Tex.png");
    setColorFile(filename+"Tex.png");
    setHeightFile(filename+"Bump.png");
    myObj>>*numFaces;
    vertices    ->clear();
    normals     ->clear();
    tangents    ->clear();
    texture     ->clear();

#define readVec2(v) numbers>>v.x;numbers>>v.y;
#define readVec3(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;
#define readVec4(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;numbers>>v.w;
    std::string m;          //por qué hace falta esto??
    std::getline(myObj, m);

    for(int i=0;i<(*numFaces)*3;i++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        glm::vec3 X;    readVec3(X);
        glm::vec3 N;    readVec3(N);
        glm::vec3 Tn;   readVec3(Tn);
        glm::vec2 T;    readVec2(T);

        vertices        ->push_back(X);
        normals         ->push_back(N);
        tangents        ->push_back(Tn);
        texture         ->push_back(T);
    }
    myObj.close();

    if(NORMALIZE) normalize();
    toBuffers();

}

void Object::draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture, int unifColorTex, int unifHeightTex,bool draw)
{
    sh->disableAttributes();
    sh->enableAttributes(attrVertex);
    sh->enableAttributes(attrNormal);
    sh->enableAttributes(attrTangent);
    sh->enableAttributes(attrTexture);

    glBindBuffer(GL_ARRAY_BUFFER, (*vbo_vertices));
    glVertexAttribPointer(sh->properties[attrVertex], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, (*vbo_normals));
    glVertexAttribPointer(sh->properties[attrNormal], 3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo_tangents);
    glVertexAttribPointer(sh->properties[attrTangent],3, GL_FLOAT, GL_FALSE,3*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo_texture);
    glVertexAttribPointer(sh->properties[attrTexture],2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);

  //  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    color->bind(sh->properties[unifColorTex]);
    height->bind(sh->properties[unifHeightTex]);

    if(draw)
        glDrawArrays(GL_TRIANGLES, 0, (*numFaces)*3);
}



glm::vec4 Object::normalize(){
    glm::vec3 mean(0,0,0);
    float largestCoordinate=0;
    for(int i=0;i<(*numFaces)*3;i++)   mean+=vertices->at(i);
    mean/=(*numFaces)*3;
    traslate(-mean);
    for(int i=0;i<(*numFaces)*3;i++)   if(glm::length(vertices->at(i))>largestCoordinate) largestCoordinate=glm::length(vertices->at(i));
    scale(1/largestCoordinate);

    return glm::vec4(mean,1/largestCoordinate);
}

void Object::traslate(float x, float y, float z){
    for(int i=0;i<(*numFaces)*3;i++)   vertices->at(i) +=glm::vec3(x,y,z);
    toBuffers();
}

void Object::traslate(glm::vec3 v){   traslate(v.x,v.y,v.z);}

void Object::scale  (float s){
    for(int i=0;i<(*numFaces)*3;i++)   vertices->at(i) *=s;
    toBuffers();
}
void Object::rotate(float x, float y, float z, float w){
    glm::mat3 rotation_matrix=glm::mat3(glm::rotate(glm::mat4(1.0),w,glm::vec3(x,y,z)));
    for(int i=0;i<(*numFaces)*3;i++){
        vertices->at(i) = rotation_matrix * vertices->at(i);
        normals ->at(i) = rotation_matrix * normals ->at(i);
        tangents->at(i) = rotation_matrix * tangents->at(i);
    }
    toBuffers();

}

void Object::rotate     (glm::vec3 v){
    rotate(v.x,v.y,v.z,glm::length(v));
}
void Object::rotate     (glm::vec4 v){
    rotate(v.x,v.y,v.z,v.w);
}
void Object::copy(Object* o){

    color       =o->color;
    height      =o->height;
    colorFile   =o->colorFile;
    heightFile  =o->heightFile;
    vertices    =o->vertices;
    normals     =o->normals;
    tangents    =o->tangents;
    texture     =o->texture;
    numFaces    =o->numFaces;
    position    =o->position;

    vbo_vertices=o->vbo_vertices;
    vbo_normals =o->vbo_normals;
    vbo_tangents=o->vbo_tangents;
    vbo_texture =o->vbo_texture;

}

void Object::alloc(){

    vertices     = new std::vector<glm::vec3>[1];
    normals      = new std::vector<glm::vec3>[1] ;
    tangents     = new std::vector<glm::vec3>[1];
    texture      = new std::vector<glm::vec2>[1] ;
    position     = new glm::vec3             [1];
    vbo_vertices = new unsigned int          [1];
    vbo_normals  = new unsigned int          [1];
    vbo_tangents = new unsigned int          [1];
    vbo_texture  = new unsigned int          [1];
    numFaces     = new int                   [1];
}

void Object::toBuffers(){
    glGenBuffers(1, vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*(*numFaces)*3, &(vertices->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*(*numFaces)*3, &(normals->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, vbo_tangents);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_tangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*(*numFaces)*3, &(tangents->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*(*numFaces)*3, &(texture->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Object::setColorFile(std::string filename){
    colorFile   = new std::string(filename); color = new Texture(colorFile->c_str(),0);
}
void Object::setHeightFile(std::string filename){
    heightFile  = new std::string(filename); height = new Texture(heightFile->c_str(),1);
}



AnimatedObject::AnimatedObject(char *filename){


    Import(filename);

}

void AnimatedObject::alloc(){
    Object::alloc();
    weights             = new std::vector<glm::vec4>  [1] ;
    weightindices       = new std::vector<glm::vec4>  [1] ;
    deform              = new std::vector<glm::vec3>  [1] ;
    skeleton            = new std::vector<Bone>       [1] ;
    animation           = new std::vector<Keyframe>   [1] ;
    numBones            = new int                     [1] ;
    numKeyframes        = new int                     [1] ;
    vbo_deform          = new unsigned int[1];
    vbo_weights         = new unsigned int[1];
    vbo_weightindices   = new unsigned int[1];;
    numBones            = new int[1];
    numKeyframes        = new int[1];

}

void AnimatedObject::Export(std::string filename){
    /*std::ofstream myObj;
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
    myObj.close();*/
}
void AnimatedObject::Import(std::string filename){
    alloc();
    std::ifstream myObj;
    myObj.open(filename);
    setColorFile(filename+"Tex.png");
    setHeightFile(filename+"Bump.png");


    myObj>>(*numFaces);
    myObj>>(*numBones);
    myObj>>(*numKeyframes);
    vertices        ->clear();
    normals         ->clear();
    tangents        ->clear();
    texture         ->clear();
    weights         ->clear();
    weightindices   ->clear();

#define readVec2(v) numbers>>v.x;numbers>>v.y;
#define readVec3(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;
#define readVec4(v) numbers>>v.x;numbers>>v.y;numbers>>v.z;numbers>>v.w;
    std::string m;          //por qué hace falta esto??
    std::getline(myObj, m);

    for(int b=0;b<(*numBones);b++){
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
        bone.setIndex(skeleton->size());
        skeleton->push_back(bone);
    }

    time=0;
    for(int b=0;b<(*numKeyframes);b++){
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
        animation->push_back(keyframe);
    }

    for(int i=0;i<(*numFaces)*3;i++){
        std::string line;
        std::getline(myObj, line);
        std::istringstream numbers (line);
        glm::vec3 X;    readVec3(X);
        glm::vec3 N;    readVec3(N);
        glm::vec3 Tn;   readVec3(Tn);
        glm::vec2 T;    readVec2(T);
        glm::vec4 W;    readVec4(W);
        glm::vec4 I;    readVec4(I);

        vertices        ->push_back(X);
        normals         ->push_back(N);
        tangents        ->push_back(Tn);
        texture         ->push_back(T);
        weights         ->push_back(W);
        weightindices   ->push_back(I);
    }
    myObj.close();

    normalize();
    toBuffers();

}

void AnimatedObject::draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,
                  int attrDeform,int unifColorTex, int unifHeightTex, int attrWeights, int attrIndices,bool drawSkeleton)
{

    Object::draw(sh, attrVertex,  attrNormal,  attrTangent,  attrTexture,
                 unifColorTex,  unifHeightTex,false);

  //  sh->enableAttributes(attrDeform);
    sh->enableAttributes(attrWeights);
    sh->enableAttributes(attrIndices);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo_weights);
    glVertexAttribPointer(sh->properties[attrWeights],4, GL_FLOAT, GL_FALSE, 4*sizeof(float),0);

    glBindBuffer(GL_ARRAY_BUFFER, *vbo_weightindices);
    glVertexAttribPointer(sh->properties[attrIndices],4, GL_FLOAT, GL_FALSE, 4*sizeof(float),0);
/*
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_deform);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*(*numFaces)*3, &(deform->at(0)), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(sh->properties[attrDeform],3, GL_FLOAT, GL_FALSE, 3*sizeof(float),0);
*/
  //  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glFlush(); glFinish();
//    qDebug()<<"\tbind: "<<    timer4.ntime();
    glDrawArrays(GL_TRIANGLES, 0, (*numFaces)*3);
/*
    if(drawSkeleton)
        for(int i=0;i<skeleton->size();i++)
            skeleton->at(i).cube->draw(sh,attrVertex,attrNormal,attrTangent,attrTexture,unifColorTex,unifHeightTex);
*///    glFlush(); glFinish();
  //  qDebug()<<"\tdraw: "<<    timer4.ntime();

}
void AnimatedObject::traslate(float x, float y, float z){
    Object::traslate(x,y,z);
    if((*numBones)>0)
        for(int i=0;i<skeleton->size();i++) skeleton->at(i).setCenter(skeleton->at(i).center+glm::vec3(x,y,z));
}

void AnimatedObject::traslate(glm::vec3 v){
    traslate(v.x,v.y,v.z);
}

void AnimatedObject::scale  (float s){
    Object::scale(s);
    if((*numBones)>0)
        for(int i=0;i<skeleton->size();i++) skeleton->at(i).setCenter(s*skeleton->at(i).center);
}

void AnimatedObject::rotate(float x, float y, float z, float w){
    Object::rotate(x,  y,  z,  w);
    glm::mat3 rotation_matrix=glm::mat3(glm::rotate(glm::mat4(1.0),w,glm::vec3(x,y,z)));

    if((*numBones)>0)
        for(int i=0;i<skeleton->size();i++){
            skeleton->at(i).setCenter(rotation_matrix * skeleton->at(i).center);
            for(int a=0;a<skeleton->at(i).axis.size();a++)
                skeleton->at(i).axis[a]=rotation_matrix * skeleton->at(i).axis[a];
        }
}
void AnimatedObject::rotate(glm::vec3 v){
    rotate(v.x,v.y,v.z,glm::length(v));
}
void AnimatedObject::rotate(glm::vec4 v){
    rotate(v.x,v.y,v.z,v.w);
}

void AnimatedObject::toBuffers(){

    Object::toBuffers();

    glGenBuffers(1, vbo_weights);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_weights);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(*numFaces)*3, &(weights->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, vbo_weightindices);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_weightindices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(*numFaces)*3, &(weightindices->at(0)), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//needed in cpu anim
    /*
    glGenBuffers(1, vbo_deform);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_deform);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numFaces*3, &deform[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
*/
}

void AnimatedObject::animate(bool calcDeforms){
//timer.time();
    time = (time==animation->back().time) ? animation->front().time : time+1;

    int keyframe=0;
    while(time>animation->at(keyframe).time)
        keyframe++;

    std::vector<float> parameters;
//qDebug()<<"init:"<<    timer.ntime();
//slerp

    int paramindex=0;
    for(int b=0;b<skeleton->size();b++){
        glm::vec3 result;
        if(keyframe>0){
            glm::vec3 before(0,0,0);
            glm::vec3 after(0,0,0);
            for(int p=0;p<skeleton->at(b).axis.size();p++){
                before  += animation->at(keyframe-1).parameters[paramindex] * skeleton->at(b).axis[p];
                after   += animation->at(keyframe  ).parameters[paramindex] * skeleton->at(b).axis[p];
                paramindex++;
            }

            float angle= angle_between(before, after);
            float t = (float)(time-animation->at(keyframe-1).time)/(animation->at(keyframe).time-animation->at(keyframe-1).time);

            #define sinNearZero (angle<0.1 || (angle-3.14159)*(angle-3.14159)<0.1)

            float wb = sinNearZero ? (1-t) :glm::sin((1-t)*angle)/glm::sin(angle);
            float wa = sinNearZero ?   (t) :glm::sin(  (t)*angle)/glm::sin(angle);

            result=wb*before+wa*after;
        }
        else{
            result=glm::vec3(0,0,0);
            for(int p=0;p<skeleton->at(b).axis.size();p++){
                result  += animation->at(0).parameters[paramindex] * skeleton->at(b).axis[p];
                paramindex++;
            }
        }

        if(glm::length(result)>0)
                skeleton->at(b).matrix=glm::mat3(glm::rotate(glm::mat4(1.0f), (float)glm::length(result), glm::normalize(result)));
        else    skeleton->at(b).matrix=glm::mat3(1.0);
    }
//qDebug()<<"slerp:"<<    timer.ntime();

    // calculate deforms

#define center(k)   skeleton->at(k).center
#define bones(k)    skeleton->at(k).matrix
#define parents(k)  skeleton->at(k).parent
#define W weights->at(i)
#define I (int)weightindices->at(i)
#define V vertices->at(i)
#define T(k,v) bones(k)*(v-center(k))+center(k)

    if(calcDeforms){
        deform->clear();

        for(int i=0;i<(*numFaces)*3;i++){
            glm::vec3 d(0,0,0);
            if(W.x>0){
                int bone =I.x;
                glm::vec3 component = V;
                while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
                d +=W.x * component;
            }
            if(W.y>0){
                int bone =I.y;
                glm::vec3 component = V;
                while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
                d +=W.y * component;
            }
            if(W.z>0){
                int bone =I.z;
                glm::vec3 component = V;
                while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
                d +=W.z * component;
            }
            if(W.w>0){
                int bone =I.w;
                glm::vec3 component = V;
                while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
                d +=W.w * component;
            }
            d-=V;

            deform->push_back(d);
        }
//qDebug()<<"deforms:"<<    timer.ntime();
    }else{


#define bone(k)             skeleton->at(k)
    #define bone_matrix(k)      skeleton->at(k).matrix

    for(int i=0;i<skeleton->size();i++){
        bone(i).m=glm::mat3(1.0f);
        bone(i).b=glm::vec3(0,0,0);
        int p = i;
        while(p!=-1){  bone(i).m = bone(p).matrix*bone(i).m; p=parents(p);}
            p = i;
        while(p!=-1){  bone(i).b = bone_matrix(p)*(bone(i).b-center(p))+center(p); p=parents(p);}
    }

    }
}
void AnimatedObject::copy(AnimatedObject* o){

    Object::copy(o);
    weights         =o->weights;
    weightindices   =o->weightindices;
    deform          =o->deform;
    skeleton        =o->skeleton;
    animation       =o->animation;
    numBones        =o->numBones;
    numKeyframes    =o->numKeyframes;

    vbo_deform          =o->vbo_deform;
    vbo_weights         =o->vbo_weights;
    vbo_weightindices   =o->vbo_weightindices;
    time                =o->time;
    deform->clear();
    deform->resize((*numFaces)*3,glm::vec3(0,0,0));



}

void AnimatedObject::setup(){}
void AnimatedObject::setup(int x,int y, int side){}
