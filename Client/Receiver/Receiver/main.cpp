#include"ClientInclude.h"

using namespace std;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int FRAMES_PER_SECOND = 25;
bool isRunning = true;

const AVCodec* codec;
AVCodecParserContext* parser;
AVCodecContext* c = NULL;
FILE* f;
AVFrame* frame;
AVPacket* pkt;


//Concurrency::concurrent_queue<uint8_t*> framebuffer;
std::queue<uint8_t*> frameQueue;
std::mutex queueMutex;
uint32_t queueSampleCounter = 0;

static void initFFMPEG() {

	pkt = av_packet_alloc();
	if (!pkt)
		exit(1);
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	parser = av_parser_init(codec->id);
	if (!parser) {
		fprintf(stderr, "parser not found\n");
		exit(1);
	}
	parser->flags = PARSER_FLAG_COMPLETE_FRAMES;

	c = avcodec_alloc_context3(codec);

	c->bit_rate = 800000;
	/* resolution must be a multiple of two */
	c->width = WINDOW_WIDTH;
	c->height = WINDOW_HEIGHT;
	c->coded_width = WINDOW_WIDTH;
	c->coded_height = WINDOW_HEIGHT;
	/* frames per second */
	c->time_base = { 1, FRAMES_PER_SECOND };
	c->framerate = { FRAMES_PER_SECOND, 1 };
	c->gop_size = 10;
	c->max_b_frames = 1;
	c->pix_fmt = AV_PIX_FMT_YUV420P;

	if (!c) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	/* For some codecs, such as msmpeg4 and mpeg4, width and height
	   MUST be initialized there because this information is not
	   available in the bitstream. */


	   /* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
}
static void cleanupFFMPEG() {
	av_parser_close(parser);
	avcodec_free_context(&c);
	av_frame_free(&frame);
	av_packet_free(&pkt);
}

static void pgm_save(unsigned char* buf, int wrap, int xsize, int ysize, char* filename)
{
	FILE* f;
	int i;

	f = fopen(filename, "wb");
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
	for (i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);
	fclose(f);
}

static void ppm_save_rgb(uint8_t* image, int xsize, int ysize, char* filename)
{
	FILE* f;

	/*
	* Channel width 3 -> interleave 3 each pixel to gain access to new pixel
		R -> image[0],
		G -> image[1],
		B -> image[2],
	*/
	f = fopen(filename, "wb");
	fprintf(f, "P6\n%d %d\n%d\n", xsize, ysize, 255);
	for (int y = 0; y < ysize; y++) {
		for (int x = 0; x < xsize; x++) {
			uint32_t pixelCoordinate = y * xsize * 3 + x * 3;
			unsigned char rgb[3];

			rgb[0] = image[pixelCoordinate];
			rgb[1] = image[pixelCoordinate + 1];
			rgb[2] = image[pixelCoordinate + 2];

			fwrite(rgb, 1, 3, f);
		}
	}
	fclose(f);
}

static void ppm_save(unsigned char** buf, int* wrap, int xsize, int ysize, char* filename)
{
	FILE* f;

	/*
		Y -> buf[0][...], wrap[0]
		U -> buf[1][...] - 128, wrap[1]
		V -> buf[2][...] - 128, wrap[2]
	*/
	f = fopen(filename, "wb");
	fprintf(f, "P6\n%d %d\n%d\n", xsize, ysize, 255);
	for (int y = 0, yhalf = 0; y < ysize; y++, yhalf = y / 2) {
		for (int x = 0, xhalf = 0; x < xsize; x++, xhalf = x / 2) {
			unsigned char rgb[3];

			rgb[0] = buf[0][y * wrap[0] + x] + (unsigned char)((1.0 / 0.877) * (buf[2][yhalf * wrap[2] + xhalf] - 128));
			rgb[2] = buf[0][y * wrap[0] + x] + (unsigned char)((1.0 / 0.493) * (buf[1][yhalf * wrap[1] + xhalf] - 128));

			rgb[1] = buf[0][y * wrap[0] + x] -
				(unsigned char)(0.39393 * (buf[1][yhalf * wrap[1] + xhalf] - 128)) -
				(unsigned char)(0.58081 * (buf[2][yhalf * wrap[2] + xhalf] - 128));

			fwrite(rgb, 1, 3, f);
		}
	}
	fclose(f);
}

