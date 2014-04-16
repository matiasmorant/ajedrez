#define GL_GLEXT_PROTOTYPES

#ifndef SHADER_H
#define SHADER_H

#include <GL/glu.h>
class Shader{

public:
    Shader(const char *vs, const char *fs);
    ~Shader();

	GLuint program;
	GLuint vertex;
	GLuint fragment;
    int attributeNumber;
	GLint*	properties;
	void	getProperties(char* list, int attributes, int uniforms);
    void	enableAttributes();
	char* shaderToString(const char* filename);
	void print_log(GLuint object);
	GLuint createShader(const char* filename, GLenum type);
};
#endif // SHADER_H
