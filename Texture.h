#define GL_GLEXT_PROTOTYPES

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glu.h>

class Texture{

public:
    Texture(const char *image,GLuint texUnit);
    ~Texture();

    GLuint name;
    GLuint tex_unit;

	void bind(GLint property);
};

#endif // TEXTURE_H
