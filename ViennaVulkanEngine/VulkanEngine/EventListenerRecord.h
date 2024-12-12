#ifndef EVENT_LISTENER_RECORD_H
#define EVENT_LISTENER_RECORD_H

using namespace ve;

class EventListenerRecord : public VEEventListener
{
	const double TIMER_UPPER_BOUND = 1.0;
	const int FRAMES_PER_SECOND = 25;
	const std::string FILEPATH = "media/screenshots/";

	AVCodecID CODEC_ID = AV_CODEC_ID_H264;
	const std::string VIDEO_FILENAME = "video.mpg";
	int64_t	BIT_RATE = 800000;

	double m_timer = 0.0;
	uint64_t m_seconds = 0;
	uint64_t m_frame = 0;

	AVCodec* m_codec;
	AVCodecContext* m_codec_context = NULL;
	AVPacket* m_pkt;
	FILE* m_file;


	UDPSend6* m_sender;
	

	void takeSnapshot();
	void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, FILE* outfile);
	void writeSnapshot2Video(uint8_t* rgbaImage);
	//void swsRGBA2YUV(uint8_t*rgbaImage, const int& sizeX, const int& sizeY, AVFrame* outFrame);

protected:
	virtual void onFrameEnded(veEvent event) override;

public:
	EventListenerRecord(std::string name);
	virtual ~EventListenerRecord();
	//void writeChannel2Image(std::string fileName, int width, int height, uint8_t* dataImage);
	//void writeY2Image(std::string fileName, int width, int height, uint8_t* dataImage);
	

};

#endif // !EVENT_LISTENER_RECORD_H

