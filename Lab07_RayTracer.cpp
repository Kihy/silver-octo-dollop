/*========================================================================
* COSC 363  Computer Graphics (2017)
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
#include <GL/glut.h>
#include "Plane.h"
#include "Cylinder.h"
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float PHONGS_CONSTANT = 10;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

TextureBMP texture;

//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 background(0);
    glm::vec3 ambientCol(0.2);
    glm::vec3 light(10, 40, -3);
    glm::vec3 colorSum;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) {
		//float texcoords=(ray.xpt.x)/1024.0f;
		//float texcoordt=(ray.xpt.y)/768.0f;
		return background;
		};      //If there is no intersection return background colour

    //shadows
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal at point of intersection
    glm::vec3 lightVector = glm::normalize(light - ray.xpt); //normalized light vector

    //shadow ray
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);


    float lDotn = glm::dot(lightVector, normalVector); //dot product

    glm::vec3 col = sceneObjects[ray.xindex]->getColor(); //else return object's colour



    //specular reflections
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    float rDotv = glm::dot(reflVector, -ray.dir);

    //make rDotn at least 0
    rDotv = max(rDotv, 0.0f);

    glm::vec3 v1(1);
    rDotv = pow(rDotv, PHONGS_CONSTANT);

    if(shadow.xindex > -1 || lDotn < 0)
    {
        colorSum = ambientCol * col;
    }
    else
    {
        colorSum = ambientCol * col + lDotn * col + rDotv * v1;
    }

    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step + 1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);  
    }


        //return ambientCol * col + lDotn * col + rDotn * v1;
        return colorSum;

    }

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
    void display()
    {
        float xp, yp;  //grid point
        float cellX = (XMAX - XMIN) / NUMDIV; //cell width
        float cellY = (YMAX - YMIN) / NUMDIV; //cell height

        glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_QUADS);  //Each cell is a quad.

        for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
        {
            xp = XMIN + i * cellX;
            for(int j = 0; j < NUMDIV; j++)
            {
                yp = YMIN + j * cellY;

                glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray

                Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
                ray.normalize();				//Normalize the direction of the ray to a unit vector
                glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

                glColor3f(col.r, col.g, col.b);
                glVertex2f(xp, yp);				//Draw each cell with its color value
                glVertex2f(xp + cellX, yp);
                glVertex2f(xp + cellX, yp + cellY);
                glVertex2f(xp, yp + cellY);
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
    void initialize()
    {
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
        glClearColor(0, 0, 0, 1);

		//--create texture
		texture=TextureBMP("background.bmp");

        //-- Create a pointer to a sphere object
        Sphere *sphere1 = new Sphere(glm::vec3(-0.0, -0.0, -100.0), 5.0, glm::vec3(0, 0, 1));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(sphere1);

        //-- Create a pointer to a sphere object
        Sphere *sphere2 = new Sphere(glm::vec3(3.0, 4.0, -70.0), 3.0, glm::vec3(1, 0, 0));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(sphere2);

        //-- Create a pointer to a sphere object
        Cylinder *cylinder = new Cylinder(glm::vec3(-10, -20, -90.0), 5.0,10.0, glm::vec3(0, 1, 0));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(cylinder);
        
        //--Add Plane 
		Plane *plane = new Plane(glm::vec3(-20., -20, -200),//Point A
        glm::vec3(20., -20, -200),//Point B
        glm::vec3(20., 20, -200),//Point C
        glm::vec3(-20., 20, -200),//Point D
        glm::vec3(0.5, 0.5, 0));//Colour
        
        sceneObjects.push_back(plane);

    }



    int main(int argc, char *argv[])
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
        glutInitWindowSize(600, 600);
        glutInitWindowPosition(20, 20);
        glutCreateWindow("Raytracer");
	
        glutDisplayFunc(display);
        initialize();

        glutMainLoop();
        return 0;
    }
