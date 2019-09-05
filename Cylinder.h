///*----------------------------------------------------------
//* COSC363  Ray Tracer
//*
//*  The ray class
//-------------------------------------------------------------*/


#include "SceneObject.h"
#ifndef H_CYLINDER
#define H_CYLINDER
#include <glm/glm.hpp>


/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Cylinder : public SceneObject
{

private:
    glm::vec3 center;
    float radius;
    float height;

public:

    Cylinder(glm::vec3 c, float r, float h, glm::vec3 col)
                   : center(c), radius(r), height(h)
           {
                   color = col;
           }

    float intersect(glm::vec3 posn, glm::vec3 dir);

    glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER
