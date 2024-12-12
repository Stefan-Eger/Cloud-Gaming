#ifndef GAME_INFO_H
#define GAME_INFO_H

struct GameInfo
{
	static bool g_gameOver;
	static Chessboard* g_chessboard;


	//Player Start
	//static constexpr glm::vec3 CAMERA_POS_ORIGIN{ -4.5f, 8.0f, 4.5f };
	static constexpr glm::vec3 CAMERA_POS_ORIGIN{ 0.0f, 8.0f, -9.0f };
	static constexpr glm::vec3 CAMERA_BOARD_MIDDLE_POS{ 3.5f, 0.0f, 3.5f };
	static constexpr float CAMERA_X_AXIS_THETA = 45.0f * (float)M_PI / 180.0f;
	static constexpr float CAMERA_Y_AXIS_THETA = 90.0f * (float)M_PI / 180.0f;
	static constexpr float CAMERA_AROUND_BOARD_ROTATION = 180.0f * (float)M_PI / 180.0f;
	static constexpr float CAMERA_ROTATION_SPEED = 5.0f;


	//Light Start

	static constexpr float LIGHT_X_AXIS_THETA = 45.0f * (float)M_PI / 180.0f;
	static constexpr float LIGHT_Y_AXIS_THETA = 225.0f * (float)M_PI / 180.0f;
};

#endif