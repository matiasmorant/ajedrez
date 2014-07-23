TEMPLATE = app
#CONFIG += console
#CONFIG -= qt
# include SDL
LIBS +=  -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread -lGLU -lGLEW -lSDL2_image

QMAKE_CXXFLAGS =-g -gdwarf-2 -std=c++11 -I/usr/local/include/SDL2 -D_REENTRANT #-gdwarf is to solve expression evaluator issue with g++4.8
#QMAKE_CXXFLAGS +=-std=c++11

QT += gui #qopengldebug
QT += core #qelapsedtimer

#-I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread
SOURCES += \
    Texture.cpp \
    Shader.cpp \
    objLoader.cpp \
    Framebuffer.cpp \
    Object.cpp \
    Skull.cpp \
    Skull.cpp \
    Utilities.cpp \
    background.cpp \
    Ant.cpp

HEADERS += \
    Texture.h \
    Shader.h \
    Framebuffer.h \
    Object.h \
    Skull.h \
    Utilities.h \
    background.h \
    Ant.h

OBJECTS_DIR = ../build_files
MOC_DIR = ../build_files
