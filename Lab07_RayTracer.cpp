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
#include "Cone.h"
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
const float PHONGS_CONSTANT = 20;
const float PI=3.14159265359;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

TextureBMP texture;
TextureBMP worldTexture;

//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 background(0);
    glm::vec3 ambientCol(0.2,0.2,0.2);
    glm::vec3 light(10, 40, -3);
    glm::vec3 colorSum;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) {

		return background;
		};      //If there is no intersection return background colour


    //shadows
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal at point of intersection
    glm::vec3 lightVector = glm::normalize(light - ray.xpt); //normalized light vector
	float lightDist=glm::distance(ray.xpt,light);
    //shadow ray
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);


    float lDotn = glm::dot(lightVector, normalVector); //dot product

    glm::vec3 col;

    //specular reflections
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    float rDotv = glm::dot(reflVector, -ray.dir);

    //make rDotn at least 0
    rDotv = max(rDotv, 0.0f);
    
    //texture
	if(ray.xindex==3){
		float texcoords=(ray.xpt.x+50)/100;
		float texcoordt=(ray.xpt.y+50)/100;
		//disable specular on background
		rDotv=0;

		col=texture.getColorAt(texcoords,texcoordt);
		}
	
	else{
		if(ray.xindex==6){
			//texture sphere
			if ((int(ray.xpt.x) % 2 == 0) !=( int(ray.xpt.y)%2==1))
			 {
				 col = glm::vec3(0.2,0.2,0.2);
				 }
			else {
				col = glm::vec3(1,1,1);
				}
			}
		else{
		if(ray.xindex==7){
			//texture earth of radius 6
			glm::vec3 center(10,15,-90);
			glm::vec3 diff=ray.xpt-center;
			
			float s=atan(diff.y/diff.x);
			float t=atan(sqrt(diff.x*diff.x+diff.y*diff.y)/diff.z);
			s=s/(2*PI);
			t=t/PI;
			col=worldTexture.getColorAt(s,t);
			//cout<<diff.x<<" "<<diff.y<<" "<<diff.z<<" "<<s<<" "<<t<<"\n";
			}else{

		col = sceneObjects[ray.xindex]->getColor(); //else return object's colour
		}
		}
	}

    glm::vec3 v1(1);
    rDotv = pow(rDotv, PHONGS_CONSTANT);

    if((shadow.xindex > -1 && shadow.xdist<lightDist)|| lDotn < 0)
    {
        colorSum = ambientCol * col;
        //cout<<col.x<<" "<<col.y<<" "<<col.z;
        if(shadow.xindex==1){
			colorSum=colorSum+sceneObjects[4]->getColor()*glm::vec3(0.15);
		}
    }
    else
    {
        colorSum = ambientCol * col + lDotn * col + rDotv * v1;
    }

//reflection
    if((ray.xindex == 0) && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step + 1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);  
    }
//refraction
	if(ray.xindex==1 && step < MAX_STEPS){

		glm::vec3 g=glm::refract(ray.dir,normalVector,1.0f/1.01f);
		Ray refractedRay(ray.xpt, g);
		
		refractedRay.closestPt(sceneObjects);
		
		if(refractedRay.xindex==-1){
			return background;
		}
		glm::vec3 m=sceneObjects[refractedRay.xindex]->normal(refractedRay.xpt);
		glm::vec3 h=glm::refract(g,-m,1.01f);
		Ray refractedRay2(refractedRay.xpt,h);
		refractedRay2.closestPt(sceneObjects);
		if(refractedRay2.xindex==-1){
			return background;
		}
		return trace(refractedRay2,step++);
	}

        //return ambientCol * col + lDotn * col + rDotn * v1;
        return colorSum;

    }
    
    //--Anti-aliasing
