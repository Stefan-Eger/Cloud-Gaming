#include "ChessInclude.h"

void EventListenerRecord::onFrameEnded(veEvent event)
{

	//As long as timer is below 1 second and less then 5 Screenshots have been taken then make another screenshot
	m_timer += event.dt;
	if (m_timer > TIMER_UPPER_BOUND) {
        std::cout << "Framerate Local: " << m_frame << std::endl;
        m_frame = 0;
		m_seconds++;
		m_timer = 0.0;
        
	}

	if (m_frame < FRAMES_PER_SECOND){
		takeSnapshot();
		++m_frame;
	}
}


void EventListenerRecord::takeSnapshot()
{
    //Retrieving fram from SwapChain
    VkExtent2D extent = getWindowPointer()->getExtent();
    uint32_t imageSize = extent.width * extent.height * 4;
    VkImage image = getEnginePointer()->getRenderer()->getSwapChainImage();

    //uint8_t* rgbaImage = new uint8_t[imageSize];
    uint8_t* rgbaImage = (uint8_t*)calloc(imageSize, sizeof(uint8_t));
    vh::vhBufCopySwapChainImageToHost(getEnginePointer()->getRenderer()->getDevice(),
        getEnginePointer()->getRenderer()->getVmaAllocator(),
        getEnginePointer()->getRenderer()->getGraphicsQueue(),
        getEnginePointer()->getRenderer()->getCommandPool(),
        image, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        rgbaImage, extent.width, extent.height, imageSize);

    //WIDTH and Height has to be a multitude of 2
    //m_codec_context->width = extent.width - extent.width % 2;
    //m_codec_context->height = extent.height - extent.height % 2;

    //TESTING CHANNELS of IMAGE
    //std::string fileName = "screenshot-s" + std::to_string(m_seconds) + "-" + std::to_string(m_frame);
    //std::thread(&EventListenerRecord::writeY2Image, this, fileName, extent.width, extent.height, rgbImage).detach();
    //writeSnapshot2Video(rgbaImage);
    //delete[] rgbaImage;
    std::thread(&EventListenerRecord::writeSnapshot2Video, this, rgbaImage).detach();
}
/*
void EventListenerRecord::writeY2Image(std::string fileName, int width, int height, uint8_t* dataImage)
{
    uint32_t imageSize = width * height;
    uint8_t* YImage = new uint8_t[imageSize];

    for (int y = 0; y < m_codec_context->height; y++) {
        for (int x = 0; x < m_codec_context->width; x++) {
            // Y
            // 0.299, 0.587 and 0.114 used to calculate y from rgb component see slide 11 (04_Codecs.pdf)
            YImage[y * width + x] = static_cast< uint8_t>(0.299 * dataImage[y * m_codec_context->width * 4 + x * 4]) +
                static_cast<uint8_t>(0.587 * dataImage[y * m_codec_context->width * 4 + x * 4 + 1]) +
                static_cast<uint8_t>(0.114 * dataImage[y * m_codec_context->width * 4 + x * 4 + 2]);
        }
    }

    std::string name(FILEPATH + fileName + ".png");
    stbi_write_png(name.c_str(), width, height, 1, YImage, 1 * width);
    delete[] YImage;
    delete[] dataImage;
}

void EventListenerRecord::writeChannel2Image(std::string fileName, int width, int height, uint8_t* dataImage)
{
    uint32_t imageSize = width * height * 4;
    uint32_t selectedChannel = 2; // 0 -> red, 1 -> Green, 2 -> Blue, 3 -> Alpha
    unsigned char* channelImage = new unsigned char[imageSize];

    for (uint32_t i = 0; i < imageSize; i++) {
        channelImage[i] = (i % 4 == selectedChannel) ? dataImage[i] : 0;
        if ((i % 4) == 3) {
            channelImage[i] = dataImage[i];
        }
        //std::cout << "i: " << i << ", ChannelImage[i]: " << channelImage[i] << ", dataImage[i]: " << dataImage[i] << std::endl;
    }

    std::string name(FILEPATH + fileName + ".png");


    stbi_write_png(name.c_str(), width, height, 4, channelImage, 4 * width);
    delete[] channelImage;
    delete[] dataImage;
}
*/
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
void EventListenerRecord::writeSnapshot2Video(uint8_t* rgbaImage)
{
    int err_code;
    AVFrame* frame;


    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = m_codec_context->pix_fmt;
    frame->width = m_codec_context->width;
    frame->height = m_codec_context->height;

    err_code = av_frame_get_buffer(frame, 0);
    if (err_code < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
    }

    /* encode 1 frame of a second */
    fflush(stdout);

    /* Make sure the frame data is writable.
        On the first round, the frame is fresh from av_frame_get_buffer()
        and therefore we know it is writable.
        But on the next rounds, encode() will have called
        avcodec_send_frame(), and the codec may have kept a reference to
        the frame in its internal structures, that makes the frame
        unwritable.
        av_frame_make_writable() checks that and allocates a new buffer
        for the frame only if necessary.
    */
    err_code = av_frame_make_writable(frame);
    if (err_code < 0)
        exit(1);

    /* Prepare a dummy image.
        In real code, this is where you would have your own logic for
        filling the frame. FFmpeg does not care what you put in the
        frame.
        */
    
    for (int y = 0; y < m_codec_context->height; y++) {
        for (int x = 0; x < m_codec_context->width; x++) {
            uint32_t channel_count = 4;
            uint32_t pixel_coordinates = y * m_codec_context->width * channel_count + x * channel_count;
            /* Y */
            // 0.299 * red, 0.587 * green and 0.114 * blue used to calculate y from rgb component see slide 11 (04_Codecs.pdf)
            frame->data[0][y * frame->linesize[0] + x] = static_cast<uint8_t>(0.299 * rgbaImage[pixel_coordinates]) +
                static_cast<uint8_t>(0.587 * rgbaImage[pixel_coordinates + 1]) +
                static_cast<uint8_t>(0.114 * rgbaImage[pixel_coordinates + 2]);
        }
    }

    /*
        Cb and Cr (128 + U and 128 + V)
        For Magic numbers also see slide 11
    */
    for (int y = 0; y < m_codec_context->height / 2; y++) {
        for (int x = 0; x < m_codec_context->width / 2; x++) {

            uint32_t pixel_coordinates = y * m_codec_context->width * 4 * 2 + x * 4 * 2;

            //Cb
            frame->data[1][y * frame->linesize[1] + x] = 128 + static_cast<uint8_t>(-0.168736 * rgbaImage[pixel_coordinates]) +
                static_cast<uint8_t>(-0.331264 * rgbaImage[pixel_coordinates + 1]) +
                static_cast<uint8_t>(0.5 * rgbaImage[pixel_coordinates + 2]);

            //Cr
            frame->data[2][y * frame->linesize[2] + x] = 128 + static_cast<uint8_t>(0.5 * rgbaImage[pixel_coordinates]) +
                static_cast<uint8_t>(-0.418688 * rgbaImage[pixel_coordinates + 1]) +
                static_cast<uint8_t>(-0.081312 * rgbaImage[pixel_coordinates + 2]);
        }
    }
    frame->pts = m_frame + m_seconds * FRAMES_PER_SECOND;

    /* encode the image */
    encode(m_codec_context, frame, m_pkt, m_file);



    av_frame_free(&frame);
    free(rgbaImage);
}
/*
//https://gist.github.com/nakaly/11eb992ebd134ee08b75e4c67afb5703 (25.06.23)
void EventListenerRecord::swsRGBA2YUV(uint8_t* rgbaImage, const int& sizeX, const int& sizeY, AVFrame* outFrame)
{
    struct SwsContext* sws_ctx = NULL;
    sws_ctx = sws_getContext
    (
        sizeX,
        sizeY,
        AVPixelFormat::AV_PIX_FMT_RGBA,
        sizeX,
        sizeY,
        AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL
    );
    uint8_t* inData[1] = { rgbaImage };
    int inLinesize[1] = { 4 * sizeX };
    sws_scale(sws_ctx, inData, inLinesize, 0, sizeY, outFrame->data, outFrame->linesize);
    sws_freeContext(sws_ctx);
}
*/


