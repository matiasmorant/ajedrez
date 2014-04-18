#include "Utilities.h"
#include <GL/glew.h>
#include <GL/glu.h>
std::string check_for_glError(){
    std::string errorString;
    GLenum errorState = glGetError();
    if(errorState!=GL_NO_ERROR){
        switch(errorState){
            case GL_INVALID_ENUM:
                errorString="GL_INVALID_ENUM";break;
            case GL_INVALID_VALUE:
                errorString="GL_INVALID_VALUE";break;
            case GL_INVALID_OPERATION:
                errorString="GL_INVALID_OPERATION";break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString="GL_INVALID_FRAMEBUFFER_OPERATION";break;
            case GL_OUT_OF_MEMORY:
                errorString="GL_OUT_OF_MEMORY";break;
        }
        std::cout<<errorString+"\n";
    }else errorString="GL_NO_ERROR";

    return errorString;

}

void check_for_glError(std::string pointMessage){
    std::cout<<pointMessage+": ";
    check_for_glError();
    std::cout<<"\n";
}

