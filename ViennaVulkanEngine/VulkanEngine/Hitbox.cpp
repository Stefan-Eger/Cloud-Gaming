#include "ChessInclude.h"

Hitbox::Hitbox(ve::VESceneNode* parent, std::string hitboxName)
{

	VESceneNode* pNode;
	VECHECKPOINTER(pNode = getSceneManagerPointer()->createSceneNode(hitboxName, parent));
	hitbox = pNode;
}
