#ifndef CHESS_INCLUDE_H
#define CHESS_INCLUDE_H

#define _USE_MATH_DEFINES

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>

#include <glm/glm.hpp>
#include <random>
#include <map>

extern "C" {

#include <libswscale\swscale.h>
#include <inttypes.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}


#include "VEInclude.h"
#include"CLInclude.h"
#include"ChessCoordinates.h"
#include"Ray.h"
#include"Hitbox.h"
#include"SphereHitbox.h"


#include"Figure.h"
#include"Square.h"
#include"Chessboard.h"

#include "UDPSend6.h"
#include "UDPReceive6.h"
#include "GameInfo.h"
#include "EventListenerUser.h"
#include "EventListenerRecord.h"
#include "EventListenerGUI.h"
#include "EventListenerCloudInput.h"
#include "ChessEngine.h"


#endif
