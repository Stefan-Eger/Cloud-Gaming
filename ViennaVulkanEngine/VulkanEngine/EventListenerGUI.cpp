#include "ChessInclude.h"

void EventListenerGUI::onDrawOverlay(veEvent event)
{
	VESubrender_Nuklear* pSubrender = (VESubrender_Nuklear*)getEnginePointer()->getRenderer()->getOverlay();
	if (pSubrender == nullptr) return;


	if (GameInfo::g_gameOver) {
		struct nk_context* ctx = pSubrender->getContext();
		if (nk_begin(ctx, "", nk_rect(0, 0, 350, 120), NK_WINDOW_BORDER)) {
			char outbuffer[100];

			nk_layout_row_dynamic(ctx, 45, 1);
			sprintf(outbuffer, "White has %s", GameInfo::g_chessboard->isWhiteKingCheckMate() ? "Lost" : "Won");
			nk_label(ctx, outbuffer, NK_TEXT_LEFT);


			nk_layout_row_dynamic(ctx, 45, 1);
			sprintf(outbuffer, "Black has %s", GameInfo::g_chessboard->isBlackKingCheckMate() ? "Lost" : "Won");
			nk_label(ctx, outbuffer, NK_TEXT_LEFT);
		}
		nk_end(ctx);
	}

	
	
}
