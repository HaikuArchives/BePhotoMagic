#include "ColorWindow.h"

ColorWindow::ColorWindow(BPoint pt,bool foremode=true)
				: BWindow(BRect(0+pt.x,0+pt.y,410+pt.x,280+pt.y), "", 
				B_FLOATING_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE |
				B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
	if(foremode==true)
	{	msgcmd=FOREWIN_CLOSED;
		SetTitle(SpTranslate("Foreground"));
		mcview = new MCView(FORECOLOR_CHANGED);
	}
	else
	{	msgcmd=BACKWIN_CLOSED;
		SetTitle(SpTranslate("Background"));
		mcview = new MCView(BACKCOLOR_CHANGED);
	}
	AddChild(mcview);
}

ColorWindow::~ColorWindow(void)
{
	BMessage *msg=new BMessage(msgcmd);
	msg->AddRect("frame",Frame());
	be_app->PostMessage(msg);
}

rgb_color ColorWindow::GetColor(void)
{	return mcview->currentcolor;
}

void ColorWindow::SetColor(rgb_color color)
{	mcview->SetColor(color);
}

#ifndef CHILDWIN_MOVED
	#define CHILDWIN_MOVED 'cwmv'
#endif

void ColorWindow::FrameMoved(BPoint origin)
{
	BMessage *msg=new BMessage(CHILDWIN_MOVED);
	if(msgcmd==FOREWIN_CLOSED)
		msg->AddPoint("forewin",origin);
	else
		msg->AddPoint("backwin",origin);
	be_app->PostMessage(msg);
}