static uint8_t* yuv2rgba(unsigned char** buf, int* wrap, int xsize, int ysize, char* filename) {
	const int CHANNEL_COUNT = 4;
	uint8_t* rgbImage = new uint8_t[WINDOW_WIDTH * WINDOW_HEIGHT * CHANNEL_COUNT];
	for (int y = 0, yhalf = 0; y < ysize; y++, yhalf = y / 2) {
		for (int x = 0, xhalf = 0; x < xsize; x++, xhalf = x / 2) {
			unsigned char rgb[CHANNEL_COUNT];
			uint32_t pixelCoordinate = y * WINDOW_WIDTH * CHANNEL_COUNT + x * CHANNEL_COUNT;


			rgb[0] = buf[0][y * wrap[0] + x] + (unsigned char)((1.0 / 0.877) * (buf[2][yhalf * wrap[2] + xhalf] - 128));
			rgb[2] = buf[0][y * wrap[0] + x] + (unsigned char)((1.0 / 0.493) * (buf[1][yhalf * wrap[1] + xhalf] - 128));

			rgb[1] = buf[0][y * wrap[0] + x] -
				(unsigned char)(0.39393 * (buf[1][yhalf * wrap[1] + xhalf] - 128)) -
				(unsigned char)(0.58081 * (buf[2][yhalf * wrap[2] + xhalf] - 128));

			rgbImage[pixelCoordinate] = (uint8_t)rgb[0];
			rgbImage[pixelCoordinate + 1] = (uint8_t)rgb[1];
			rgbImage[pixelCoordinate + 2] = (uint8_t)rgb[2];
			rgbImage[pixelCoordinate + 3] = std::numeric_limits<uint8_t>::max();

		}
	}
	return rgbImage;
}

static uint8_t* swsYUV2RGBA(AVFrame* frame, int xsize, int ysize) {
	struct SwsContext* sws_ctx = NULL;
	sws_ctx = sws_getContext
		(
			xsize,
			ysize,
			(AVPixelFormat)frame->format,
			xsize,
			ysize,
			AV_PIX_FMT_RGBA,
			SWS_BILINEAR, NULL, NULL, NULL
		);
	uint8_t* prgba32 = (uint8_t*) calloc(4 * xsize * ysize, sizeof(uint8_t));
	uint8_t* rgba32[1] = { prgba32 };
	int rgba32_stride[1] = { 4 * xsize };
	sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, rgba32, rgba32_stride);
	sws_freeContext(sws_ctx);

	return prgba32;
}



static int decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt, const char* filename)
{
	char buf[1024];
	int ret;

	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		return ret;
	}



	while (ret >= 0) {
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			return 0;
		}
		if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			return ret;
		}

		//printf("saving frame %3d\n", dec_ctx->frame_number);
		//fflush(stdout);

		/* the picture is allocated by the decoder. no need to free it */


		//snprintf(buf, sizeof(buf), "%s-%d.ppm", filename, dec_ctx->frame_number);
		//ppm_save(frame->data, frame->linesize, frame->width, frame->height, buf);


		//uint8_t* rgbData = yuv2rgba(frame->data, frame->linesize, frame->width, frame->height, buf);
		uint8_t* rgbData = swsYUV2RGBA(frame, frame->width, frame->height);
		queueMutex.lock();
		frameQueue.push(rgbData);
		queueMutex.unlock();
	}
}
static int decodeMPEGStream(uint8_t* data, size_t data_size, std::string& fileName, int64_t* pPts, int64_t* pDts, int64_t* pPos, double* ptime) {
	int ret = 0;
	/* use the parser to split the data into frames */
	while (data_size > 0) {

		int bytes = av_parser_parse2(parser, c, &pkt->data, &pkt->size, (uint8_t*)data, data_size, AV_NOPTS_VALUE, *pDts, -1);
		if (bytes < 0) {
			fprintf(stderr, "Error while parsing\n");
			return bytes;
		}
		data_size -= bytes;
		if (pkt->size > 0) {
			ret = decode(c, frame, pkt, fileName.c_str());
		}
	}
	return ret;
}


