#include "ChessInclude.h"

Ray EventListenerUser::createRayThroughPixel(const float& cursorX, const float& cursorY) const
{

	VkExtent2D window = getEnginePointer()->getWindow()->getExtent();
	VECameraProjective* pCamera = dynamic_cast<VECameraProjective*> (getSceneManagerPointer()->getCamera());

	//Helper Variables
	float aspectRatio = pCamera->m_aspectRatio;
	float invWidth = 1.0f / window.width;
	float invHeight = 1.0f / window.height;

	float fovVertical = std::tan(glm::radians(pCamera->m_fov * 0.5f));

	//Screen Space 
	float xx = (((cursorX + 0.5f) * invWidth) * 2 - 1) * aspectRatio * fovVertical;
	float yy = (1 - 2 * ((cursorY + 0.5f) * invHeight)) * fovVertical;


	//Camera CS to calculate the direction of the ray (if the camera Rotates then the Ray has to adjust accordingly) 
	glm::vec3 u = pCamera->getWorldTransform() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 v = pCamera->getWorldTransform() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec3 camDir = pCamera->getWorldTransform() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

	//creating Ray
	glm::vec3 cameraPos = pCamera->getWorldTransform()[3];
	glm::vec3 direction = u * xx + v * yy + camDir;

	Ray r(cameraPos, direction);
	return r;
}

void EventListenerUser::rotateAroundBoard()
{
	VESceneNode* cameraBoardPoint = getSceneManagerPointer()->getSceneNode("CameraBoardPoint");
	glm::mat4 aroundBoard = cameraBoardPoint->getRotation() * glm::rotate(GameInfo::CAMERA_AROUND_BOARD_ROTATION, glm::vec3(0.0f, 1.0f, 0.0f));
	aroundBoard[3] = glm::vec4(cameraBoardPoint->getPosition(), 1.0f); 
	cameraBoardPoint->setTransform(aroundBoard); 
}
void EventListenerUser::isGameOver()
{
	bool isBlackCheckmate = GameInfo::g_chessboard->isBlackKingCheckMate(); 
	bool isWhiteCheckmate = GameInfo::g_chessboard->isWhiteKingCheckMate();

	GameInfo::g_gameOver = isBlackCheckmate || isWhiteCheckmate;
}


//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays (14.06.23)
bool EventListenerUser::onMouseButton(veEvent event)
{
	if (event.idata3 == GLFW_PRESS) {
		m_usePrevCursorPosition = false;
		if (event.idata1 == GLFW_MOUSE_BUTTON_LEFT) {
			std::cout << "Left Click Pressed: " << std::endl;
			std::cout << "MousePos: [" << "X: " << event.fdata1 << "Y: " << event.fdata2 << "]" << std::endl;


			float cursorX = event.fdata1;
			float cursorY = event.fdata2;

			Ray ray = createRayThroughPixel(cursorX, cursorY);

			GameInfo::g_chessboard->selectFigure(ray);

			return true;
		}
	}
	if (event.idata3 == GLFW_PRESS) {
		m_usePrevCursorPosition = false;
		if (event.idata1 == GLFW_MOUSE_BUTTON_RIGHT) {
			std::cout << "Right Click Pressed: " << std::endl;
			std::cout << "MousePos: [" << "X: " << event.fdata1 << "Y: " << event.fdata2 << "]" << std::endl;

			float cursorX = event.fdata1;
			float cursorY = event.fdata2;

			Ray ray = createRayThroughPixel(cursorX, cursorY);

			bool figureWasMoved = GameInfo::g_chessboard->moveFigure(ray);
			if (figureWasMoved) {
				rotateAroundBoard();
				isGameOver();
			}
			return true;
		}
	}

	if (event.idata3 == GLFW_RELEASE) {
		m_usePrevCursorPosition = false;
		if (event.idata1 == GLFW_MOUSE_BUTTON_LEFT) {
			std::cout << "Left Click Released: " << std::endl;
			return true;
		}
	}
	if (event.idata3 == GLFW_RELEASE) {
		m_usePrevCursorPosition = false;
		if (event.idata1 == GLFW_MOUSE_BUTTON_RIGHT) {
			std::cout << "Right Click Released" << std::endl;
			m_rightButtonClicked = false;
			return true;
		}
	}
	return false;
}

bool EventListenerUser::onKeyboard(veEvent event)
{
	if (event.idata1 == GLFW_KEY_ESCAPE)
	{ //ESC pressed - end the engine
		getEnginePointer()->end();
		return true;
	}

	bool roundWasCompleted = false;
	if (event.idata1 == GLFW_KEY_R && event.idata3 == GLFW_PRESS)
	{
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::ROOK);
	}
	else if (event.idata1 == GLFW_KEY_K && event.idata3 == GLFW_PRESS)
	{
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::KNIGHT);
	}
	else if (event.idata1 == GLFW_KEY_B && event.idata3 == GLFW_PRESS)
	{
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::BISHOP);		
	}
	else if (event.idata1 == GLFW_KEY_Q && event.idata3 == GLFW_PRESS)
	{
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::QUEEN);
	}

	if (roundWasCompleted) {
		rotateAroundBoard();
		isGameOver();
	}

	return roundWasCompleted;
}


EventListenerUser::EventListenerUser(std::string name) :VEEventListenerGLFW(name) {}

EventListenerUser::~EventListenerUser() {}