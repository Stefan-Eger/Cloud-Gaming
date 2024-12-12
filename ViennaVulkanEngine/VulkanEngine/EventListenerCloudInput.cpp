#include "ChessInclude.h"

Ray EventListenerCloudInput::createRayThroughPixel(const float& cursorX, const float& cursorY) const
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

void EventListenerCloudInput::rotateAroundBoard()
{
	VESceneNode* cameraBoardPoint = getSceneManagerPointer()->getSceneNode("CameraBoardPoint");
	glm::mat4 aroundBoard = cameraBoardPoint->getRotation() * glm::rotate(GameInfo::CAMERA_AROUND_BOARD_ROTATION, glm::vec3(0.0f, 1.0f, 0.0f));
	aroundBoard[3] = glm::vec4(cameraBoardPoint->getPosition(), 1.0f);
	cameraBoardPoint->setTransform(aroundBoard);
}

void EventListenerCloudInput::onMouseButton(const InputType& type, const float& cursorX, const float& cursorY)
{
	if (type == InputType::LEFT_CLICK) {
		std::cout << "Left Click Pressed: " << std::endl;
		std::cout << "MousePos: [" << "X: " << cursorX << "Y: " << cursorY << "]" << std::endl;


		Ray ray = createRayThroughPixel(cursorX, cursorY);

		GameInfo::g_chessboard->selectFigure(ray);

		return;
	}
	
	if (type == InputType::RIGHT_CLICK) {
		std::cout << "Right Click Pressed: " << std::endl;
		std::cout << "MousePos: [" << "X: " << cursorX << "Y: " << cursorY << "]" << std::endl;


		Ray ray = createRayThroughPixel(cursorX, cursorY);  

		bool figureWasMoved = GameInfo::g_chessboard->moveFigure(ray);
		if (figureWasMoved) {
			rotateAroundBoard();
			isGameOver();
		}

		return;
	}
	
}

void EventListenerCloudInput::onKeyboard(const InputType& type)
{
	bool roundWasCompleted = false;
	switch (type)
	{
	case InputType::PROMOTION_ROOK:
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::ROOK);
		break;
	case InputType::PROMOTION_KNIGHT:
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::KNIGHT);
		break;
	case InputType::PROMOTION_BISHOP:
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::BISHOP);
		break;
	case InputType::PROMOTION_QUEEN:
		roundWasCompleted = GameInfo::g_chessboard->promotion(Figure::Figure_t::QUEEN);
		break;

	default:
		break;
	}

	if (roundWasCompleted) {
		rotateAroundBoard();
		isGameOver();
	}
}

std::vector<std::string> EventListenerCloudInput::splitInput(const std::string& input, const char& delimeter) const
{
	std::vector<std::string> ret;
	std::string word;

	std::istringstream iss(input);
	while (std::getline(iss, word, delimeter)) {
		ret.push_back(word);
	}
	return ret;
}

void EventListenerCloudInput::isGameOver()
{
	bool isBlackCheckmate = GameInfo::g_chessboard->isBlackKingCheckMate();
	bool isWhiteCheckmate = GameInfo::g_chessboard->isWhiteKingCheckMate();

	GameInfo::g_gameOver = isBlackCheckmate || isWhiteCheckmate;
}

void EventListenerCloudInput::onFrameEnded(veEvent event)
{
	char* buf = nullptr;
	int buf_size = m_receiver->receive(&buf, sizeof(buf));

	if (buf != nullptr) {
		std::string inputRaw(buf);
		auto input = splitInput(inputRaw, ',');

		if (input.at(0) == "Left Click") {
			onMouseButton(InputType::LEFT_CLICK, std::stof(input.at(1)), std::stof(input.at(2)));
		}
		else if (input.at(0) == "Right Click") {
			onMouseButton(InputType::RIGHT_CLICK, std::stof(input.at(1)), std::stof(input.at(2)));
		}
		if (input.at(0) == "Keyboard" && input.at(1) == "R") {
			onKeyboard(InputType::PROMOTION_ROOK);
		}
		else if (input.at(0) == "Keyboard" && input.at(1) == "K") {
			onKeyboard(InputType::PROMOTION_KNIGHT);
		}
		else if (input.at(0) == "Keyboard" && input.at(1) == "B") {
			onKeyboard(InputType::PROMOTION_BISHOP);
		}
		else if (input.at(0) == "Keyboard" && input.at(1) == "Q") {
			onKeyboard(InputType::PROMOTION_QUEEN);
		}
		free(buf);
	}
}

EventListenerCloudInput::EventListenerCloudInput(std::string name) : VEEventListener(name)
{
	//INITALIZE UDPSOCKET
	m_receiver = new UDPReceive6();
	m_receiver->init(9091);

}

EventListenerCloudInput::~EventListenerCloudInput()
{
	delete m_receiver;
}