EventListenerRecord::EventListenerRecord(std::string name) : VEEventListener(name){
    //INITALIZE UDPSOCKET
    m_sender = new UDPSend6();
    m_sender->init((char*)"::1", 9090);

    //INITIALIZE CODEC     
    int err_code;

    /* find the mpeg1video encoder */
    //m_codec = avcodec_find_encoder_by_name(CODEC_NAME.c_str());
    m_codec = avcodec_find_encoder(CODEC_ID);
    if (!m_codec) {
        fprintf(stderr, "Codec not found\n");
    }

    m_codec_context = avcodec_alloc_context3(m_codec);
    if (!m_codec_context) {
        fprintf(stderr, "Could not allocate video codec context\n");
    }

    m_pkt = av_packet_alloc();
    if (!m_pkt)
        fprintf(stderr, "Could not allocate packet\n");

    /* put sample parameters */
    m_codec_context->bit_rate = BIT_RATE;
    /* resolution must be a multiple of two */

    VkExtent2D extent = getWindowPointer()->getExtent();
    m_codec_context->width = extent.width - extent.width % 2;
    m_codec_context->height = extent.height - extent.height % 2;
    /* frames per second */
    m_codec_context->time_base = { 1, FRAMES_PER_SECOND };
    m_codec_context->framerate = { FRAMES_PER_SECOND, 1 };

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    m_codec_context->gop_size = 5;
    m_codec_context->max_b_frames = 1;
    m_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    if (m_codec->id == AV_CODEC_ID_MPEG2VIDEO) {
        av_opt_set(m_codec_context->priv_data, "level", "high1440", 0);
    }
    
    if (m_codec->id == AV_CODEC_ID_H264) {
        av_opt_set(m_codec_context->priv_data, "profile", "high444", 0);
        av_opt_set(m_codec_context->priv_data, "preset", "veryslow", 0);
        av_opt_set(m_codec_context->priv_data, "crf", "22", 0);
    }


    /* open it */
    err_code = avcodec_open2(m_codec_context, m_codec, NULL);
    if (err_code < 0) {
        //std::cout << "Could not open codec:" << av_err2str(ret) << "\n";
        fprintf(stderr, "Could not open codec - Error Code: %d\n", err_code);
    }


    std::string file_str(FILEPATH + VIDEO_FILENAME);
    m_file = fopen(file_str.c_str(), "wb");
    if (!m_file) {
        fprintf(stderr, "Could not open %s\n", VIDEO_FILENAME.c_str());
        exit(1);
    }

    uint8_t startcode[] = { 0x000001 };
    if (m_codec->id == AV_CODEC_ID_MPEG4) {
        fwrite(startcode, 1, sizeof(startcode), m_file);
        auto sentBytes = m_sender->send((char*)startcode, sizeof(startcode), AV_NOPTS_VALUE, AV_NOPTS_VALUE, -1);

        fprintf(stdout, "Sent %d Bytes\n", sentBytes);
    }
}

