#include "ChessInclude.h"

void ChessEngine::registerEventListeners()
{
	//VEEngine::registerEventListeners();
	registerEventListener(new EventListenerUser("UserInput"));
	registerEventListener(new EventListenerCloudInput("CloudInput"), { veEvent::VE_EVENT_FRAME_ENDED });
	registerEventListener(new EventListenerGUI("GUI"), { veEvent::VE_EVENT_DRAW_OVERLAY });
	registerEventListener(new EventListenerRecord("Record"), { veEvent::VE_EVENT_FRAME_ENDED }); 
}

void ChessEngine::loadLevel(uint32_t numLevel)
{

	registerEventListeners();
	

	//CAMERA
	
	//cameraParent is used for translations
	VESceneNode* cameraBoardPoint = getSceneManagerPointer()->createSceneNode("CameraBoardPoint", getRoot(),
		glm::translate(glm::mat4(1.0f), GameInfo::CAMERA_BOARD_MIDDLE_POS));
	//cameraParent is used for translations
	VESceneNode* cameraParent = getSceneManagerPointer()->createSceneNode("StandardCameraParent", cameraBoardPoint,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	//camera is used for rotation
	VkExtent2D extent = getWindowPointer()->getExtent();
	VECameraProjective* camera = (VECameraProjective*)getSceneManagerPointer()->createCamera("StandardCamera", VECamera::VE_CAMERA_TYPE_PROJECTIVE, cameraParent);
	camera->m_nearPlane = 0.1f;
	camera->m_farPlane = 500.1f;
	camera->m_aspectRatio = extent.width / (float)extent.height;
	camera->m_fov = 45.0f;
	//camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	getSceneManagerPointer()->setCamera(camera);

	//init camera
	cameraParent->setPosition(GameInfo::CAMERA_POS_ORIGIN);
	
	camera->lookAt(camera->getWorldTransform()[3], GameInfo::CAMERA_BOARD_MIDDLE_POS, glm::vec3(0.0f, 1.0f, 0.0f));
	camera->setPosition(glm::vec3(0, 0, 0));
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//One Rotation around board
	/*
	glm::mat4 aroundBoard = glm::rotate(GameInfo::CAMERA_AROUND_BOARD_ROTATION, glm::vec3(0.0f, 1.0f, 0.0f));
	aroundBoard[3] = glm::vec4(cameraBoardPoint->getPosition(), 1.0f);
	cameraBoardPoint->setTransform(aroundBoard);
	*/
	glm::vec3 pos = camera->getWorldTransform()[3];
	std::cout << "CAM-POS: [" << pos.x << ", " << pos.y << ", " << pos.z << "]";
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	VELight* light4 = (VESpotLight*)getSceneManagerPointer()->createLight("StandardAmbientLight", VELight::VE_LIGHT_TYPE_AMBIENT, camera);
	light4->m_col_ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

	//use one light source
	VELight* light1 = (VEDirectionalLight*)getSceneManagerPointer()->createLight("StandardDirLight", VELight::VE_LIGHT_TYPE_DIRECTIONAL, getRoot());     //new VEDirectionalLight("StandardDirLight");
	//light1->lookAt(glm::vec3(0.0f, 20.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto rotMat = glm::rotate(GameInfo::LIGHT_Y_AXIS_THETA, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(GameInfo::LIGHT_X_AXIS_THETA, glm::vec3(1.0f, 0.0f, 0.0f));
	
	light1->multiplyTransform(rotMat);
	light1->m_col_diffuse = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	light1->m_col_specular = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	

	VESceneNode* pScene;
	VECHECKPOINTER(pScene = getSceneManagerPointer()->createSceneNode("Level 1", getRoot()));

	

	/*
	//Skybox
	VESceneNode* sp1;
	VECHECKPOINTER(sp1 = getSceneManagerPointer()->createSkybox("The Sky", "media/models/test/sky/cloudy",
		{ "bluecloud_ft.jpg", "bluecloud_bk.jpg", "bluecloud_up.jpg",
			"bluecloud_dn.jpg", "bluecloud_rt.jpg", "bluecloud_lf.jpg" }, pScene));
	/*
	//Plane
	VESceneNode* e4;
	VEEntity* pE4;
	VECHECKPOINTER(e4 = getSceneManagerPointer()->loadModel("The Plane", "media/models/test/plane", "plane_t_n_s.obj", 0, pScene));
	e4->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 0.0f, 1000.0f)));
	VECHECKPOINTER(pE4 = (VEEntity*)getSceneManagerPointer()->getSceneNode("The Plane/plane_t_n_s.obj/plane/Entity_0"));
	pE4->setParam(glm::vec4(1000.0f, 1000.0f, 0.0f, 0.0f));
	*/

	
	//Chessboard
	VESceneNode* pChessboard;
	VECHECKPOINTER(pChessboard = getSceneManagerPointer()->createSceneNode("Chessboard", pScene));
	Chessboard* board = new Chessboard(pChessboard);
	board->createNewGame();
	GameInfo::g_chessboard = board;


	//Sound
	//m_irrklangEngine->play2D("media/sounds/ophelia.wav", true);

}


ChessEngine::~ChessEngine()
{
	delete GameInfo::g_chessboard;
}

