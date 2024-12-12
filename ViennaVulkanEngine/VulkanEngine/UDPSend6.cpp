#include "ChessInclude.h"


UDPSend6::UDPSend6() {
	WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err != 0)
		printf("WSAStartup failed with error: %d\n", err);
}

UDPSend6::~UDPSend6() {
	closesocket(sock);
	WSACleanup();
}

void UDPSend6::init(char* address, int port)
{
	sock = socket(AF_INET6, SOCK_DGRAM, 0);
	
	struct addrinfo hints;
	memset(&addr, 0, sizeof(addr));
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	struct addrinfo* result = NULL;
	auto dwRetval = getaddrinfo(address, nullptr, &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		return;
	}
	for (addrinfo* ptr = result; ptr != NULL;
		ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_INET6) {
			memcpy(&addr, ptr->ai_addr, ptr->ai_addrlen);
			addr.sin6_port = htons(port);
			addr.sin6_family = AF_INET6;
		}
	}
	freeaddrinfo(result);
}

int UDPSend6::sendPackage(char* buffer, int length)
{
	
	int ret = -1;
	if (length + sizeof(header) <= MAX_SEND_BUFFER_SIZE) {
		char* sendbuffer = (char*)malloc(MAX_SEND_BUFFER_SIZE);

		if (sendbuffer != NULL) {
			memcpy(sendbuffer, &header, sizeof(header));
			memcpy(sendbuffer + sizeof(header), buffer, length);
			ret = sendto(sock, sendbuffer, length + sizeof(header), 0, (const struct sockaddr*)&addr, sizeof(addr));
			free(sendbuffer);
		}
	}

	
	return ret;
}

int UDPSend6::send(char* buffer, int length, int64_t pts, int64_t dts, int64_t pos)
{

	int ret = 0;
	header.time = clock() / (double)CLOCKS_PER_SEC;
	header.pts = pts;
	header.dts = dts;
	header.pos = pos;

	char* data_package_start = &buffer[0];
	int length_package = length;
	ret += sendPackage(data_package_start, length_package);
	
	return ret;

}

void UDPSend6::closeSocket()
{
	closesocket(sock);
}
