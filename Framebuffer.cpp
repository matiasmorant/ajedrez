#define GL_GLEXT_PROTOTYPES
#include "Framebuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/glu.h>
#include "Shader.h"

void Framebuffer::init(int width, int height, int n){

/* Create back-buffer, used for post-processing */
  /* Textures */
	N=n;
	fbo_texture = new GLuint[n];
	int i;
	for(i=0;i<n;i++){
	  glGenTextures(1, &(fbo_texture[i]));
	  glActiveTexture(GL_TEXTURE0+fbo_texture[i]);
	  glBindTexture(GL_TEXTURE_2D, fbo_texture[i]);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	  glBindTexture(GL_TEXTURE_2D, 0);
	} 
  /* Depth buffer */
  glGenRenderbuffers(1, &rbo_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
 
  /* Framebuffer to link everything together */
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for(i=0;i<n;i++){
	  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, fbo_texture[i], 0);}
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
  GLenum status;
  if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

//generate plane
  GLfloat fbo_vertices[] = {
    -1, -1,
     1, -1,
    -1,  1,
     1,  1,
  };
  glGenBuffers(1, &vbo_fbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Framebuffer::Framebuffer(int width, int height){		init(width,height, 1);}
Framebuffer::Framebuffer(int width, int height,int n){	init(width,height, n);}

void Framebuffer::draw(Shader* sh, int i){
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 // "v_coord fbo_texture"
  glUseProgram(sh->program);
  glActiveTexture(GL_TEXTURE0+fbo_texture[i]);
  glBindTexture(GL_TEXTURE_2D, fbo_texture[i]);
  glUniform1i(sh->properties[1], fbo_texture[i]);
  glEnableVertexAttribArray(sh->properties[0]);
 
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
  glVertexAttribPointer(
    sh->properties[0],  // attribute
    2,                  // number of elements per vertex, here (x,y)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableVertexAttribArray(sh->properties[0]);
}

void Framebuffer::draw(Shader* sh){ 

 glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 // "v_coord fbo_texture"
 // glUseProgram(sh->program);
int i;
for(int i=0;i<N;i++){
  glActiveTexture(GL_TEXTURE0+fbo_texture[i]);
  glBindTexture(GL_TEXTURE_2D, fbo_texture[i]);
  glUniform1i(sh->properties[i+1], fbo_texture[i]);
}
  glEnableVertexAttribArray(sh->properties[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
  glVertexAttribPointer(
    sh->properties[0],  // attribute
    2,                  // number of elements per vertex, here (x,y)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableVertexAttribArray(sh->properties[0]);

}
