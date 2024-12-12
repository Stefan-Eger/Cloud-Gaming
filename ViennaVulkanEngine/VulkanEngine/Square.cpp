#include "ChessInclude.h"

size_t Square::squareCounter = 1;

Square::Square(VESceneNode* parent, const ChessCoordinates& pos, const Figure::Team& color) : m_pos{pos}, m_figure{nullptr}
{
	std::string squareName = "Square-" + std::to_string(Square::squareCounter++);
	std::string fileName;

	m_color = color;
	switch (color)
	{
	case Figure::Team::BLACK:
		fileName = BLACK_TILE_FILENAME;
		break;
	case Figure::Team::WHITE:
		fileName = WHITE_TILE_FILENAME;
		break;
	default:
		break;
	}

	//Normal Tile
	VESceneNode* e1, * eParent1;
	eParent1 = getSceneManagerPointer()->createSceneNode(squareName + "-Parent", parent, glm::mat4(1.0));
	VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel(squareName + "-Model", TILE_FILEPATH, fileName, AI_FLAGS));
	eParent1->multiplyTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.95f,1.0f,0.95f)));
	eParent1->multiplyTransform(glm::translate(glm::mat4(1.0f), pos.toVec3()));
	eParent1->addChild(e1);

	m_tileParent = eParent1;
	m_tile = findEntity(e1);

	m_tile->m_visible = true;

	//Highlighted Tile - Standard Move
	VESceneNode* e2, * eParent2;
	eParent2 = getSceneManagerPointer()->createSceneNode(squareName + "-StandardMove-Parent", parent, glm::mat4(1.0));
	VECHECKPOINTER(e2 = getSceneManagerPointer()->loadModel(squareName + "-StandardMove-Model", TILE_FILEPATH, STANDARD_MOVE_TILE_FILENAME, AI_FLAGS));
	eParent2->multiplyTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.95f, 1.0f, 0.95f)));
	eParent2->multiplyTransform(glm::translate(glm::mat4(1.0f), pos.toVec3()));
	eParent2->addChild(e2);

	m_tileStandardMoveParent = eParent2;
	m_tileStandardMove = findEntity(e2);
	m_tileStandardMove->m_visible = false;

	//Highlighted Tile - Special Move
	VESceneNode* e3, * eParent3;
	eParent3 = getSceneManagerPointer()->createSceneNode(squareName + "-SpecialMove-Parent", parent, glm::mat4(1.0));
	VECHECKPOINTER(e3 = getSceneManagerPointer()->loadModel(squareName + "-SpecialMove-Model", TILE_FILEPATH, SPECIAL_MOVE_TILE_FILENAME, AI_FLAGS));
	eParent3->multiplyTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.95f, 1.0f, 0.95f)));
	eParent3->multiplyTransform(glm::translate(glm::mat4(1.0f), pos.toVec3()));
	eParent3->addChild(e3);

	m_tileSpecialMoveParent = eParent3;
	m_tileSpecialMove = findEntity(e3);
	m_tileSpecialMove->m_visible = false;

	m_hitbox = new SphereHitbox(eParent1, squareName+"-Hitbox", SPHERE_HITBOX_RADIUS);
}

Hitbox* Square::getHitbox() const {
	return m_hitbox;
}
Square::~Square()
{
	if (m_hitbox != nullptr) {
		delete m_hitbox;
		m_hitbox = nullptr;
	}
}

VEEntity* Square::findEntity(VESceneNode* node) {

	VESceneNode* ret;
	for (ret = node; ret->getChildrenList().size() != 0;) {
		ret = ret->getChildrenList().at(0);
	}
	return dynamic_cast<VEEntity*>(ret);
}

void Square::setEmpty()
{
	m_figure = nullptr;
}

void Square::setFigure(Figure* figure)
{
	m_figure = figure;
}


void Square::setHighlighted(const SquareHighlightType& highlightSquare) {
	m_tile->m_visible = (highlightSquare == Square::SquareHighlightType::NONE);
	m_tileStandardMove->m_visible = (highlightSquare == Square::SquareHighlightType::STANDARD_MOVE);
	m_tileSpecialMove->m_visible = (highlightSquare == Square::SquareHighlightType::SPECIAL_MOVE);
	
	m_isHighlighted = highlightSquare;
}


ChessCoordinates Square::getPos() const
{
	return m_pos; 
}

void Square::setPos(const ChessCoordinates& pos)
{
	m_pos = pos;
	m_tileParent->setPosition(pos.toVec3());
	m_tileStandardMoveParent->setPosition(pos.toVec3());
	m_tileSpecialMoveParent->setPosition(pos.toVec3());
}

Square::SquareHighlightType Square::isHighlighted()
{
	return m_isHighlighted;
}

bool Square::isEmpty() const
{
	return (m_figure==nullptr);
}

Figure* Square::getFigure() const
{
	return m_figure;
}

void Square::setUnderThreat(const bool& isUnderThreat) {
	m_isUnderThreat = isUnderThreat;
}
bool Square::isUnderThreat() const {
	return m_isUnderThreat;
}