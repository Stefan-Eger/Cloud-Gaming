#ifndef UDP_RECEIVE_6_H
#define UDP_RECEIVE_6_H


#include<iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>



class UDPReceive6 {
	typedef struct RTHeader {
		double		  time;
		unsigned long packetnum;
		unsigned char fragments;
		unsigned char fragnum;
	} RTHeader_t;

public:
	int sock = 0;
	struct sockaddr_in6 addr;
	const static int MAX_SEND_BUFFER_SIZE = 65000;
	char receivebuffer[MAX_SEND_BUFFER_SIZE];



	UDPReceive6();
	~UDPReceive6();

	void init(int port);
	int receive(char** buffer, int length, double* ptime=nullptr);
	void closeSocket();

};

#endif // !UDP_SEND_6_H
