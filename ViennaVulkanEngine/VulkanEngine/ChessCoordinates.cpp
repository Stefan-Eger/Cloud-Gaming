#include "ChessInclude.h"

ChessCoordinates::ChessCoordinates() : X{ 1 }, Y{ 1 }
{
}

ChessCoordinates::ChessCoordinates(const_reference x, const_reference y)
{
	this->X = x;
	this->Y = y;
}

ChessCoordinates::ChessCoordinates(const ChessCoordinates& vc)
{
	X = vc.X;
	Y = vc.Y;
}

ChessCoordinates& ChessCoordinates::operator=(const ChessCoordinates& vc)
{
	X = vc.X;
	Y = vc.Y;

	return *this;
}

bool ChessCoordinates::operator==(const ChessCoordinates& rhs) const
{
	return ((X == rhs.X) && (Y == rhs.Y));
}

ChessCoordinates ChessCoordinates::operator+(const ChessCoordinates& rhs) const
{
	return ChessCoordinates(X + rhs.X, Y + rhs.Y);
}

ChessCoordinates ChessCoordinates::operator-(const ChessCoordinates& rhs) const
{
	return ChessCoordinates(X - rhs.X, Y - rhs.Y);
}

bool ChessCoordinates::isOnBoard() const
{
	return ((X >= MIN_COORDINATE && X <= MAX_COORDINATE) && (Y >= MIN_COORDINATE && Y <= MAX_COORDINATE));
}

glm::vec3 ChessCoordinates::toVec3() const
{
	return glm::vec3(X, 0.0f, Y);
}