glm::vec3 anti_aliasing(int step, float pixelSize, float xp, float yp){
	float center1=pixelSize/4.0f;
	float center2=center1+pixelSize/2.0f;
	glm::vec3 eye(0);
	float xc1=xp+center1;
	float yc1=yp+center1;
	
	float xc2=xp+center1;
	float yc2=yp+center2;
	
	float xc3=xp+center2;
	float yc3=yp+center1;
	
	float xc4=xp+center2;
	float yc4=yp+center2;
	
	glm::vec3 colorSum(0);
	
	
    Ray ray = Ray(eye, glm::vec3(xc1,yc1,-EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	 ray = Ray(eye, glm::vec3(xc2,yc2,-EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	 ray = Ray(eye, glm::vec3(xc3,yc3,-EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	 ray = Ray(eye, glm::vec3(xc4,yc4,-EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	return colorSum*glm::vec3(0.25);
	
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
                //glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
				glm::vec3 col = anti_aliasing(1,cellX,xp,yp);//anti aliasing
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

//--draws a box with location x,y,x and size width, height,depth
void drawBox(float x,float y,float z,float w,float h,float d,glm::vec3 color){
	    //--Add box    
	    glm::vec3 A(x,y+h,z);
	    glm::vec3 B(x,y,z);
	    glm::vec3 C(x+w,y,z);
	    glm::vec3 D(x+w,y+h,z);
	    
	    glm::vec3 E(x,y+h,z+d);
	    glm::vec3 F(x,y,z+d);
	    glm::vec3 G(x+w,y,z+d);
	    glm::vec3 H(x+w,y+h,z+d);
	    
		Plane *front = new Plane(A,B,C,D,color);
        sceneObjects.push_back(front);
        
        Plane *left = new Plane(A,E,F,B,color);
        sceneObjects.push_back(left);
        
        Plane *back = new Plane(F,G,H,E,color);
        sceneObjects.push_back(back);
        
        Plane *right = new Plane(D,H,G,C,color);
        sceneObjects.push_back(right);
        
        Plane *top = new Plane(E,H,D,A,color);
        sceneObjects.push_back(top);
        
        Plane *bottom = new Plane(B,C,G,F,color);
        sceneObjects.push_back(bottom);
        
        
	
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
		worldTexture=TextureBMP("worldMap.bmp");

        //-- Create a pointer to a sphere object
        Sphere *sphere1 = new Sphere(glm::vec3(-0.0, -0.0, -100.0), 5.0, glm::vec3(0, 0, 1));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(sphere1);

        //-- Create a pointer to a sphere object
        Sphere *sphere2 = new Sphere(glm::vec3(8.0, 0.0, -100.0), 3.0, glm::vec3(1, 0, 0));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(sphere2);

        //-- Create a pointer to a sphere object
        Cylinder *cylinder = new Cylinder(glm::vec3(10, -20, -90.0), 5.0,10.0, glm::vec3(0, 1, 0));

        //--Add the above to the list of scene objects.
        sceneObjects.push_back(cylinder);
        
        
        //--Add background plane xindex=3
		Plane *backplane = new Plane(glm::vec3(-50., -50, -200),//Point A
        glm::vec3(50., -50, -200),//Point B
        glm::vec3(50., 50, -200),//Point C
        glm::vec3(-50., 50, -200),//Point D
        glm::vec3(0.5, 0.5, 0));//Colour
        
        sceneObjects.push_back(backplane);
        
        //--Add floor
        Plane *floor = new Plane(glm::vec3(-50., -20, -40),//Point A
        glm::vec3(50., -20, -40),//Point B
        glm::vec3(50., -20, -180),//Point C
        glm::vec3(-50., -20, -180),//Point D
        glm::vec3(214/255.0f,197/255.0f,70/255.0f));//Colour
        
        sceneObjects.push_back(floor);
        
        
        //--Add cone
        Cone *cone = new Cone(glm::vec3(0., -20, -90),2.5,5,
        glm::vec3());//Colour
        
        sceneObjects.push_back(cone);
        
		//--add pattern sphere
		Sphere *sphere3=new Sphere(glm::vec3(-10,20,-100),5,glm::vec3(0,0,1));
		sceneObjects.push_back(sphere3);
		
		//--add earth sphere
		Sphere *earth=new Sphere(glm::vec3(10,15,-90),6,glm::vec3(0,0,0));
        sceneObjects.push_back(earth);
        
        //--add box
        drawBox(-20,-20,-120,5,5,5,glm::vec3(1));
        
        


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
