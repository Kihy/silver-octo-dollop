
/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Disc.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using
* member variables center and radius
*/
bool Disc::isInside(glm::vec3 pt)
{
	glm::vec3 diff=pt-center;
    //=== Complete this function ====
    
    return sqrt(diff.x*diff.x+diff.z*diff.z)<=radius;
}

/**
* Plane's intersection method.  The input is a ray (pos, dir).
*/
float Disc::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 n = normal(posn);
    glm::vec3 a(center.x+radius,center.y,center.z);
    glm::vec3 vdif = a - posn;
    float vdotn = glm::dot(dir, n);
    if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n) / vdotn;
    if(fabs(t) < 0.0001) return -1;
    glm::vec3 q = posn + dir * t;
    if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the plane's normal vector using
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Disc::normal(glm::vec3 pt)
{
    glm::vec3 n = glm::vec3(0,1,0);
    //=== Complete this function ====
    return n;
}



