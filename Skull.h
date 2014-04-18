#include<Object.h>

class Skull : public AnimatedObject
{
public:
    Skull():AnimatedObject("skull2.obj","skTex.png","skullBump.png"){

        scale(0.7); rotate(0,1,0,90.); traslate(-1.,0,1.1);
        position=glm::vec3(0,0,0);
    }

    glm::vec3 position;

};
