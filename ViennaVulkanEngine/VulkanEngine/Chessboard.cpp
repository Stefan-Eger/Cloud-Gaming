#include "ChessInclude.h"




void Chessboard::selectFigure(const Ray& incomingRay) {
	if (m_isKingCheckMate || m_isPromotionInProgress) { return; } 

	deselectFigure();
	m_selectedFigure = findFigure(incomingRay);
	if (m_selectedFigure == nullptr) { return; }

	possibleMoves(m_selectedFigure, true);
}

bool Chessboard::moveFigure(const Ray& incomingRay)
{
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			Hitbox* h = m_board[y][x]->getHitbox();
			bool isHit = h->isHit(incomingRay);

			if (isHit) {
				std::cout << "Square Selected: [x: "<< x+1 << ", y: " << y+1 << "]" << std::endl;
				bool moveWasSuccesful = false;
				//Peform Standard Move
				if (m_board[y][x]->isHighlighted() == Square::SquareHighlightType::STANDARD_MOVE) {
					performStandardMove(ChessCoordinates(x, y));
					m_movedFigures.push_back(m_selectedFigure);
					moveWasSuccesful = nextRound();
				}
				//Perform Special Move
				else if (m_board[y][x]->isHighlighted() == Square::SquareHighlightType::SPECIAL_MOVE) {
					performSpecialMove(ChessCoordinates(x, y));
					m_movedFigures.push_back(m_selectedFigure);
					moveWasSuccesful = nextRound();
				}

				

				if (!m_isPromotionInProgress) {
					deselectFigure();
				}
				return moveWasSuccesful;
			}
		}
	}
	deselectFigure();
	return false;
}


bool Chessboard::promotion(const Figure::Figure_t& newFigureType)
{
	ChessCoordinates pos = m_selectedFigure->getPos()->getPos();
	auto team = m_selectedFigure->getTeam();
	bool isOnLastField = team == Figure::Team::WHITE ? ((pos.Y) == 7 ? true : false) : ((pos.Y) == 0 ? true : false);


	if (m_isPromotionInProgress && (m_selectedFigure->getType() == Figure::Figure_t::PAWN)) {
		//Kill PAWN
		captureFigure(m_selectedFigure);

		//Create new Figure at pos of Pawn
		Figure* newFigure = new Figure(m_chessboard, team, newFigureType, m_board[pos.Y][pos.X]);
		m_board[pos.Y][pos.X]->setFigure(newFigure);
		m_Figures.push_back(newFigure);
		newFigure->createSceneNode();
		m_isPromotionInProgress = false;
		nextRound();
		deselectFigure();


		auto lastMove = m_undoHistory.back();
		lastMove->promotedFigure = newFigure;
		lastMove->promotionType = newFigureType;
		return true;
	}
	return false;

}

Figure* Chessboard::findFigure(const Ray& ray) const
{
	for (const auto& figure : m_Figures) {
		if (figure->getTeam() != m_turn) { continue; }
		
		Hitbox* h = figure->getHitbox();
		bool isHit = h->isHit(ray);

		if (isHit) {
			std::cout << "Figure Selected: " << figure->getName() << std::endl;
			return figure;
		}
	}
	return nullptr;
}

void Chessboard::deselectFigure()
{
	//deselect figure
	m_selectedFigure = nullptr;
	//Deselects all options
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			m_board[y][x]->setHighlighted(Square::SquareHighlightType::NONE);
		}
	}
}

void Chessboard::captureFigure(Figure* f)
{
	for (auto it = m_Figures.begin(); it != m_Figures.end();) {
		if (*it == f) {

			m_undoHistory.back()->figureWasCaptured = true;
			m_capturedFigures.push_back(*it);
			(*it)->moveTo(m_CapturedSquare);
			it = m_Figures.erase(it);
		}
		else ++it;
	}
}

void Chessboard::performStandardMove(const ChessCoordinates& pos)
{
	m_undoHistory.push_back(new UndoMove());
	
	//Kill figure if new Position contains enemy figure
	if (m_board[pos.Y][pos.X]->getFigure() != nullptr) {
		captureFigure(m_board[pos.Y][pos.X]->getFigure());
	}

	//moving to new position
	m_selectedFigure->moveTo(m_board[pos.Y][pos.X]);
}

