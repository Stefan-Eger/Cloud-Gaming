#include "ChessInclude.h"

UDPReceive6::UDPReceive6() {
	WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err != 0)
		printf("WSAStartup failed with error: %d\n", err);
}

UDPReceive6::~UDPReceive6()
{
	closesocket(sock);
	WSACleanup();
}

void UDPReceive6::init(int port)
{
	sock = socket(AF_INET6, SOCK_DGRAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_any;

	//WINDOWS SPECIFIC TIMEOUT
	//https://stackoverflow.com/questions/30395258/setting-timeout-to-recv-function (25.06.23)
	DWORD timeout = (DWORD)(0.001 * 1000);
	auto error = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	
	auto ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	//printf("Binding port %d return %d errno %d\n", port, ret, WSAGetLastError());


}

int UDPReceive6::receive(char** buffer, int length, double* ptime)
{
	//Empty buffer before receiveing a new
	memset(receivebuffer, 0, sizeof(receivebuffer));
	struct sockaddr_in6 si_other;
	socklen_t slen = sizeof(si_other);
	RTHeader_t* pheader = (RTHeader_t*)receivebuffer;
	int header_size = sizeof(RTHeader_t);

	int ret = recvfrom(sock, receivebuffer, UDPReceive6::MAX_SEND_BUFFER_SIZE, 0, (sockaddr*)&si_other, &slen);
	if (ret > header_size) {
		auto buffer_size_no_header = ret - header_size;
		if (ptime != nullptr) {
			*ptime = pheader->time;
		}
		uint8_t packetNum = pheader->packetnum;

		*buffer = (char*)malloc(buffer_size_no_header+1);

		if (*buffer != nullptr) {
			memcpy(*buffer, receivebuffer + header_size, buffer_size_no_header);
			(*buffer)[buffer_size_no_header] = '\0';
		}

		return ret - header_size;
	}

	return ret;
}

void UDPReceive6::closeSocket()
{
	closesocket(sock);
}
