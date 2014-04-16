#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL.h>
#include <SDL_opengl.h>
//#include "Object.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <Skull.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	640

    Object * skull;
    Object * board;
    Object * einstein;
    Object * boneDebug;
	Shader *texShader;
	Shader *mixShader;
//	Shader *post_shader;
    Framebuffer* texFramebuffer;
//	Framebuffer* mixFramebuffer;

	GLuint texNames;
	float Xrot;
	float Yrot;
	float Zrot;
	float wXrot;
	float wYrot;
	float wZrot;
	float X0;
	float Y0;
	float lastX;
	float lastY;

    float cameraTheta=0;
    float cameraPhi=3.14/4;

    int fichaL=1;
    int fichaR=1;

float angle=0;
int an=0;

std::vector<Skull> toyList;

void initializeGL()
{
	glewInit();

    einstein = new Object("einstein");
    boneDebug= new Object("boneDebug");
    einstein->skeleton[2].center+=glm::vec3(0.1,-0.3,0);
//    board=new Object("Board.obj","boardTexkrit2.png","boardBump.png"); board->scale(1.8);

    glEnable(GL_DEPTH_TEST);

	texShader	= new Shader("texShader.v","texShader.f");
	mixShader	= new Shader("mixShader.v","mixShader.f");
    texShader	->getProperties("coord3d normal tangent texture weights indices deform m v p m_tr_inv monkeyTex heightTex bones",7,7);
    mixShader	->getProperties("corners colorTex normalTex posTex view",1,4);

	texFramebuffer = new Framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT,3);

}

void texPass();

void paintGL()
{
    glm::mat4 model     =	glm::rotate(glm::mat4(1.0f), Xrot, glm::vec3(1, 0, 0))*
                            glm::rotate(glm::mat4(1.0f), Yrot, glm::vec3(0, 1, 0))*
                            glm::rotate(glm::mat4(1.0f), Zrot, glm::vec3(0, 0, 1));
    glm::vec3 cameraPos =   glm::vec3(4*sin(cameraTheta)*sin(cameraPhi), 4*cos(cameraPhi),4*cos(cameraTheta)*sin(cameraPhi));
    glm::vec3 cameraUp  =   glm::vec3(0.0,-1.0, 0.0);
    glm::mat4 view      =	glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0), cameraUp);
    glm::mat4 projection=   glm::perspective(45.0f, 1.0f*1/1, 0.1f, 5.0f);
    glm::mat3 m_tr_inv  =   glm::transpose(glm::inverse(glm::mat3(view*model)));

    glUseProgram(texShader->program);
//	"coord3d normal tangent texture weights indices m v p m_tr_inv monkeyTex heightTex"

    glUniformMatrix4fv(texShader->properties[7], 1, GL_FALSE, glm::value_ptr(model)		);
    glUniformMatrix4fv(texShader->properties[8], 1, GL_FALSE, glm::value_ptr(view)		);
    glUniformMatrix4fv(texShader->properties[9], 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix3fv(texShader->properties[10], 1, GL_FALSE, glm::value_ptr(m_tr_inv)	);
//    glUniformMatrix3fv(texShader->properties[13], 1, GL_FALSE, glm::value_ptr(bones[0])	);

	glBindFramebuffer(GL_FRAMEBUFFER, texFramebuffer->fbo);
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0+1,GL_COLOR_ATTACHMENT0+2};
	glDrawBuffers(3, buffers);
    texPass();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(mixShader->program);
    glUniformMatrix4fv(mixShader->properties[4], 1, GL_FALSE, glm::value_ptr(view)	);
    texFramebuffer->draw(mixShader);

	glFlush(); 
	Xrot+=wXrot;
	Yrot+=wYrot;
	Zrot+=wZrot;
}

