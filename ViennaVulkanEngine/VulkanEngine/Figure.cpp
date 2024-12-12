#include "ChessInclude.h"

size_t Figure::figureCounter = 1;

Figure::Figure(ve::VESceneNode* parent, const Team& team, const Figure_t& type, Square* pos) : m_Parent{ parent }, m_Team{ team }, m_Type{ type }, m_pos{ pos }, m_hitbox{nullptr} {
}

Figure::~Figure()
{
	if (m_hitbox != nullptr) {
		delete m_hitbox;
		m_hitbox = nullptr;
	}
}

void Figure::createSceneNode()
{
	//WHICH TEAM -> for right color
	std::string teamPrefix; 
	std::string basedir; 
	float startTeamRot; 
	switch (m_Team)
	{
	case Team::BLACK:
		teamPrefix = "Black"; 
		basedir = BLACK_FILEPATH; 
		startTeamRot = BLACK_STARTING_ROTATION; 
		break;
	case Team::WHITE: 
		teamPrefix = "White"; 
		basedir = WHITE_FILEPATH; 
		startTeamRot = WHITE_STARTING_ROTATION; 
		break;
	default:
		break;
	}
	//WHICH Figure to create
	std::string filename;
	std::string nodeName;
	std::string postFix = "-" + std::to_string(Figure::figureCounter++) + "-";
	switch (m_Type)
	{
	case Figure::Figure_t::PAWN: 
		filename = PAWN_FILENAME; 
		nodeName = "PAWN";
		break; 
	case Figure::Figure_t::ROOK: 
		filename = ROOK_FILENAME;
		nodeName = "ROOK";
		break; 
	case Figure::Figure_t::KNIGHT: 
		filename = KNIGHT_FILENAME;
		nodeName = "KNIGHT";
		break; 
	case Figure::Figure_t::BISHOP: 
		filename = BISHOP_FILENAME;
		nodeName = "BISHOP";
		break; 
	case Figure::Figure_t::QUEEN: 
		filename = QUEEN_FILENAME;
		nodeName = "QUEEN";
		break;
	case Figure::Figure_t::KING: 
		filename = KING_FILENAME;
		nodeName = "KING";
		break;
	default:
		filename = "unknown";
		nodeName = "unknown";
		break;
	}
	m_FigureName = teamPrefix + nodeName + postFix;

	//CREATING THE CHESSPIECE
	VESceneNode* e1, * eParent;
	eParent = getSceneManagerPointer()->createSceneNode(teamPrefix + nodeName + postFix + "Parent", m_Parent, glm::mat4(1.0)); 
	VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel(teamPrefix + nodeName + postFix + "Model", basedir, filename, AI_FLAGS));

	glm::mat4 startRot = glm::rotate(glm::mat4(1.0f), ((startTeamRot / 180.0f) * (float)M_PI), glm::vec3(0.0, 1.0, 0.0));
	eParent->multiplyTransform(startRot);
	eParent->multiplyTransform(glm::translate(glm::mat4(1.0f), m_pos->getPos().toVec3()));
	eParent->addChild(e1);
	m_Figure = eParent;


	//CREATING HITBOX FOR CHESSPIECE

	if (m_hitbox != nullptr) {
		delete m_hitbox;
		m_hitbox = nullptr;
	}
	m_hitbox = new SphereHitbox(eParent, teamPrefix + nodeName + postFix + "Hitbox", SPHERE_HITBOX_RADIUS);

}

Square* Figure::getPos() const
{
	return m_pos;
}

Hitbox* Figure::getHitbox() const
{
	return m_hitbox;
}


Figure::Team Figure::getTeam() const
{ 
	return m_Team; 
} 

Figure::Figure_t Figure::getType() const
{
	return m_Type;
}

std::string Figure::getName() const
{
	return m_FigureName;
}

bool Figure::isFirstMove() const
{
	return !m_moves;
}

size_t Figure::getMoves() const
{
	return m_moves;
}

//Otherwise in Destructor the m_Figure value only contains garbage
void Figure::deleteNode()
{
	if (m_Figure != nullptr) {
		getSceneManagerPointer()->deleteSceneNodeAndChildren(m_Figure->getName());
	}
}

void Figure::undoMove()
{
	auto target = m_prevPositions.back();
	m_pos->setEmpty();
	m_pos = target; 
	target->setFigure(this);
	m_Figure->setPosition(target->getPos().toVec3());
	--m_moves;
	m_prevPositions.pop_back();
}

void Figure::moveTo(Square* target)
{
	m_pos->setEmpty();
	m_prevPositions.push_back(m_pos);
	m_pos = target;
	target->setFigure(this);
	m_Figure->setPosition(target->getPos().toVec3());
	++m_moves;
}
