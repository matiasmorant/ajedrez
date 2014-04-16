#define GL_GLEXT_PROTOTYPES
#include "Texture.h"
#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"
#include <GL/glu.h>

Texture::Texture(const char *image){

		SDL_Surface* texImage = IMG_Load(image);
		if(texImage==NULL){	printf("cant load image %s: %s\n",image,IMG_GetError());}
		else{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, // target
               0,  // level, 0 = base, no minimap,
               texImage->format->BytesPerPixel == 4? GL_RGBA : GL_RGB, // internalformat
               texImage->w,  // width
               texImage->h,  // height
               0,  // border, always 0 in OpenGL ES
               texImage->format->BytesPerPixel == 4? GL_RGBA : GL_RGB,  // format
               GL_UNSIGNED_BYTE, // type
               texImage->pixels);
		}
}

void Texture::bind(GLint property){
	glActiveTexture(GL_TEXTURE0+id);
	glBindTexture(GL_TEXTURE_2D, id);
	glUniform1i(property, /*GL_TEXTURE*/id);
}
