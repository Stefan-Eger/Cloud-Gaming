#include "ChessInclude.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin{ origin }, direction{ glm::normalize(direction) } {}

glm::vec3 Ray::getPositionOnRay(const float& alpha) const
{
	return (origin + alpha * direction);
}

const glm::vec3& Ray::getOrigin() const {
	return origin;
}

const glm::vec3& Ray::getDirection() const {
	return direction;
}


