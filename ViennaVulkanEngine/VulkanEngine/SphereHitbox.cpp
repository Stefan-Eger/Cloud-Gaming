#include "ChessInclude.h"

SphereHitbox::SphereHitbox(ve::VESceneNode* parent, std::string hitboxName, const float& radius) : Hitbox(parent, hitboxName), radius{radius}, radiusSquared{radius*radius}
{
}
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection (14.06.2023)
//intersect Method builds on this Math Part
//As the Tutorial described
//First calculate l vector to Center of Sphere then map l onto ray with dot Product
//then calculate d2 (d^2) with pythagoras (l.dot(l) == l^2) l^2 - tca^2
//now check if d2 is bigger than radius2 (square should not be an issue)
//then calculate thc again with pythagoras  d2 and radius squared guarantees it on the surface of a sphere
bool SphereHitbox::isHit(const Ray& ray) const
{

    glm::vec3 pos = this->hitbox->getWorldTransform()[3];
    //std::cout << "Pos-Figure: [" << pos.x << ", " << pos.y << ", " << pos.z << "]" << std::endl;
    glm::vec3 l =  pos - ray.getOrigin();
    float tca = glm::dot(l, ray.getDirection());
    if (tca < 0) { return false; } // is direction of ray correct( has to be bigger than 0)

    float dSquared = glm::dot(l,l) - (tca * tca); // now d > r is equivivalent t d^2 > r^2
    if (dSquared > radiusSquared) { return false; }


    return true;
    
}
