/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

#define eps 1e-3

/**
* Cone's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
	//constants
	float k=radius*radius/(height*height);
	glm::vec3 diff=posn-center;
	float dy=height-diff.y;
	
	float a=dir.x*dir.x+dir.z*dir.z-k*dir.y*dir.y;
	float b=2*(dir.x*diff.x+dir.z*diff.z+k*dir.y*dy);
	float c=diff.x*diff.x+diff.z*diff.z-k*dy*dy;
	
	float d=b*b-4*a*c;
    if(d<eps) return -1;
    
    float t1=(-b-sqrt(d))/(2*a);
    float t2=(-b+sqrt(d))/(2*a);
    
    float t= (t1 < t2)? t1: t2;
    
    float r = posn.y + t*dir.y;
    
    if ((r >= center.y) && (r <= center.y + height)) return t;
    else return -1;
    
    
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    
    glm::vec3 diff=p-center;
    float alpha=atan(diff.x/diff.z);
    float theta=atan(radius/height);
    
    glm::vec3 n(sin(alpha)*cos(theta),sin(theta),cos(alpha)*cos(theta));
    n=glm::normalize(n);
    return n;
}

