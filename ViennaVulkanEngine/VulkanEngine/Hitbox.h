#ifndef HITBOX_H
#define HITBOX_H
class Hitbox
{
protected:
	ve::VESceneNode* hitbox; // for position
public:
	Hitbox(ve::VESceneNode* parent, std::string hitboxName);
	virtual ~Hitbox() = default;
	virtual bool isHit(const Ray& ray) const = 0;
};
#endif // !HITBOX_H

