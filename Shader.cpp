#define GL_GLEXT_PROTOTYPES
#include "Shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <GL/glu.h>

char* Shader::shaderToString(const char* filename){

	FILE* input = fopen(filename,"r");

	fseek(input,0,SEEK_END);
	long size = ftell(input);
	fseek(input,0,SEEK_SET);
	char* content = (char*) malloc((size_t) size+1);
	fread(content, 1, (size_t) size, input);
	fclose(input);
	content[size]='\0';
	return content;
	
}

/**
 * Display compilation errors from the OpenGL shader compiler
 */
void Shader::print_log(GLuint object)
{
  GLint log_length = 0;
  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }
 
  char* log = (char*)malloc(log_length);
 
  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);
 
  fprintf(stderr, "%s", log);
  free(log);
}


GLuint Shader::createShader(const char* filename, GLenum type){

	const char* source =shaderToString(filename);
	GLuint shaderID = glCreateShader(type);
	const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"
    "#define GLES2\n",
#else
    "#version 120\n",
#endif
    source };
	glShaderSource(shaderID,2,sources,NULL);
	free((void*)source);
	glCompileShader(shaderID);

//error?
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
    	fprintf(stderr, "%s:", filename);
    	print_log(shaderID);
    	glDeleteShader(shaderID);
    	return 0;
	}
//
	return shaderID;

}

Shader::Shader(const char *vs, const char *fs){

    vertex	 = createShader(vs,GL_VERTEX_SHADER);
    fragment = createShader(fs,GL_FRAGMENT_SHADER);

	GLint link_ok = GL_FALSE;
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      printf("glLinkProgram:\n");
    }
}

char* readWord(char* list, int n){
		int i;
		int count=1;
		for(i=0;count<n;i++){
			if(list[i]==' ') count++;
		}
		int length=1;
		while(list[i+length]!=' ' && list[i+length]!='\0') length++;
	char * word= new char[length+1];
		int j;		
		for(j=0;j<length;j++) word[j]=list[i+j];
		word[length]='\0';
	return word;
}


void Shader::getProperties(char* list, int attributeNum, int uniformNum){
    attributeNumber=attributeNum;

	int i;
    for(i=0;i<attributeNum;i++){
        std::string attribute_name(readWord(list,i+1));
        attributes[attribute_name] = glGetAttribLocation(program, attribute_name.data());
        if (attributes[attribute_name] == -1) {
          fprintf(stderr, "Could not find attribute %s\n", attribute_name.data());
        }
	}
    for(i=0;i<uniformNum;i++){
        std::string uniform_name(readWord(list,attributeNum+i+1));
        uniforms[uniform_name] = glGetUniformLocation(program, uniform_name.data());
        if (uniforms[uniform_name] == -1) {
          fprintf(stderr, "Could not find uniform %s\n", uniform_name.data());
        }
	}
}
void	Shader::enableAttributes(){
    for(auto &kv: attributes) glEnableVertexAttribArray(kv.second);
}

void	Shader::enableAttributes(std::string attributeName){
    glEnableVertexAttribArray(attributes[attributeName]);
}

void	Shader::disableAttributes(){
    for(auto &kv: attributes)  glDisableVertexAttribArray(kv.second);
}

void	Shader::disableAttributes(std::string attributeName){
    glDisableVertexAttribArray(attributes[attributeName]);
}
