/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Plane.h"
#include <GL/glut.h>
using namespace std;
TextureBMP texture, texture1, texture2;
const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 1000;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

float eta = 1.01;
float direction = 0.1;
vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene
//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//   closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------

glm::vec3 trace(Ray ray, int step)
{
    float specularTerm;
    float specularTerm1;

    glm::vec3 backgroundCol(0);
    glm::vec3 colorSum;

    glm::vec3 light(20, 40, -3);
    glm::vec3 light1(30, 60, 50);

	glm::vec3 ambientCol(0.2);   //Ambient color of light
    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray
    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = light - ray.xpt;
    glm::vec3 lightVector1 = light1 - ray.xpt;

    glm::vec3 unit_vector = glm::normalize(lightVector);
    glm::vec3 unit_vector1 = glm::normalize(lightVector1);

    float lDotn = glm::dot(unit_vector, normalVector);
    float lDotn1 = glm::dot(unit_vector1, normalVector);

    glm::vec3 reflVector = glm::reflect(-unit_vector, normalVector);
    glm::vec3 reflVector1 = glm::reflect(-unit_vector1, normalVector);

    glm::vec3 viewVector =  - ray.dir;
    float rDotv = glm::dot(reflVector, viewVector);
    float rDotv1 = glm::dot(reflVector1, viewVector);

    Ray shadow(ray.xpt, unit_vector);
    shadow.closestPt(sceneObjects);

    Ray shadow1(ray.xpt, unit_vector1);
    shadow1.closestPt(sceneObjects);


    if(rDotv < 0){
        specularTerm = 0.0;
    }
    else{
        specularTerm = pow(rDotv, 15);
    }

    if(rDotv1 < 0){
        specularTerm1 = 0.0;
    }
    else{
        specularTerm1 = pow(rDotv1, 15);
    }

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour

    if (ray.xindex == 3){
        glm::vec3 n = normalize(normalVector);
        float s = n.x /(2 * M_PI) + 0.5;
        float t = n.y /(2 * M_PI) + 0.5;
        materialCol = texture2.getColorAt(s,t);
    }

    if(ray.xindex == 7)
    {
        float s = (ray.xpt.x + 60) / 110;
        float t = (ray.xpt.y + 20) / 70;
        materialCol = texture.getColorAt(s,t);
    }

//    ---Cilinder texture---

    if(ray.xindex == 5)
    {
        if ((int(ray.xpt.y + ray.xpt.x) % 2 == 0)){
            materialCol = glm::vec3(0.133, 0.431, 0.945);
        }else{
            materialCol = glm::vec3(0.894, 0.105, 0.156);
        }
    }


    if(ray.xindex == 4)
    {
        int x = (int)((ray.xpt.x +60 ) / 4) % 2;
        int z = (int)((ray.xpt.z)/ 4) % 2;

        if((x && z) || (!x && !z)){
             materialCol   = glm::vec3(0.2,0.2,0.2);}
        else{
             materialCol = glm::vec3(1,1,1);
        }
    }


//    if (lDotn >= 0){
//        colorSum =  ambientCol*materialCol + lDotn*materialCol + specularTerm;
//    }
//    else{
//        colorSum = ambientCol*materialCol + specularTerm;
//    }



//    float lDotn = glm::dot(unit_vector, normalVector);
    // the shadow

    float lightDist = glm::length(lightVector);
    float lightDist1 = glm::length(lightVector1);

    if (lDotn <= 0 ){
        colorSum  = ambientCol * materialCol;
    }

    if (lDotn1 <= 0 ){
        colorSum  = ambientCol * materialCol;
    }


    if(shadow.xindex>-1 && shadow.xdist < lightDist){
         colorSum  = ambientCol * materialCol;
        if(shadow.xindex == 2){
            colorSum += (lDotn*materialCol + specularTerm)*glm::vec3(0.6)+sceneObjects[2]->getColor()*glm::vec3(0.3);
        }
        if( shadow.xindex == 1){
            colorSum += (lDotn*materialCol + specularTerm)*glm::vec3(0.8);
        }
    }

    else if (shadow1.xindex>-1 && shadow1.xdist < lightDist1){
       colorSum  = ambientCol * materialCol;
       if(shadow1.xindex == 2){

           glm::vec3 sphereCol = sceneObjects[2]->getColor()*glm::vec3(0.3);
           colorSum += (lDotn1*materialCol + specularTerm1)*glm::vec3(1)+ sphereCol;
       }
       if( shadow1.xindex == 1){
           colorSum += (lDotn1*materialCol + specularTerm1)*glm::vec3(0.8);
       }

    }
    else{
           colorSum = ambientCol*materialCol+ lDotn*materialCol + specularTerm +specularTerm1;
//           colorSum = ambientCol*materialCol+ lDotn1*materialCol + specularTerm1;
    }



    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);
    }

    if(ray.xindex == 2)
    {
        Ray stranp(ray.xpt, ray.dir);
        glm::vec3 stranCol = trace(stranp, 1);
        colorSum = colorSum + (0.5f*stranCol);
    }

    if(ray.xindex == 6)
    {
        float s = (ray.xpt.x + 60) / 110 ;
        float t = (ray.xpt.y + 20) / 70 ;
        return texture1.getColorAt(s,t);
    }



    if(ray.xindex == 1)
    {
       glm::vec3 g = glm::refract(ray.dir, normalVector, 1.0f/eta);
       Ray refrRay1(ray.xpt, g);
       refrRay1.closestPt(sceneObjects);
       glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
       glm::vec3 h = glm::refract(g, -m, eta);
       Ray refracRay2(refrRay1.xpt,h);
       refracRay2.closestPt(sceneObjects);
       glm::vec3 refracCol = trace(refracRay2, 1);
       colorSum =  refracCol + specularTerm1 * glm::vec3(1.0f) ;
    }


    return colorSum;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.15*cellX, yp+0.15*cellY, -EDIST);
            glm::vec3 dir1(xp+0.75*cellX, yp+0.25*cellY, -EDIST);
            glm::vec3 dir2(xp+0.25*cellX, yp+0.75*cellY, -EDIST);
            glm::vec3 dir3(xp+0.75*cellX, yp+0.75*cellY, -EDIST);
            Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            Ray ray1 = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            Ray ray2 = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            Ray ray3 = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
            ray.normalize();				//Normalize the direction of the ray to a unit vector
            ray1.normalize();				//Normalize the direction of the ray to a unit vector
            ray2.normalize();				//Normalize the direction of the ray to a unit vector
            ray3.normalize();				//Normalize the direction of the ray to a unit vector
            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
            glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
            glm::vec3 col2 = trace (ray2, 1); //Trace the primary ray and get the colour value
            glm::vec3 col3 = trace (ray3, 1); //Trace the primary ray and get the colour value

            glColor3f(((col.r + col1.r + col2.r + col3.r)/4), ((col.g + col1.g + col2.g + col3.g)/4), ((col.b + col1.b + col2.b + col3.b)/4));

