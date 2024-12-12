#ifndef CHESS_COORDINATES_H
#define CHESS_COORDINATES_H
struct ChessCoordinates
{

	using value_type = int;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	static const value_type MIN_COORDINATE = 0;
	static const value_type MAX_COORDINATE = 7;


	value_type X, Y;
	ChessCoordinates();
	ChessCoordinates(const_reference x, const_reference y);
	ChessCoordinates(const ChessCoordinates& vc);
	ChessCoordinates& operator=(const ChessCoordinates& vc);

	bool operator==(const ChessCoordinates& rhs) const;

	ChessCoordinates operator+(const ChessCoordinates& rhs) const;
	ChessCoordinates operator-(const ChessCoordinates& rhs) const;


	bool isOnBoard() const;

	glm::vec3 toVec3() const;
};


namespace std {
	//https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key (26.10.21)
	template<> struct hash<ChessCoordinates> {
		size_t operator()(const ChessCoordinates& vc) const {
			size_t res = 17;

			res = res * 31 + hash<ChessCoordinates::value_type> ()(vc.X);
			res = res * 31 + hash<ChessCoordinates::value_type> ()(vc.Y);
			return res;
		}
	};
}

#endif // !VOXEL_COORDINATES_Hs

