#ifndef EVENT_LISTENER_CLOUD_INPUT
#define EVENT_LISTENER_CLOUD_INPUT

class EventListenerCloudInput : public VEEventListener
{
	enum class InputType {
		LEFT_CLICK,
		RIGHT_CLICK,
		PROMOTION_ROOK,
		PROMOTION_KNIGHT,
		PROMOTION_BISHOP,
		PROMOTION_QUEEN
	};

	UDPReceive6* m_receiver;
	Ray createRayThroughPixel(const float& cursorX, const float& cursorY) const;
	void rotateAroundBoard();
	void onMouseButton(const InputType& type, const float& cursorX, const float& cursorY);
	void onKeyboard(const InputType& type);
	std::vector<std::string> splitInput(const std::string& input, const char& delimeter) const;
	void isGameOver();
protected:
	virtual void onFrameEnded(veEvent event) override;
public:
	EventListenerCloudInput(std::string name) ;
	virtual ~EventListenerCloudInput();
};

#endif // !EVENT_LISTENER_CLOUD_INPUT
