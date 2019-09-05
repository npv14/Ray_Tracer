///*----------------------------------------------------------
//* COSC363  Ray Tracer
//*
//*  The object class
//*  This is a generic type for storing objects in the scene
//*  Sphere, Plane etc. must be defined as subclasses of Object.
//*  Being an abstract class, this class cannot be instantiated.
//-------------------------------------------------------------*/
#include "Cone.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 vdif = posn - center;

    float RdivH = (radius / height) * (radius / height);
    float a = pow(dir.x, 2) + pow(dir.z, 2) - RdivH * pow(dir.y, 2);
    float b = 2 * (dir.x * vdif.x + dir.z * vdif.z + RdivH*(height - posn.y + center.y)*dir.y) ;
    float c = pow(vdif.x, 2) + pow(vdif.z, 2) - (RdivH*(pow((height - posn.y + center.y),2)));

    float delta = b*b - (4*(a*c));

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / ( 2*a);
    float t2 = (-b + sqrt(delta)) / ( 2*a);
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;

    float t = (t1 < t2)? t1: t2;
    float m = posn.y + dir.y * t;

    return (center.y <= m) && (m <= height + center.y)? t : -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 d = p - center;
    float r = sqrt(d.x * d.x + d.z * d.z);
    glm::vec3 n = glm::vec3 (d.x,r*(radius/height),d.z);
    n = glm::normalize(n);
    return n;
}