EventListenerRecord::~EventListenerRecord(){

    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    /* Add sequence end code to have a real MPEG file.
       It makes only sense because this tiny examples writes packets
       directly. This is called "elementary stream" and only works for some
       codecs. To create a valid file, you usually need to write packets
       into a proper file format or protocol; see muxing.c.
    */


    /* flush the encoder */
    encode(m_codec_context, NULL, m_pkt, m_file);

    if (m_codec->id == AV_CODEC_ID_MPEG1VIDEO || m_codec->id == AV_CODEC_ID_MPEG2VIDEO) {
        fwrite(endcode, 1, sizeof(endcode), m_file);
        auto sentBytes = m_sender->send((char*)endcode, sizeof(endcode), AV_NOPTS_VALUE, AV_NOPTS_VALUE, -1);
        fprintf(stdout, "Sent %d Bytes\n", sentBytes);
    }
    fclose(m_file);

    avcodec_free_context(&m_codec_context);
    av_packet_free(&m_pkt);


    m_sender->closeSocket();
    delete m_sender;
}

void EventListenerRecord::encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt, FILE* outfile)
{
    int ret;
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        //printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        //std::cout << "Write packet " << " (size=" << pkt->size << ")\n";
        //fwrite(pkt->data, 1, pkt->size, outfile);

        auto sentBytes = m_sender->send((char*)pkt->data, pkt->size, pkt->pts, pkt->dts, pkt->pos);
        //fprintf(stdout, "Sent %d Bytes\n", sentBytes);

        av_packet_unref(pkt);
    }
}

