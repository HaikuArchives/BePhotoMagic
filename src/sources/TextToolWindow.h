#include <Application.h>
#include <Window.h>
#include <View.h>
#include <stdio.h>
#include <Bitmap.h>
#include <string.h>

#include "TextToolView.h"

#ifndef _TEXT_TOOL_WINDOW_H_

#define _TEXT_TOOL_WINDOW_H_

#define TEXTWIN_CLOSED 'txwc'

class TextToolWindow : public BWindow
{
public:
	TextToolWindow (void);
	~TextToolWindow (void);

//	virtual bool QuitRequested(void);
	virtual void MessageReceived(BMessage *msg);

	void PostBitmap(void);
	TextToolView *containerview;
	BFont *currentfont;
	char text[255];
};

#endif