void Chessboard::performSpecialMove(const ChessCoordinates& pos)
{
	m_undoHistory.push_back(new UndoMove());
	if (m_selectedFigure->getType() == Figure::Figure_t::KING) {
		if (isCastleingBig && pos.X == 2) {
			performCastleingBig(); 
			isCastleingBig = false;
		}
		if(isCastleingSmall && pos.X == 6) {
			performCastleingSmall();
			isCastleingSmall = false;
		}
		m_undoHistory.back()->castleing = true;
	}
	if (m_selectedFigure->getType() == Figure::Figure_t::PAWN) {
		if (isEnPassantLeft || isEnPassantRight) {
			performEnPassant();
			m_undoHistory.back()->enPassant = true;
		}
		if (m_isPawnAtEnd) {
			m_isPawnAtEnd = false;
			m_isPromotionInProgress = true;
			m_undoHistory.back()->isPromotionInProgress = true;

			//Kill figure if new Position contains enemy figure
			if (m_board[pos.Y][pos.X]->getFigure() != nullptr) {
				captureFigure(m_board[pos.Y][pos.X]->getFigure());
			}

			//moving to new position
			m_selectedFigure->moveTo(m_board[pos.Y][pos.X]);
		}
	}
}

void Chessboard::performCastleingSmall()
{
	auto team = m_selectedFigure->getTeam();

	ChessCoordinates kingNewPos = team == Figure::Team::WHITE ? ChessCoordinates(6, 0) : ChessCoordinates(6, 7);
	ChessCoordinates rookNewPos = team == Figure::Team::WHITE ? ChessCoordinates(5, 0) : ChessCoordinates(5, 7);
	Figure* rook = team == Figure::Team::WHITE ? m_rookWhiteSmall : m_rookBlackSmall;
	

	m_selectedFigure->moveTo(m_board[kingNewPos.Y][kingNewPos.X]);
	rook->moveTo(m_board[rookNewPos.Y][rookNewPos.X]);
	m_movedFigures.push_back(rook);
}

void Chessboard::performCastleingBig()
{
	auto team = m_selectedFigure->getTeam();

	ChessCoordinates kingNewPos = team == Figure::Team::WHITE ? ChessCoordinates(2, 0) : ChessCoordinates(2, 7);
	ChessCoordinates rookNewPos = team == Figure::Team::WHITE ? ChessCoordinates(3, 0) : ChessCoordinates(3, 7);
	Figure* rook = team == Figure::Team::WHITE ? m_rookWhiteBig : m_rookBlackBig;


	m_selectedFigure->moveTo(m_board[kingNewPos.Y][kingNewPos.X]);
	rook->moveTo(m_board[rookNewPos.Y][rookNewPos.X]);
	m_movedFigures.push_back(rook);
}

void Chessboard::performEnPassant()
{

	ChessCoordinates pos = m_selectedFigure->getPos()->getPos();
	auto team = m_selectedFigure->getTeam();

	ChessCoordinates attackMoveLeft = team == Figure::Team::WHITE ? ChessCoordinates(pos.X - 1, pos.Y + 1) : ChessCoordinates(pos.X - 1, pos.Y - 1);
	ChessCoordinates attackMoveRight = team == Figure::Team::WHITE ? ChessCoordinates(pos.X + 1, pos.Y + 1) : ChessCoordinates(pos.X + 1, pos.Y - 1);

	if (isEnPassantLeft) {
		captureFigure(m_board[pos.Y][pos.X-1]->getFigure());
		m_selectedFigure->moveTo(m_board[attackMoveLeft.Y][attackMoveLeft.X]);
	}
	if (isEnPassantRight) {
		captureFigure(m_board[pos.Y][pos.X+1]->getFigure());
		
		m_selectedFigure->moveTo(m_board[attackMoveRight.Y][attackMoveRight.X]);
	}
}

void Chessboard::cleanBoard()
{
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			m_board[y][x]->setEmpty();
		}
	}
	for (auto i = 0; i < m_undoHistory.size(); ++i) {
		delete m_undoHistory.at(i);
	}

	for (auto i = 0; i < m_Figures.size(); ++i) {
		delete m_Figures.at(i);
	}
}

void Chessboard::setupPawns()
{
	for (auto x = 0; x < 8; ++x) {
		Figure* pawnWhite = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::PAWN, m_board[1][x]);
		Figure* pawnBlack = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::PAWN, m_board[6][x]);

		m_board[1][x]->setFigure(pawnWhite);
		m_board[6][x]->setFigure(pawnBlack);

		m_Figures.push_back(pawnWhite);
		m_Figures.push_back(pawnBlack);
	}
}

void Chessboard::setupRooks()
{
	Figure* white1 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::ROOK, m_board[0][0]);
	Figure* white2 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::ROOK, m_board[0][7]);
	Figure* black1 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::ROOK, m_board[7][0]);
	Figure* black2 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::ROOK, m_board[7][7]);

	m_board[0][0]->setFigure(white1);
	m_board[0][7]->setFigure(white2);
	m_board[7][0]->setFigure(black1);
	m_board[7][7]->setFigure(black2);

	m_Figures.push_back(white1);
	m_Figures.push_back(white2);
	m_Figures.push_back(black1);
	m_Figures.push_back(black2);

	m_rookWhiteBig = white1;
	m_rookWhiteSmall = white2;
	m_rookBlackBig = black1;
	m_rookBlackSmall = black2;
}

