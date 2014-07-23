#ifndef ANT_H
#define ANT_H

#include<Object.h>
#include <QDebug>
#include <Utilities.h>

class Ant : public AnimatedObject
{
public:
    Ant();
    Ant(int x,int y, int side):AnimatedObject("ant2"){
        setup(x,y,side);
    }

    float speed;

    void setup(){
  //      rotate(1,0,0,-90);
        rotate(0,0,1,-90);
        scale(0.3);
        traslate(0,0,0.22);
    }

    void setup(int x,int y, int side){

        position = new glm::vec3[1];
        *position=glm::vec3(x,y,0);
        model_matrix = new glm::mat4[1];
        *model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3((position->x-1.)*2.2/7-1.1,(position->y-1.)*2.2/7.-1.1,0)) *
                        glm::rotate(glm::mat4(1.0f),(float)(90-180*side),glm::vec3(0,0,1));

        speed=0.1*(1-2*side);
    }

    virtual void animate(bool calcDeforms=true){

        AnimatedObject::animate(calcDeforms);

        *position=glm::vec3(position->x+speed,position->y,0);
        *model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3((speed)*2.2/7,0,(0)*2.2/7.)) * (*model_matrix);

        /*qDebug()<<"position: ("<<position->x<<" "<<position->y<<" "<<position->z<<")";
   #define m(i) <<"("<<(*model_matrix)[i].x<<"\t\t"<<(*model_matrix)[i].y<<"\t\t"<<(*model_matrix)[i].z<<"\t\t"<<(*model_matrix)[i].w<<")"
        qDebug()<<"matrix  :\n"m(0)<<"\n"m(1)<<"\n"m(2)<<"\n"m(3);*/
    }
};

#endif // ANT_H