void texPass(){

    glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texShader->enableAttributes();

    einstein->time=((an/10)%4)*8;
    einstein->animate();
//    einstein->skeleton[1].parameters[0]=(float)(20*(sin(4*an*3.14159/180)+1));
//    einstein->skeleton[2].parameters[0]=(float)an*10;
//    skull->skeleton[0].parameters[0]=(float)(20*(sin(4*an*3.14159/180)+1));
//    skull->skeleton[6].parameters[0]=(float)(45*sin(4*an*3.14159/180));
//    skull->skeleton[7].parameters[0]=(float)(45*sin(4*an*3.14159/180));
//    skull->skeleton[4].parameters[0]=(float)(45*sin(4*an*3.14159/180));
//    skull->skeleton[4].parameters[1]=(float)(45*cos(4*an*3.14159/180));
    einstein->buildMatrices();
    einstein->deform.clear();
#define center(k)   einstein->skeleton[k].center
#define bones(k)    einstein->skeleton[k].matrix
#define parents(k)  einstein->skeleton[k].parent
#define W einstein->weights[i]
#define I (int)einstein->weightindices[i]
#define V einstein->vertices[i]
#define T(k,v) bones(k)*(v-center(k))+center(k)
    for(int i=0;i<einstein->numFaces*3;i++){
        glm::vec3 deform(0,0,0);
        if(W.x>0){
            int bone =I.x;
            glm::vec3 component = V;
            while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
            deform +=W.x * component;
        }
        if(W.y>0){
            int bone =I.y;
            glm::vec3 component = V;
            while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
            deform +=W.y * component;
        }
        if(W.z>0){
            int bone =I.z;
            glm::vec3 component = V;
            while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
            deform +=W.z * component;
        }
        if(W.w>0){
            int bone =I.w;
            glm::vec3 component = V;
            while(bone!=-1){ component=T(bone,component); bone=parents(bone);}
            deform +=W.w * component;
        }
        deform-=V;

        einstein->deform.push_back(deform);
    }
//    board->draw(texShader,0,1,2,3,6,11,12,4,5);
    einstein->draw(texShader,0,1,2,3,6,11,12,4,5);
    boneDebug->draw(texShader,0,1,2,3,6,11,12,4,5);

    an= (an>359)? 0 : an+1;

    int i;
    for(i=0;i<5;i++) glDisableVertexAttribArray(texShader->properties[i]);
}
void finalPass(){

}
void mousePressEvent(SDL_MouseButtonEvent event)
{
	X0=event.x;
	Y0=event.y;	
}

void mouseReleaseEvent(SDL_MouseButtonEvent event)
{ 
	lastX=event.x;
	lastY=event.y;

	wYrot =(lastX-X0)/100;
	wXrot =(lastY-Y0)/100;
}

void keyPressEvent(SDL_KeyboardEvent event){

    if(event.keysym.sym==SDLK_KP_2)	if(cameraPhi<3.14/2)  cameraPhi+=0.02;
    if(event.keysym.sym==SDLK_KP_8)	if(cameraPhi>=0.02) cameraPhi-=0.02;
    if(event.keysym.sym==SDLK_KP_4)	cameraTheta+=0.02;
    if(event.keysym.sym==SDLK_KP_6) cameraTheta-=0.02;
    if(event.keysym.sym==SDLK_LEFT) fichaR-= fichaR>1 ? 1:0;
    if(event.keysym.sym==SDLK_RIGHT)fichaR+= fichaR<1 ? 1:0;
    if(event.keysym.sym==SDLK_a)    fichaL-= fichaL>1 ? 1:0;
    if(event.keysym.sym==SDLK_d)    fichaL+= fichaL<1 ? 1:0;
//    if(event.keysym.sym==SDLK_RETURN)


}


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

void initSDL()
{
	//Initialize SDL
	SDL_Init( SDL_INIT_VIDEO );
	//Use OpenGL 2.1
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
	//Create context
	if( SDL_GL_CreateContext( gWindow ) != NULL ){
		//Use Vsync
		if( SDL_GL_SetSwapInterval( 1 ) < 0 ) printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
	}
}

void close()
{
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	SDL_Quit();
}

int main( int argc, char* args[] )
{

	initSDL();
	initializeGL();

	SDL_Event e;
	bool quit = false;

	while( !quit )
	{
		while( SDL_PollEvent( &e ) != 0 )
		{ 
			switch( e.type ){
            case SDL_QUIT: 				quit = true; 				break;
            case SDL_MOUSEBUTTONDOWN:	mousePressEvent(e.button);	break;
            case SDL_MOUSEBUTTONUP:		mouseReleaseEvent(e.button);break;
            case SDL_KEYDOWN:           keyPressEvent(e.key);break;
   //        case SDL_KEYUP:
			}
		}

		paintGL();
		
		SDL_GL_SwapWindow( gWindow );
	}
	
	close();

	return 0;
}