void Chessboard::setupKnights()
{
	Figure* white1 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::KNIGHT, m_board[0][1]);
	Figure* white2 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::KNIGHT, m_board[0][6]);
	Figure* black1 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::KNIGHT, m_board[7][1]);
	Figure* black2 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::KNIGHT, m_board[7][6]);

	m_board[0][1]->setFigure(white1);
	m_board[0][6]->setFigure(white2);
	m_board[7][1]->setFigure(black1);
	m_board[7][6]->setFigure(black2);

	m_Figures.push_back(white1);
	m_Figures.push_back(white2);
	m_Figures.push_back(black1);
	m_Figures.push_back(black2);
}

void Chessboard::setupBishops()
{
	Figure* white1 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::BISHOP, m_board[0][2]);
	Figure* white2 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::BISHOP, m_board[0][5]);
	Figure* black1 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::BISHOP, m_board[7][2]);
	Figure* black2 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::BISHOP, m_board[7][5]);

	m_board[0][2]->setFigure(white1);
	m_board[0][5]->setFigure(white2);
	m_board[7][2]->setFigure(black1);
	m_board[7][5]->setFigure(black2);

	m_Figures.push_back(white1);
	m_Figures.push_back(white2);
	m_Figures.push_back(black1);
	m_Figures.push_back(black2);
}

void Chessboard::setupQueens()
{
	Figure* white1 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::QUEEN, m_board[0][3]);
	Figure* black1 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::QUEEN, m_board[7][3]);

	m_board[0][3]->setFigure(white1);
	m_board[7][3]->setFigure(black1);

	m_Figures.push_back(white1);
	m_Figures.push_back(black1);
}

void Chessboard::setupKings()
{
	Figure* white1 = new Figure(m_chessboard, Figure::Team::WHITE, Figure::Figure_t::KING, m_board[0][4]);
	Figure* black1 = new Figure(m_chessboard, Figure::Team::BLACK, Figure::Figure_t::KING, m_board[7][4]);

	m_board[0][4]->setFigure(white1);
	m_board[7][4]->setFigure(black1);

	m_Figures.push_back(white1);
	m_Figures.push_back(black1);

	m_kingWhite = white1;
	m_kingBlack = black1;
}

bool Chessboard::nextRound()
{
	//if king is still in check undo Move
	newThreatAssessment();
	if (m_isKingCheck) {
		undoLastMove();
		return false;
	}
	else if (m_isPromotionInProgress) {
		return false;
	}
	//New Round
	m_otherTeam = m_turn;
	m_turn = m_turn == Figure::Team::WHITE ? Figure::Team::BLACK : Figure::Team::WHITE;
	newThreatAssessment();
	m_round++;
	return true;
}
void Chessboard::prevRound()
{
	undoLastMove();
	m_otherTeam = m_turn;
	m_turn = m_turn == Figure::Team::WHITE ? Figure::Team::BLACK : Figure::Team::WHITE;
	newThreatAssessment();
	m_round--;
}

void Chessboard::newThreatAssessment()
{
	clearThreat();
	for (auto* figure : m_Figures) {
		if(figure->getTeam() == m_otherTeam)
			possibleMoves(figure, false);
	}
	m_isKingCheckMate = isKingCheckMate(m_turn);
	if (m_isKingCheckMate) {
		std::string kingTeam = m_turn == Figure::Team::WHITE ? "White" : "Black";
		std::cout << "King-" << kingTeam << " is checkmate" << std::endl;
		m_selectedFigure = (m_turn == Figure::Team::WHITE) ? m_kingWhite : m_kingBlack;
	}
	else if (m_isKingCheck) {
		std::string kingTeam = m_turn == Figure::Team::WHITE ? "White" : "Black";
		std::cout << "King-" << kingTeam << " is check" << std::endl;
	}
}

void Chessboard::clearThreat()
{
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			m_board[y][x]->setUnderThreat(false);
		}
	}
}

void Chessboard::setSquareUnderThreat(Square* square, const Figure::Team& selectedFigureTeam)
{
	if (selectedFigureTeam != m_turn) {
		square->setUnderThreat(true);
	}
}

void Chessboard::undoLastMove()
{
	if (m_undoHistory.empty()) { return; }

	auto lastMove = m_undoHistory.back();

	m_movedFigures.back()->undoMove();
	m_movedFigures.pop_back();

	if (lastMove->figureWasCaptured) {
		auto capturedFigure = m_capturedFigures.back();
		capturedFigure->undoMove();
		m_Figures.push_back(capturedFigure);
		m_capturedFigures.pop_back();
	}
	if (lastMove->castleing) {
		m_movedFigures.back()->undoMove(); //extra undo move
		m_movedFigures.pop_back();
	}
	if (lastMove->isPromotionInProgress) {
		m_isPawnAtEnd = false;
		m_isPromotionInProgress = false;
		if (lastMove->promotedFigure != nullptr) {
			for (auto it = m_Figures.begin(); it != m_Figures.end();) {
				if (*it == lastMove->promotedFigure) {
					it = m_Figures.erase(it);
				}
				else ++it;
			}
			delete lastMove->promotedFigure;
		}
	}
	m_undoHistory.pop_back();
	
}

