/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

#define eps 1e-3

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 diff=posn-center;
    float a=dir.x*dir.x+dir.z*dir.z;
    float b=2*(dir.x*diff.x+dir.z*diff.z);
    float c=diff.x*diff.x+diff.z*diff.z-radius*radius;
    
    float d=b*b-4*a*c;
    if(d<eps) return -1;
    
    float t1=(-b-sqrt(d))/(2*a);
    float t2=(-b+sqrt(d))/(2*a);
    
    float t=(t1 < t2)? t1: t2;
    
    float r = posn.y + t*dir.y;
    
    if ((r >= center.y) && (r <= center.y + height)) return t;
    else return -1;
    
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    
    glm::vec3 diff=p-center;
    glm::vec3 n(diff.x,0,diff.z);
    n = glm::normalize(n);
    return n;
}
