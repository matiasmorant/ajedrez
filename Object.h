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
class Bone{
public:
    Bone(){center=glm::vec3(0,0,0); parent=-1;size=0.1;}
    ~Bone(){}
    glm::vec3 center;
    std::vector<glm::vec3> axis;
    std::vector<float> parameters;
    glm::mat3 matrix;
//    std::vector<glm::vec3>  vertices;
//    std::vector<glm::vec3>  normals;
    float size;
    int parent;
    void buildMatrix(){
        glm::vec3 v(0,0,0);
        for(int i=0;i<axis.size();i++) v+=parameters[i]*axis[i];
        if(glm::length(v)>0)
                matrix=glm::mat3(glm::rotate(glm::mat4(1.0f), (float)glm::length(v), glm::normalize(v)));
        else    matrix=glm::mat3(1.0);
    }
//    void makeBone(){
//        vertices.push_back(glm::vec3());

//    }
};

class Keyframe{
public:
    Keyframe(){}
    ~Keyframe(){}
    int time;
    std::vector<float> parameters;
};



class Object //: public Shape
{
public:
    Object(char *filename);
    Object(char *filename,char* colorImg, char* heightImg);
//    ~Object();

    Texture* color;
    Texture* height;
    std::string colorFile;
    std::string heightFile;
    std::vector<glm::vec3>  vertices;
    std::vector<glm::vec3>  normals;
    std::vector<glm::vec3>  tangents;
    std::vector<glm::vec2>  texture;
    std::vector<glm::vec4>  weights;
    std::vector<glm::vec4>  weightindices;
    std::vector<glm::vec3>  deform;
    std::vector<Bone>       skeleton;
    std::vector<Keyframe>   animation;
    int time;
    int numFaces;
    int numBones;
    int numKeyframes;

    unsigned int vbo_vertices, vbo_normals, vbo_tangents, vbo_texture, vbo_deform, vbo_weights,vbo_weightindices;

    void Export(std::string filename);
    void Import(std::string filename);
    void draw(Shader* sh, int attrVertex, int attrNormal, int attrTangent, int attrTexture,
              int attrDeform, int unifColorTex, int unifHeightTex, int attrWeights, int attrIndices);
    void normalize();
    void traslate   (float x, float y, float z);
    void scale      (float s);
    void rotate     (float x, float y, float z, float w);
    void toBuffers();
    void buildMatrices(){ for(int i=0;i<skeleton.size();i++) skeleton[i].buildMatrix();}
    void animate();

};