//            glm::vec3 dir(xp+0.15*cellX, yp+0.15*cellY, -EDIST);	//direction of the primary ray
//            Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
//            ray.normalize();				//Normalize the direction of the ray to a unit vector
//            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
//            glColor3f(col.r, col.g, col.b);

			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }
    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void drawDiamond(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{

    glm::vec3 A = glm::vec3(x,y,z);
    glm::vec3 B = glm::vec3(x+l,y,z);
    glm::vec3 C = glm::vec3(x+l/2,y+h,z);
    glm::vec3 E = glm::vec3(x+l,y,z-w);
    glm::vec3 H = glm::vec3(x,y,z-w);

    Plane *plane1 = new Plane(A,B,C,C,color);
    Plane *plane2 = new Plane(B,E,C,C,color);
    Plane *plane3 = new Plane(E,H,C,C,color);
    Plane *plane4 = new Plane(H,A,C,C,color);


    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);

}

void drawCubeShear(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{

    glm::vec3 A = glm::vec3(x,y,z);
    glm::vec3 B = glm::vec3(x+l,y,z);
    glm::vec3 C = glm::vec3(x+l+1,y+h,z);
    glm::vec3 D = glm::vec3(x+1,y+h,z);
    glm::vec3 E = glm::vec3(x+l,y,z-w);
    glm::vec3 F = glm::vec3(x+l+1,y+h,z-w);
    glm::vec3 G = glm::vec3(x+1,y+h,z-w);
    glm::vec3 H = glm::vec3(x,y,z-w);

    Plane *plane1 = new Plane(A,B,C,D,color);
    Plane *plane2 = new Plane(B,E,F,C,color);
    Plane *plane3 = new Plane(E,H,G,F,color);
    Plane *plane4 = new Plane(D,G,H,A,color);
    Plane *plane5 = new Plane(D,C,F,G,color);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);
}

