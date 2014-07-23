#define GL_GLEXT_PROTOTYPES

#ifndef SHADER_H
#define SHADER_H

#include <GL/glu.h>
#include <unordered_map>
#include <map>
class Shader{

public:
    Shader(const char *vs, const char *fs);
    ~Shader();

	GLuint program;
	GLuint vertex;
	GLuint fragment;
    int attributeNumber;
    std::unordered_map<std::string,GLint> attributes;
    std::unordered_map<std::string,GLint> uniforms;
    void	getProperties(char* list, int attributeNum, int uniformNum);
    void	enableAttributes();
    void	enableAttributes(std::string attributeName);
    void	disableAttributes();
    void	disableAttributes(std::string attributeName);
	char* shaderToString(const char* filename);
	void print_log(GLuint object);
	GLuint createShader(const char* filename, GLenum type);
};
#endif // SHADER_H
