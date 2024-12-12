#ifndef EVENT_LISTENER_GUI
#define EVENT_LISTENER_GUI

using namespace ve;

class EventListenerGUI : public VEEventListener
{
protected:
	virtual void onDrawOverlay(veEvent event) override;

public:
	EventListenerGUI(std::string name) : VEEventListener(name) { };
	virtual ~EventListenerGUI() {};
};


#endif // !EVENT_LISTENER_GUI