bool Chessboard::isWhiteKingCheckMate() const {
	return (m_isKingCheckMate && (m_turn == Figure::Team::WHITE));
}
bool Chessboard::isBlackKingCheckMate() const {
	return (m_isKingCheckMate && (m_turn == Figure::Team::BLACK));
}

bool Chessboard::isKingCheck(Figure* king)
{
	return king->getPos()->isUnderThreat();
}

bool Chessboard::isKingCheckMate(const Figure::Team& kingTeam)
{
	size_t movesLeft = 0;
	auto king = (kingTeam == Figure::Team::WHITE) ? m_kingWhite : m_kingBlack;
	m_isKingCheck = isKingCheck(king); 
	if (m_isKingCheck) {
		movesLeft = kingMoves(king, false);
	}
	return (!movesLeft && m_isKingCheck);
}

void Chessboard::possibleMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	//Search for possible Moves
	switch (selectedFigure->getType())
	{
	case Figure::Figure_t::PAWN:
		pawnMoves(selectedFigure, highlightSquare);
		enPassant(selectedFigure);
		promotionMove(selectedFigure);
		break;
	case Figure::Figure_t::ROOK:
		rookMoves(selectedFigure, highlightSquare);
		break;
	case Figure::Figure_t::KNIGHT:
		knightMoves(selectedFigure, highlightSquare);
		break;
	case Figure::Figure_t::BISHOP:
		bishopMoves(selectedFigure, highlightSquare);
		break;
	case Figure::Figure_t::QUEEN:
		queenMoves(selectedFigure, highlightSquare);
		break;
	case Figure::Figure_t::KING:
		kingMoves(selectedFigure, highlightSquare);
		castleingSmall(selectedFigure);
		castleingBig(selectedFigure);
		break;
	default:
		break;
	}
}

