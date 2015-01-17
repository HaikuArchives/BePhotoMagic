#ifndef _COLORWINDOW_H
#define _COLORWINDOW_H

#include <Application.h>
#include <Window.h>
#include <Message.h>

#include "SpLocaleApp.h"
#include "MCView.h"

#define FOREWIN_CLOSED 'fcwc'
#define BACKWIN_CLOSED 'bcwc'
#define FORECOLOR_CHANGED 'fcch'
#define BACKCOLOR_CHANGED 'bcch'

class ColorWindow : public BWindow 
{
public:
	ColorWindow(BPoint pt,bool foremode=true); 
	~ColorWindow(void); 
	rgb_color GetColor(void);
	void SetColor(rgb_color color);
	void FrameMoved(BPoint origin);
	
	MCView	*mcview;
	uint32 msgcmd;
};

#endif
