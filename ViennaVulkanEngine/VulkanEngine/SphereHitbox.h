#ifndef SPHERE_HITBOX_H
#define SPHERE_HITBOX_H

class SphereHitbox : public Hitbox
{

    float radiusSquared;
    float radius;

public:
    SphereHitbox(ve::VESceneNode* parent, std::string hitboxName, const float& radius);
    virtual bool isHit(const Ray& ray) const override;
    virtual ~SphereHitbox() = default;
};

#endif // !SPHERE_HITBOX_H
