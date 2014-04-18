#include "Texture.h"
#include "Shader.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Triangle{
public:
    glm::vec3* vd;
    std::vector<int> g;
};

class Object //: public Shape
{
public:
    Object();
    Object(char *filename);
//    ~Object();

    Texture* color;
    Texture* height;
    std::string colorFile;
    std::string heightFile;
    std::vector<glm::vec3>  vertices;
    std::vector<glm::vec3>  normals;
    std::vector<glm::vec3>  tangents;
    std::vector<glm::vec2>  texture;
    int numFaces;

    unsigned int vbo_vertices, vbo_normals, vbo_tangents, vbo_texture;

    void Export(std::string filename);
    void Import(std::string filename);
    void draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,int unifColorTex, int unifHeightTex,bool draw=true);
    glm::vec4 normalize();
    virtual void traslate   (float x, float y, float z);
    virtual void traslate   (glm::vec3 v);
    virtual void scale      (float s);
    virtual void rotate     (float x, float y, float z, float w);
    virtual void rotate     (glm::vec3 v);
    virtual void rotate     (glm::vec4 v);
    void toBuffers();
    void setColorFile (std::string filename);
    void setHeightFile(std::string filename);
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


class AnimatedObject : public Object
{
public:
    AnimatedObject(char *filename);

    std::vector<glm::vec4>  weights;
    std::vector<glm::vec4>  weightindices;
    std::vector<glm::vec3>  deform;
    std::vector<Bone>       skeleton;
    std::vector<Keyframe>   animation;
    int time;
    int numBones;
    int numKeyframes;

    unsigned int vbo_deform, vbo_weights,vbo_weightindices;

    void Export(std::string filename);
    void Import(std::string filename);
    void draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,
              int attrDeform, int unifColorTex, int unifHeightTex, int attrWeights, int attrIndices, bool drawSkeleton=false);

    virtual void traslate   (float x, float y, float z);
    virtual void traslate   (glm::vec3 v);
    virtual void scale      (float s);
    virtual void rotate     (float x, float y, float z, float w);
    virtual void rotate     (glm::vec3 v);
    virtual void rotate     (glm::vec4 v);
    void toBuffers();
    void buildMatrices(){
        for(int i=0;i<skeleton.size();i++)
            skeleton[i].buildMatrix();}
    void animate();
};
