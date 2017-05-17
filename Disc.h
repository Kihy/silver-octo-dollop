
/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Disc class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_DISC
#define H_DISC

#include <glm/glm.hpp>
#include "SceneObject.h"

class Disc : public SceneObject
{
private:
    glm::vec3 center;  //The four vertices
    float radius;

public:	
	Disc();
	
    Disc(glm::vec3 pcenter, float pradius, glm::vec3 col)
		: center(pcenter),radius(pradius)
	{
		color = col;
	};

	bool isInside(glm::vec3 pt);
	
	float intersect(glm::vec3 posn, glm::vec3 dir);
	
	glm::vec3 normal(glm::vec3 pt);

};

#endif //!H_DISC
