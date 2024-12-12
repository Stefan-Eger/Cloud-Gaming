#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

using namespace ve;

class ChessEngine : public VEEngine
{
public:
	ChessEngine(ve::veRendererType type = veRendererType::VE_RENDERER_TYPE_FORWARD, bool debug = false) : VEEngine(type, debug) {};
	~ChessEngine();

	virtual void registerEventListeners() override;
	virtual void loadLevel(uint32_t numLevel = 1) override;

};

#endif
