#ifndef SQUARE_H
#define SQUARE_H

using namespace ve;
class Figure;
class Square
{
public:
	enum class SquareHighlightType {
		NONE,
		STANDARD_MOVE,
		SPECIAL_MOVE
	};
private:
	
	Figure::Team m_color;
	VESceneNode* m_tileParent;
	VEEntity* m_tile;
	VESceneNode* m_tileStandardMoveParent;
	VEEntity* m_tileStandardMove;
	VESceneNode* m_tileSpecialMoveParent;
	VEEntity* m_tileSpecialMove;
	const std::string TILE_FILEPATH = "media/models/Game/Tile";

	const std::string WHITE_TILE_FILENAME = "TileWhite.obj";
	const std::string BLACK_TILE_FILENAME = "TileBlack.obj";
	const std::string STANDARD_MOVE_TILE_FILENAME = "TileRed.obj";
	const std::string SPECIAL_MOVE_TILE_FILENAME = "TileBlue.obj";

	const uint32_t AI_FLAGS = aiProcess_FlipWindingOrder | aiProcess_FlipUVs;

	const float SPHERE_HITBOX_RADIUS = 0.5f;

	Figure* m_figure;
	ChessCoordinates m_pos;
	SquareHighlightType m_isHighlighted = SquareHighlightType::NONE;
	bool m_isUnderThreat = false;

	Hitbox* m_hitbox = nullptr;

	static size_t squareCounter;

	VEEntity* findEntity(VESceneNode* node);
public:
	Square(VESceneNode* parent, const ChessCoordinates& pos, const Figure::Team& color);
	~Square();
	Hitbox* getHitbox() const;

	void setEmpty();
	void setFigure(Figure* figure);
	Figure* getFigure()const;
	bool isEmpty()const;
	void setHighlighted(const SquareHighlightType& highlightSquare);
	SquareHighlightType isHighlighted();

	ChessCoordinates getPos() const; 
	void setPos(const ChessCoordinates& pos);

	void setUnderThreat(const bool& isUnderThreat);
	bool isUnderThreat() const;
};

#endif