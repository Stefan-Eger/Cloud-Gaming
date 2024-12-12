#ifndef FIGURE_H
#define FIGURE_H

class Square;
class Figure
{
public:

	enum class Figure_t {
		PAWN,
		ROOK,
		KNIGHT,
		BISHOP,
		QUEEN,
		KING
	};
	enum class Team {
		BLACK,
		WHITE
	};


private:

	const std::string WHITE_FILEPATH = "media/models/Game/White";
	const std::string BLACK_FILEPATH = "media/models/Game/Black";

	const std::string PAWN_FILENAME = "Pawn.obj";
	const std::string ROOK_FILENAME = "Rook.obj";
	const std::string KNIGHT_FILENAME = "Knight.obj";
	const std::string BISHOP_FILENAME = "Bishop.obj";
	const std::string QUEEN_FILENAME = "Queen.obj";
	const std::string KING_FILENAME = "King.obj";

	const uint32_t AI_FLAGS = aiProcess_FlipWindingOrder | aiProcess_FlipUVs;

	const float BLACK_STARTING_ROTATION = 180.0f;  //degrees
	const float WHITE_STARTING_ROTATION = 0.0f;

	const float SPHERE_HITBOX_RADIUS = 0.7f;

	static size_t figureCounter;
	ve::VESceneNode* m_Parent;
	ve::VESceneNode* m_Figure;
	Hitbox* m_hitbox;
	Square* m_pos;

	Figure_t m_Type;
	Team m_Team;
	std::string m_FigureName;

	size_t m_moves = 0;

	std::vector<Square*> m_prevPositions;



public:
	Figure(ve::VESceneNode* parent, const Team& team, const Figure_t& type, Square* pos);
	~Figure();

	void createSceneNode();

	void moveTo(Square* target);
	Square* getPos() const;
	Hitbox* getHitbox() const;

	Team getTeam() const;
	Figure_t getType() const;
	std::string getName() const;
	bool isFirstMove()const;
	size_t getMoves()const;

	void deleteNode();

	void undoMove();

};

#endif

