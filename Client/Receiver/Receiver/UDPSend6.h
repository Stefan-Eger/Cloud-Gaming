#ifndef UDP_SEND_6_H
#define UDP_SEND_6_H



class UDPSend6 {

	typedef struct RTHeader {
		double		  time;
		unsigned long packetnum;
		unsigned char fragments;
		unsigned char fragnum;
	} RTHeader_t;

	typedef struct ip6_hbh {
		uint8_t  ip6h_nxt;        /* next header */
		uint8_t  ip6h_len;        /* length in units of 8 octets */
		/* followed by options */
	}ip6_hbh_t;

	const static int MAX_SEND_BUFFER_SIZE = 65000;

	int sendPackage(char* buffer, int length);
public:
	int sock = 0;
	struct sockaddr_in6 addr;
	unsigned int packetnum = 0;
	RTHeader_t header;

	UDPSend6();
	~UDPSend6();

	void init(char* address, int port);
	int send(char* buffer, int length);
	void closeSocket();

};

#endif // !UDP_SEND_6_H
