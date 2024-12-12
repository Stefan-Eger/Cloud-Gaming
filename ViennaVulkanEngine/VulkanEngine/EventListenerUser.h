#ifndef EVENT_LISTENER_USER_H
#define EVENT_LISTENER_USER_H

//This class was mainly copied from my Bachelor thesis. 
class EventListenerUser : public VEEventListenerGLFW
{
	Ray createRayThroughPixel(const float& cursorX, const float& cursorY) const;
	void rotateAroundBoard();
	void isGameOver();

protected:
	virtual bool onMouseButton(veEvent event) override;
	virtual bool onKeyboard(veEvent event) override;
public:
	EventListenerUser(std::string name);
	virtual ~EventListenerUser();

};

#endif

