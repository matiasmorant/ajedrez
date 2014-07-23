#ifndef UTILITIES_H
#define UTILITIES_H


#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QElapsedTimer>
#include <QDebug>


std::string check_for_glError();
void check_for_glError(std::string pointMessage);
std::string check_for_errno();
float angle_between(glm::vec3 u, glm::vec3 v);
class Timer{

    public:
        Timer(){}
        QElapsedTimer qtimer;
        qint64 last;
        qint64 time(){
            last=qtimer.restart();
            return last;
        }
        qint64 ntime(){
            last=qtimer.nsecsElapsed();
            qtimer.start();
            return last;
        }

};

#endif // UTILITIES_H