void receiveAndDecode() {
	initFFMPEG();
	UDPReceive6 receiver;
	receiver.init(9090);

	std::string fileName("Frame");
	int error = -1;
	while (isRunning) {
		int64_t pts;
		int64_t dts;
		int64_t pos;
		double ptime;

		//DECODE stream into single Images
		char* buf = nullptr;
		int buf_size = receiver.receive(&buf, sizeof(buf), &pts, &dts, &pos, &ptime);
		//memset(buf, 0, MAX_BUFFER_SIZE);
		//std::cout << " Received Package of size: " << buf_size << " bytes" << std::endl;
		error = decodeMPEGStream((uint8_t*)buf, buf_size, fileName, &pts, &dts, &pos, &ptime);
		free(buf);
		//free(buf);
		if (error == AVERROR_INVALIDDATA) {
			std::cout << "SEARCHING FOR I_FRAME..." << std::endl;
		}
	}

	cleanupFFMPEG();
}

void cleanupWindow() {
	std::lock_guard<std::mutex>lock(queueMutex);

	while (!frameQueue.empty()) {
		uint8_t* frameRGB = frameQueue.front();
		frameQueue.pop();
		delete[] frameRGB;
	}
}

int main() {
	//receiveAndDecode();
	std::thread producer(receiveAndDecode);

	//Init Window
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chess!", sf::Style::Close | sf::Style::Titlebar);
	sf::Texture texture;
	if (!texture.create(WINDOW_WIDTH, WINDOW_HEIGHT))
		return -1;
	sf::Sprite sprite(texture);

	//Init Input Communication
	UDPSend6 sender;
	sender.init((char*)"::1", 9091);

	//Init Timer
	int frame = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	long long duration = 0;
	const long TIMER_UPPER_BOUND_MILLISECONDS = 1000;


	while (window.isOpen())
	{
		//next Timer Step
		auto end_time = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
		//Restarting Timer
		if (duration > TIMER_UPPER_BOUND_MILLISECONDS) {
			std::cout << "Samples displayed: " << queueSampleCounter << std::endl;
			queueSampleCounter = 0;
			duration = 0;
			start_time = end_time;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				isRunning = false;
				window.close();
			}
			//Input MouseClick
			if (event.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					auto mousePos = sf::Mouse::getPosition(window);
					std::cout << "Left Click[" << mousePos.x << ", " << mousePos.y << "]" << std::endl;
					std::string msg = ("Left Click," + std::to_string(mousePos.x) + ',' + std::to_string(mousePos.y));
					sender.send(msg.data(), msg.size());
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
					auto mousePos = sf::Mouse::getPosition(window);
					std::cout << "Right Click[" << mousePos.x << ", " << mousePos.y << "]" << std::endl;
					std::string msg = ("Right Click," + std::to_string(mousePos.x) + ',' + std::to_string(mousePos.y));
					sender.send(msg.data(), msg.size());
				}
			}
			//Input Keyboard
			if (event.type == sf::Event::TextEntered) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
					std::cout << "Pressed R for Rook" << std::endl;
					std::string msg = ("Keyboard,R\0");
					sender.send(msg.data(), msg.size());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
					std::cout << "Pressed K for Knight" << std::endl;
					std::string msg = ("Keyboard,K\0");
					sender.send(msg.data(), msg.size());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
					std::cout << "Pressed B for Bishop" << std::endl;
					std::string msg = ("Keyboard,B\0");
					sender.send(msg.data(), msg.size());
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
					std::cout << "Pressed Q for Queen" << std::endl;
					std::string msg = ("Keyboard,Q\0");
					sender.send(msg.data(), msg.size());
				}
			}
		}

		if (queueSampleCounter < (FRAMES_PER_SECOND+10)) {
			//Poll all Events of Window example (TextEvent, Resize, ...)

			//Assign new Frame
			if (!frameQueue.empty()) {

				queueMutex.lock();
				uint8_t* frameRGB = frameQueue.front();
				frameQueue.pop();
				queueMutex.unlock();


				sf::Uint8* pixels = frameRGB;
				texture.update(pixels);
				++queueSampleCounter;

				//Draw and display new Frame
				window.clear();
				window.draw(sprite);
				window.display();
				free(frameRGB);
			}
		}
	}
	producer.join();
	cleanupWindow();
	return 0;
}