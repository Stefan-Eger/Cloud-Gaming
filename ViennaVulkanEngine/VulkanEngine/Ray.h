#ifndef RAY_H
#define RAY_H

class Ray {
    glm::vec3 origin;
    glm::vec3 direction;

public:
    Ray(const glm::vec3& origin, const glm::vec3& direction);

    //Formula: RayPosition = Origin + alpha * direction
    glm::vec3 getPositionOnRay(const float& alpha) const;

    const glm::vec3& getOrigin() const;
    const glm::vec3& getDirection() const;
};


#endif //PROJECT_RAY_H