void drawCubeRotation(float x, float y, float z, float l, float w, float h, glm::vec3 color)
{

    glm::vec3 A = glm::vec3(x,y,z);
    glm::vec3 B = glm::vec3(x+l,y-1,z);
    glm::vec3 C = glm::vec3(x+l,y+h-1,z);
    glm::vec3 D = glm::vec3(x,y+h,z);
    glm::vec3 E = glm::vec3(x+l+1.5,y,z-w);
    glm::vec3 F = glm::vec3(x+l+1.5,y+h,z-w);
    glm::vec3 G = glm::vec3(x+1.5,y+h+1,z-w);
    glm::vec3 H = glm::vec3(x+1.5,y+1,z-w);

    Plane *plane1 = new Plane(A,B,C,D,color);
    Plane *plane2 = new Plane(B,E,F,C,color);
    Plane *plane3 = new Plane(E,H,G,F,color);
    Plane *plane4 = new Plane(D,G,H,A,color);
    Plane *plane5 = new Plane(D,C,F,G,color);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);

}



void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
//    -- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -120.0), 15.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(-10.0, -10.0, -80.0), 4.0, glm::vec3(1, 0, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(2, -8, -70.0), 3.0, glm::vec3(0.2, 0, 0.2));
    Sphere *sphere4 = new Sphere(glm::vec3(12, 10, -70.0), 3.0, glm::vec3(1, 1, 0));
    Cylinder *cylinder= new Cylinder(glm::vec3(20, -20, -150.0), 3, 20,  glm::vec3(1, 1, 0));
    Cone *cone= new Cone(glm::vec3(25, -20, -130.0), 3, 20,  glm::vec3(1, 1, 0));

    Plane *wall = new Plane(glm::vec3(-50., -20, -200),//Point A
                                glm::vec3(50., -20, -200),//Point B
                                glm::vec3(50., 50, -200),//Point C
                                glm::vec3(-50., 50, -200),//Point D
                                glm::vec3(0.2,0.2,0.2));

    Plane *plane = new Plane (glm::vec3(-50., -20, -40),
    //Point A
    glm::vec3(50., -20, -40),
    //Point B
    glm::vec3(50., -20, -200),
    //Point C
    glm::vec3(-50., -20, -200),
    //Point D
    glm::vec3(0.5, 0.5, 0));
    //Colour
    glm::vec3 color = glm::vec3(0,0,1);
//    --Add the above to the list of scene objects.
    texture = TextureBMP((char* const)"Sun.bmp");
    texture1 = TextureBMP((char* const)"image.bmp");
    texture2 = TextureBMP((char* const)"galaxy.bmp");
    sceneObjects.push_back(sphere1); //0
    sceneObjects.push_back(sphere2); //1
    sceneObjects.push_back(sphere3); //2
    sceneObjects.push_back(sphere4); //3
    sceneObjects.push_back(plane); //4
    sceneObjects.push_back(cylinder); //5
    sceneObjects.push_back(wall); //6
    sceneObjects.push_back(cone); //7
    drawCubeShear(5, -18,  -85, 3, 3, 3,  glm::vec3(1,0,0)); //6
    drawCubeRotation(10, -18,  -85, 3, 3, 3,  glm::vec3(0,0.9,0.3) );
    drawDiamond(16, -19,  -85, 4, 4, 5, color);

}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