void Chessboard::pawnMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	ChessCoordinates moveOne = team == Figure::Team::WHITE ? ChessCoordinates(pos.X, pos.Y + 1) : ChessCoordinates(pos.X, pos.Y - 1);
	ChessCoordinates moveTwo = team == Figure::Team::WHITE ? ChessCoordinates(pos.X, pos.Y + 2) : ChessCoordinates(pos.X, pos.Y - 2);

	ChessCoordinates attackMoveLeft = team == Figure::Team::WHITE ? ChessCoordinates(pos.X + 1, pos.Y + 1) : ChessCoordinates(pos.X + 1, pos.Y - 1);
	ChessCoordinates attackMoveRight = team == Figure::Team::WHITE ? ChessCoordinates(pos.X - 1, pos.Y + 1) : ChessCoordinates(pos.X - 1, pos.Y - 1);
	auto isHighlighted = highlightSquare ? Square::SquareHighlightType::STANDARD_MOVE : Square::SquareHighlightType::NONE;

	//Normal Movement
	if (selectedFigure->isFirstMove()) {
		if (m_board[moveOne.Y][moveOne.X]->isEmpty()) { 
			m_board[moveOne.Y][moveOne.X]->setHighlighted(isHighlighted);
		}
		if (m_board[moveOne.Y][moveOne.X]->isEmpty() && m_board[moveTwo.Y][moveTwo.X]->isEmpty()) {
			m_board[moveTwo.Y][moveTwo.X]->setHighlighted(isHighlighted);
		}
	}
	else {
		if (moveOne.isOnBoard() && m_board[moveOne.Y][moveOne.X]->isEmpty()) {
			m_board[moveOne.Y][moveOne.X]->setHighlighted(isHighlighted);
		}
	}

	//Attack Movement
	if (attackMoveLeft.isOnBoard()) {
		if (!m_board[attackMoveLeft.Y][attackMoveLeft.X]->isEmpty() && m_board[attackMoveLeft.Y][attackMoveLeft.X]->getFigure()->getTeam() != team) {
			if (m_board[attackMoveLeft.Y][attackMoveLeft.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[attackMoveLeft.Y][attackMoveLeft.X]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[attackMoveLeft.Y][attackMoveLeft.X], team);
	}
	if (attackMoveRight.isOnBoard()) {
		if(!m_board[attackMoveRight.Y][attackMoveRight.X]->isEmpty() && m_board[attackMoveRight.Y][attackMoveRight.X]->getFigure()->getTeam() != team) {
			if (m_board[attackMoveRight.Y][attackMoveRight.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[attackMoveRight.Y][attackMoveRight.X]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[attackMoveRight.Y][attackMoveRight.X], team);
	}
}

void Chessboard::rookMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	auto isHighlighted = highlightSquare ? Square::SquareHighlightType::STANDARD_MOVE : Square::SquareHighlightType::NONE;


	//Vertical Up
	for (auto y = pos.Y; y < 8; ++y) {
		if (m_board[y][pos.X]->getFigure() == selectedFigure) {
			continue;
		}
		if (m_board[y][pos.X]->isEmpty()) {
			m_board[y][pos.X]->setHighlighted(isHighlighted);
			setSquareUnderThreat(m_board[y][pos.X], team);
			continue;
		}
		if(m_board[y][pos.X]->getFigure()->getTeam() != team) {
			if (m_board[y][pos.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[y][pos.X]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[y][pos.X], team);
		
		break;
	}
	//Vertical Down
	for (auto y = pos.Y; y >= 0; --y) {
		if (m_board[y][pos.X]->getFigure() == selectedFigure) {
			continue;
		}
		if (m_board[y][pos.X]->isEmpty()) {
			m_board[y][pos.X]->setHighlighted(isHighlighted);
			setSquareUnderThreat(m_board[y][pos.X], team);
			continue;
		}
		if (m_board[y][pos.X]->getFigure()->getTeam() != team) {
			if (m_board[y][pos.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[y][pos.X]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[y][pos.X], team);
		
		break;
		
	}
	//Horizontal Left
	for (auto x = pos.X; x >= 0; --x) {
		if (m_board[pos.Y][x]->getFigure() == selectedFigure) {
			continue;
		}
		if (m_board[pos.Y][x]->isEmpty()) {
			m_board[pos.Y][x]->setHighlighted(isHighlighted);
			setSquareUnderThreat(m_board[pos.Y][x], team);
			continue;
		}
		if (m_board[pos.Y][x]->getFigure()->getTeam() != team) {

			if (m_board[pos.Y][x]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[pos.Y][x]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[pos.Y][x], team);
		
		break;
	}

	//Horizontal Right
	for (auto x = pos.X; x < 8; ++x) {
		if (m_board[pos.Y][x]->getFigure() == selectedFigure) {
			continue;
		}
		if (m_board[pos.Y][x]->isEmpty()) {
			m_board[pos.Y][x]->setHighlighted(isHighlighted);
			setSquareUnderThreat(m_board[pos.Y][x], team);
			continue;
		}
		if (m_board[pos.Y][x]->getFigure()->getTeam() != team) {
			if (m_board[pos.Y][x]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[pos.Y][x]->setHighlighted(isHighlighted);
			}
		}
		setSquareUnderThreat(m_board[pos.Y][x], team);
		
		break;
	}
}

void Chessboard::knightMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	auto isHighlighted = highlightSquare ? Square::SquareHighlightType::STANDARD_MOVE : Square::SquareHighlightType::NONE;

	std::vector<ChessCoordinates> moves = {
		ChessCoordinates(pos.X - 1, pos.Y + 2),
		ChessCoordinates(pos.X + 1, pos.Y + 2),

		ChessCoordinates(pos.X - 2, pos.Y + 1),
		ChessCoordinates(pos.X + 2, pos.Y + 1),

		ChessCoordinates(pos.X - 2, pos.Y - 1),
		ChessCoordinates(pos.X + 2, pos.Y - 1),

		ChessCoordinates(pos.X - 1, pos.Y - 2),
		ChessCoordinates(pos.X + 1, pos.Y - 2),
	};
	for (const auto& move : moves) {
		if (move.isOnBoard()) {
			if (m_board[move.Y][move.X]->isEmpty()) {
				m_board[move.Y][move.X]->setHighlighted(isHighlighted);
				setSquareUnderThreat(m_board[move.Y][move.X], team);
			}
			else{
				if (m_board[move.Y][move.X]->getFigure()->getTeam() != team) {
					if (m_board[move.Y][move.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[move.Y][move.X]->setHighlighted(isHighlighted);
					}
				}
				setSquareUnderThreat(m_board[move.Y][move.X], team);
			}
		}
	}
}

void Chessboard::bishopMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	auto isHighlighted = highlightSquare ? Square::SquareHighlightType::STANDARD_MOVE : Square::SquareHighlightType::NONE;

	//As seen from white Player
	auto diagonalLeftUpBlocked = false;
	auto diagonalRightUpBlocked = false;
	auto diagonalLeftDownBlocked = false;
	auto diagonalRightDownBlocked = false;

	//Diagonals
	for (size_t i = 1; i < 8; ++i) {
		auto offset = ChessCoordinates(i, i);

		auto posDiagLeftUp = ChessCoordinates(pos.X - offset.X, pos.Y + offset.Y);
		if (!diagonalLeftUpBlocked && posDiagLeftUp.isOnBoard()) {
			if (m_board[posDiagLeftUp.Y][posDiagLeftUp.X]->isEmpty()) {
				m_board[posDiagLeftUp.Y][posDiagLeftUp.X]->setHighlighted(isHighlighted);
			}
			else {
				if (m_board[posDiagLeftUp.Y][posDiagLeftUp.X]->getFigure()->getTeam() != team) {
					if (m_board[posDiagLeftUp.Y][posDiagLeftUp.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[posDiagLeftUp.Y][posDiagLeftUp.X]->setHighlighted(isHighlighted);
					}
				}
				diagonalLeftUpBlocked = true;
			}
			setSquareUnderThreat(m_board[posDiagLeftUp.Y][posDiagLeftUp.X], team);

		}

		auto posDiagRightUp = ChessCoordinates(pos.X + offset.X, pos.Y + offset.Y); 
		if (!diagonalRightUpBlocked && posDiagRightUp.isOnBoard()) {
			if (m_board[posDiagRightUp.Y][posDiagRightUp.X]->isEmpty()) {
				m_board[posDiagRightUp.Y][posDiagRightUp.X]->setHighlighted(isHighlighted);
			}
			else {
				if (m_board[posDiagRightUp.Y][posDiagRightUp.X]->getFigure()->getTeam() != team) {
					if (m_board[posDiagRightUp.Y][posDiagRightUp.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[posDiagRightUp.Y][posDiagRightUp.X]->setHighlighted(isHighlighted);
					}
				}
				diagonalRightUpBlocked = true;
			}
			setSquareUnderThreat(m_board[posDiagRightUp.Y][posDiagRightUp.X], team);
		}
		

		auto posDiagLeftDown = ChessCoordinates(pos.X - offset.X, pos.Y - offset.Y); 
		if (!diagonalLeftDownBlocked && posDiagLeftDown.isOnBoard()) {
			if (m_board[posDiagLeftDown.Y][posDiagLeftDown.X]->isEmpty()) {
				m_board[posDiagLeftDown.Y][posDiagLeftDown.X]->setHighlighted(isHighlighted);
			}
			else {
				if (m_board[posDiagLeftDown.Y][posDiagLeftDown.X]->getFigure()->getTeam() != team) {
					if (m_board[posDiagLeftDown.Y][posDiagLeftDown.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[posDiagLeftDown.Y][posDiagLeftDown.X]->setHighlighted(isHighlighted);
					}
				}
				diagonalLeftDownBlocked = true;
			}
			setSquareUnderThreat(m_board[posDiagLeftDown.Y][posDiagLeftDown.X], team);
		}

		auto posDiagRightDown = ChessCoordinates(pos.X + offset.X, pos.Y - offset.Y);
		if (!diagonalRightDownBlocked && posDiagRightDown.isOnBoard()) {
			if (m_board[posDiagRightDown.Y][posDiagRightDown.X]->isEmpty()) {
				m_board[posDiagRightDown.Y][posDiagRightDown.X]->setHighlighted(isHighlighted);
			}
			else {
				if ( m_board[posDiagRightDown.Y][posDiagRightDown.X]->getFigure()->getTeam() != team) {
					if (m_board[posDiagRightDown.Y][posDiagRightDown.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[posDiagRightDown.Y][posDiagRightDown.X]->setHighlighted(isHighlighted);
					}
				}
				diagonalRightDownBlocked = true;
			}
			setSquareUnderThreat(m_board[posDiagRightDown.Y][posDiagRightDown.X], team);
		}
	}
}

void Chessboard::queenMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	rookMoves(selectedFigure, highlightSquare);
	bishopMoves(selectedFigure, highlightSquare);
}

size_t Chessboard::kingMoves(Figure* selectedFigure, const bool& highlightSquare)
{
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	auto isHighlighted = highlightSquare ? Square::SquareHighlightType::STANDARD_MOVE : Square::SquareHighlightType::NONE;

	size_t movesLeft = 0;
	

	std::vector<ChessCoordinates> moves = {
		ChessCoordinates(pos.X, pos.Y + 1),
		ChessCoordinates(pos.X + 1, pos.Y + 1),

		ChessCoordinates(pos.X + 1, pos.Y),
		ChessCoordinates(pos.X + 1, pos.Y - 1),

		ChessCoordinates(pos.X, pos.Y - 1),
		ChessCoordinates(pos.X - 1, pos.Y - 1),

		ChessCoordinates(pos.X - 1, pos.Y),
		ChessCoordinates(pos.X - 1, pos.Y +1),
	};
	for (const auto& move : moves) {
		if (move.isOnBoard() && !m_board[move.Y][move.X]->isUnderThreat()) {
			if (m_board[move.Y][move.X]->isEmpty()) {
				m_board[move.Y][move.X]->setHighlighted(isHighlighted);
				setSquareUnderThreat(m_board[move.Y][move.X], team);
				movesLeft++;
			}
			else {
				if (m_board[move.Y][move.X]->getFigure()->getTeam() != team) {
					if (m_board[move.Y][move.X]->getFigure()->getType() != Figure::Figure_t::KING) {
						m_board[move.Y][move.X]->setHighlighted(isHighlighted);
						movesLeft++;
					}
				}
				setSquareUnderThreat(m_board[move.Y][move.X], team);
			}
		}
	}
	return movesLeft;
}

void Chessboard::castleingSmall(Figure* selectedFigure)
{
	auto team = selectedFigure->getTeam();
	Figure* rook = nullptr;
	ChessCoordinates pos;
	std::vector<Square*> freeSquares;
	std::vector<Square*> noThreatSquares;
	switch (team)
	{
	case Figure::Team::BLACK: 
		rook = m_rookBlackSmall; 
		pos = ChessCoordinates(6,7); 
		freeSquares = {
			m_board[7][5],
			m_board[7][6],
		};
		noThreatSquares = {
			m_board[7][4],
			m_board[7][5],
			m_board[7][6],
			m_board[7][7],
		};
		break; 
	case Figure::Team::WHITE: 
		rook = m_rookWhiteSmall; 
		pos = ChessCoordinates(6,0); 
		freeSquares = {
			m_board[0][5],
			m_board[0][6],
		};
		noThreatSquares = {
			m_board[0][4],
			m_board[0][5],
			m_board[0][6],
			m_board[0][7],
		};
		break;
	default:
		break;
	}
	
	if(rook != nullptr){
		if (selectedFigure->isFirstMove() && (rook->isFirstMove())) {
			bool allEmpty = true;
			bool allUnthreatened = true;
			for (auto* square : freeSquares) {
				if (square->isEmpty())
					continue;
				allEmpty = false;
			}
			for (auto* square : noThreatSquares) {
				if (!square->isUnderThreat()) {
					continue;
				}
				allUnthreatened = false;
			}
			if (allEmpty && allUnthreatened) {
				m_board[pos.Y][pos.X]->setHighlighted(Square::SquareHighlightType::SPECIAL_MOVE);
				isCastleingSmall = true;
			}
		}
	}
}

void Chessboard::castleingBig(Figure* selectedFigure)
{
	auto team = selectedFigure->getTeam();
	Figure* rook = nullptr;
	ChessCoordinates pos;
	std::vector<Square*> freeSquares;
	std::vector<Square*> noThreatSquares;
	switch (team)
	{
	case Figure::Team::BLACK:
		rook = m_rookBlackBig;
		pos = ChessCoordinates(2, 7);
		freeSquares = {
			m_board[7][1],
			m_board[7][2],
			m_board[7][3],
		};
		noThreatSquares = {
			m_board[7][0],
			m_board[7][1],
			m_board[7][2],
			m_board[7][3],
			m_board[7][4],
		};
		break;
	case Figure::Team::WHITE:
		rook = m_rookWhiteBig;
		pos = ChessCoordinates(2, 0);
		freeSquares = {
			m_board[0][1],
			m_board[0][2],
			m_board[0][3],
		};
		noThreatSquares = {
			m_board[0][0],
			m_board[0][1],
			m_board[0][2],
			m_board[0][3],
			m_board[0][4],
		};
		break;
	default:
		break;
	}
	if (rook != nullptr) {
		if (selectedFigure->isFirstMove() && (rook->isFirstMove())) {
			bool allEmpty = true;
			bool allUnthreatened = true;
			for (auto* square : freeSquares) {
				if (square->isEmpty())
					continue;
				allEmpty = false;
			}
			for (auto* square : noThreatSquares) {
				if (!square->isUnderThreat()) {
					continue;
				}
				allUnthreatened = false;
			}
			if (allEmpty && allUnthreatened) {
				m_board[pos.Y][pos.X]->setHighlighted(Square::SquareHighlightType::SPECIAL_MOVE);
				isCastleingBig = true;
			}
		}
	}
}

void Chessboard::enPassant(Figure* selectedFigure)
{
	isEnPassantLeft = false;
	isEnPassantRight = false;
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();


	ChessCoordinates attackMoveLeft = team == Figure::Team::WHITE ? ChessCoordinates(pos.X - 1, pos.Y + 1) : ChessCoordinates(pos.X - 1, pos.Y - 1);
	ChessCoordinates attackMoveRight = team == Figure::Team::WHITE ? ChessCoordinates(pos.X + 1, pos.Y + 1) : ChessCoordinates(pos.X + 1, pos.Y - 1);
	size_t enPassantRow = team == Figure::Team::WHITE ? 4 : 3;
	auto isHighlighted =  Square::SquareHighlightType::SPECIAL_MOVE;

	if (!m_movedFigures.empty()) {
		auto lastMovedFigure = m_movedFigures.back();
		ChessCoordinates lastMovedFigurePos = lastMovedFigure->getPos()->getPos();

		if ((lastMovedFigure->getType() == Figure::Figure_t::PAWN) && (lastMovedFigure->getMoves() == 1)) {
			if ((pos.Y == enPassantRow) && (lastMovedFigurePos.Y == enPassantRow) && ((lastMovedFigurePos.X - pos.X) == -1)) {
				m_board[attackMoveLeft.Y][attackMoveLeft.X]->setHighlighted(isHighlighted);
				isEnPassantLeft = true;
			}
			if ((pos.Y == enPassantRow) && (lastMovedFigurePos.Y == enPassantRow) && ((lastMovedFigurePos.X - pos.X) == 1)) {
				m_board[attackMoveRight.Y][attackMoveRight.X]->setHighlighted(isHighlighted);
				isEnPassantRight = true;
			}
		}
	}

}
void Chessboard::promotionMove(Figure* selectedFigure) {
	ChessCoordinates pos = selectedFigure->getPos()->getPos();
	auto team = selectedFigure->getTeam();
	ChessCoordinates moveOne = team == Figure::Team::WHITE ? ChessCoordinates(pos.X, pos.Y + 1) : ChessCoordinates(pos.X, pos.Y - 1);

	ChessCoordinates attackMoveLeft = team == Figure::Team::WHITE ? ChessCoordinates(pos.X + 1, pos.Y + 1) : ChessCoordinates(pos.X + 1, pos.Y - 1);
	ChessCoordinates attackMoveRight = team == Figure::Team::WHITE ? ChessCoordinates(pos.X - 1, pos.Y + 1) : ChessCoordinates(pos.X - 1, pos.Y - 1);
	auto isHighlighted = Square::SquareHighlightType::SPECIAL_MOVE;

	bool isOnLastField = team == Figure::Team::WHITE ? ((pos.Y+1) == 7 ? true : false) : ((pos.Y-1) == 0 ? true : false);

	m_isPawnAtEnd = false;
	//Normal Movement
	if (isOnLastField && moveOne.isOnBoard() && m_board[moveOne.Y][moveOne.X]->isEmpty()) {
		m_board[moveOne.Y][moveOne.X]->setHighlighted(isHighlighted);
		m_isPawnAtEnd = true;
	}
	//Attack Movement
	if (isOnLastField && attackMoveLeft.isOnBoard() && !m_board[attackMoveLeft.Y][attackMoveLeft.X]->isEmpty()) {
		if (m_board[attackMoveLeft.Y][attackMoveLeft.X]->getFigure()->getTeam() != team) {
			if (m_board[attackMoveLeft.Y][attackMoveLeft.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[attackMoveLeft.Y][attackMoveLeft.X]->setHighlighted(isHighlighted);
				m_isPawnAtEnd = true;
			}
		}
	}
	if (isOnLastField && attackMoveRight.isOnBoard() && !m_board[attackMoveRight.Y][attackMoveRight.X]->isEmpty()) {
		if (m_board[attackMoveRight.Y][attackMoveRight.X]->getFigure()->getTeam() != team) {
			if (m_board[attackMoveRight.Y][attackMoveRight.X]->getFigure()->getType() != Figure::Figure_t::KING) {
				m_board[attackMoveRight.Y][attackMoveRight.X]->setHighlighted(isHighlighted);
				m_isPawnAtEnd = true;
			}
		}
	}
}

void Chessboard::createNewGame() {
	cleanBoard();

	
	setupPawns();
	setupRooks();
	setupKnights();
	setupBishops();
	setupQueens();
	setupKings();

	newThreatAssessment();

	//Rendering Everything in VVE now
	for (auto i = 0; i < m_Figures.size(); ++i) {
		m_Figures.at(i)->createSceneNode();
	}

	
}

Chessboard::Chessboard(VESceneNode* chessboard):m_chessboard{chessboard}
{
	//CREATING TILES (SQUARES) of CHESSBOARD
	std::vector<Square*> row(8,nullptr);
	bool color = true;
	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			color = !color;
			if (color) {
				row[x] = new Square(m_chessboard, ChessCoordinates(x, y), Figure::Team::BLACK);
				continue;
			}
			row[x] = new Square(m_chessboard, ChessCoordinates(x, y), Figure::Team::WHITE);
		}
		color = !color;
		m_board.push_back(row);
	}
	m_CapturedSquare = new Square(m_chessboard, ChessCoordinates(20, 20), Figure::Team::BLACK);
}

Chessboard::~Chessboard()
{
	delete m_CapturedSquare;

	for (auto y = 0; y < 8; ++y) {
		for (auto x = 0; x < 8; ++x) {
			delete m_board[y][x];
		}
	}
	for (auto i = 0; i < m_undoHistory.size(); ++i) {
		delete m_undoHistory.at(i);
	}

	for (auto i = 0; i < m_Figures.size(); ++i) {
		delete m_Figures.at(i);
	}
}
