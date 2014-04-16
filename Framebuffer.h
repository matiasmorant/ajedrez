#define GL_GLEXT_PROTOTYPES

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glu.h>
#include "Shader.h"

class Framebuffer{

public:
    Framebuffer(int width, int height, int n);
    Framebuffer(int width, int height);
    ~Framebuffer();

GLuint fbo, rbo_depth;
GLuint *fbo_texture;
GLuint vbo_fbo_vertices;
int N;

void draw(Shader* sh, int i);
void draw(Shader* sh);
void init(int width, int height, int n);
};
#endif // SHADER_H
