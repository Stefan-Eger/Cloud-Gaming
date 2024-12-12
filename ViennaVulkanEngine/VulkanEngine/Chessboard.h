#ifndef CHESSBOARD_H
#define CHESSBOARD_H

using namespace ve;
using namespace cl;

class Chessboard
{

	struct UndoMove {
		bool figureWasCaptured = false;
		bool castleing = false;
		bool enPassant = false;
		bool isPromotionInProgress = false;
		Figure::Figure_t promotionType = Figure::Figure_t::PAWN;
		Figure* promotedFigure = nullptr;
	};
	std::vector<UndoMove*> m_undoHistory;


	VESceneNode* m_chessboard = nullptr;
	

	std::vector<std::vector<Square*>> m_board;

	Figure::Team m_otherTeam = Figure::Team::BLACK;
	Figure::Team m_turn = Figure::Team::WHITE;
	size_t m_round = 1; 
	Figure* m_selectedFigure = nullptr;


	std::vector<Figure*> m_Figures;
	std::vector<Figure*> m_capturedFigures;
	std::vector<Figure*> m_movedFigures;
	Square* m_CapturedSquare;
	//King Variables
	Figure* m_kingWhite = nullptr;
	Figure* m_kingBlack = nullptr;
	bool m_isKingCheck = false;
	bool m_isKingCheckMate = false;

	//Castleing Variables
	Figure* m_rookWhiteSmall = nullptr;
	Figure* m_rookWhiteBig = nullptr;
	Figure* m_rookBlackSmall = nullptr;
	Figure* m_rookBlackBig = nullptr;
	bool isCastleingBig = false;
	bool isCastleingSmall = false;

	//En Passant Variables
	bool isEnPassantLeft = false;
	bool isEnPassantRight = false;

	//Promotion Variable
	bool m_isPawnAtEnd = false;
	bool m_isPromotionInProgress = false;

	Figure* findFigure(const Ray& ray) const;
	void deselectFigure();
	void captureFigure(Figure* f);

	//Performing Actual Movement
	void performStandardMove(const ChessCoordinates& pos);
	void performSpecialMove(const ChessCoordinates& pos);
	void performCastleingSmall();
	void performCastleingBig();
	void performEnPassant();
	void performPromotion();

	//Setup for new Game
	void cleanBoard();
	void setupPawns();
	void setupRooks();
	void setupKnights();
	void setupBishops();
	void setupQueens();
	void setupKings();

	//Game
	bool nextRound();
	void prevRound();
	void newThreatAssessment();
	void clearThreat();
	void setSquareUnderThreat(Square* square, const Figure::Team& selectedFigureTeam);

	//King underThread
	bool isKingCheck(Figure* kingTeam);
	bool isKingCheckMate(const Figure::Team& kingTeam); // Winning Condition



	//Highlights Standard Moves
	void possibleMoves(Figure* selectedFigure, const bool& highlightSquare);
	void pawnMoves(Figure* selectedFigure, const bool& highlightSquare);
	void rookMoves(Figure* selectedFigure, const bool& highlightSquare);
	void knightMoves(Figure* selectedFigure, const bool& highlightSquare);
	void bishopMoves(Figure* selectedFigure, const bool& highlightSquare);
	void queenMoves(Figure* selectedFigure, const bool& highlightSquare);
	size_t kingMoves(Figure* selectedFigure, const bool& highlightSquare);

	//Highlights SpecialMoves
	void castleingSmall(Figure* selectedFigure);
	void castleingBig(Figure* selectedFigure);
	void enPassant(Figure* selectedFigure);
	void promotionMove(Figure* selectedFigure);

public:
	Chessboard(VESceneNode* chessboard);
	~Chessboard();

	void createNewGame();


	void selectFigure(const Ray& incomingRay);
	bool moveFigure(const Ray& incomingRay);
	bool promotion(const Figure::Figure_t& newFigureType);

	void undoLastMove();

	bool isWhiteKingCheckMate() const;
	bool isBlackKingCheckMate() const;
};

#endif