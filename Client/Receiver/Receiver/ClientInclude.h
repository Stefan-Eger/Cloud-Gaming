
//Standard
#include <iostream>;
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <SFML\Graphics.hpp>
#include <queue>
#include <mutex>

//concurrent
#include <thread>
#include <concurrent_queue.h>

//FFMPEG
extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
	#include <libswscale\swscale.h>
}

//Winsock Communication
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>

//My Classes
#include"UDPReceive6.h"
#include"UDPSend6.h"