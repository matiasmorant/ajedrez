#ifndef OBJECT_H
#define OBJECT_H

#include "Texture.h"
#include "Shader.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<Utilities.h>

class Triangle{
public:
    glm::vec3* vd;
    std::vector<int> g;
};

//template <class TYPE>
class Object
{
public:
    Object();
    Object(char *filename,bool NORMALIZE=true);
//    ~Object();

    Texture                 * color;
    Texture                 * height;
    std::string             * colorFile;
    std::string             * heightFile;
    std::vector<glm::vec3>  * vertices;
    std::vector<glm::vec3>  * normals;
    std::vector<glm::vec3>  * tangents;
    std::vector<glm::vec2>  * texture;
    int                     * numFaces;
    glm::vec3               * position;

    unsigned int* vbo_vertices, *vbo_normals, *vbo_tangents, *vbo_texture;
    glm::mat4* model_matrix;

    void Export(std::string filename);
    void Import(std::string filename,bool NORMALIZE=true);
    void draw(Shader* sh,bool draw=true);
    glm::vec4 normalize();
    virtual void traslate   (float x, float y, float z);
    virtual void traslate   (glm::vec3 v);
    virtual void scale      (float s);
    virtual void rotate     (float x, float y, float z, float w);
    virtual void rotate     (glm::vec3 v);
    virtual void rotate     (glm::vec4 v);
    void copy(Object* o);
    void alloc();
    void toBuffers();
    void setColorFile (std::string filename);
    void setHeightFile(std::string filename);
};

class DebugCube: public Object{
public:
    DebugCube():Object("boneDebug"){
        center=glm::vec3(0,0,0);
        scale(0.5*std::sqrt(3.0));
    }
    DebugCube(glm::vec3 c):Object("boneDebug"){
        center=glm::vec3(0,0,0);
        scale(0.5*std::sqrt(3.0));
        setCenter(c);
    }
    DebugCube(float size):Object("boneDebug"){
        center=glm::vec3(0,0,0);
        scale(0.5*std::sqrt(3.0));
        scale(size);
    }
    DebugCube(float size,glm::vec3 c):Object("boneDebug"){
        center=glm::vec3(0,0,0);
        scale(0.5*std::sqrt(3.0));
        scale(size);
        setCenter(c);
    }
    ~DebugCube(){}
    glm::vec3 center;
    int index;
    void setCenter(glm::vec3 c){
        traslate(c-center);
        center=c;
    }
    void setIndex(int i){
        index=i;
        std::string filename(1,i+'0');
        filename=filename+".png";
        setColorFile(filename);
    }

};

class Bone{
public:
    Bone(){
        center=glm::vec3(0,0,0);
        parent=-1;
        cube = new Object("boneDebug");
        cube->scale(0.2);
        cube->traslate(center);
    }
    ~Bone(){}
    glm::vec3 center;
    std::vector<glm::vec3> axis;
    std::vector<float> parameters;
    glm::mat3 matrix;
    glm::mat3 m;
    glm::vec3 b;
    int parent;
    int index;
    Object * cube;
    void buildMatrix(){
        glm::vec3 v(0,0,0);
        for(int i=0;i<axis.size();i++) v+=parameters[i]*axis[i];
        if(glm::length(v)>0)
                matrix=glm::mat3(glm::rotate(glm::mat4(1.0f), (float)glm::length(v), glm::normalize(v)));
        else    matrix=glm::mat3(1.0);
    }
    void setCenter(glm::vec3 c){
        cube->traslate(c-center);
        center=c;
    }
    void setIndex(int i){
        index=i;
        std::string filename(1,i+'0');
        filename=filename+".png";
        cube->setColorFile(filename);
    }
};

class Keyframe{
public:
    Keyframe(){}
    ~Keyframe(){}
    int time;
    std::vector<float> parameters;
};

//template <class TYPE>
class AnimatedObject : public Object//<TYPE>
{
public:
    AnimatedObject(){}
    AnimatedObject(char *filename);
Timer timer;
    std::vector<glm::vec4>  * weights;
    std::vector<glm::vec4>  * weightindices;
    std::vector<glm::vec3>  * deform;
    std::vector<Bone>       * skeleton;
    std::vector<Keyframe>   * animation;
    int                       time;
    int                     * numBones;
    int                     * numKeyframes;

    unsigned int* vbo_deform, *vbo_weights,*vbo_weightindices;

    void Export(std::string filename);
    void Import(std::string filename);
    void draw(Shader* sh, bool drawSkeleton=false);

    virtual void traslate   (float x, float y, float z);
    virtual void traslate   (glm::vec3 v);
    virtual void scale      (float s);
    virtual void rotate     (float x, float y, float z, float w);
    virtual void rotate     (glm::vec3 v);
    virtual void rotate     (glm::vec4 v);
    void toBuffers();

    void buildMatrices(){
        for(int i=0;i<skeleton->size();i++)
            skeleton->at(i).buildMatrix();}

    virtual void animate(bool calcDeforms=true);
    void copy(AnimatedObject* o);
    void alloc();
    virtual void setup();
    virtual void setup(int x,int y, int side);
};

#endif //OBJECT
