#include "WaitView.h"

WaitView::WaitView(BRect size)
	: BView(size,"WaitView",B_FOLLOW_ALL,B_WILL_DRAW)
{	text.SetTo(SpTranslate("Processing, please wait..."));
	textpt.Set(10,Bounds().bottom-10);
}

WaitView::WaitView(BRect size, const char *message)
	: BView(size,"WaitView",B_FOLLOW_ALL,B_WILL_DRAW)
{	text.SetTo(message);
	textpt.Set(10,Bounds().bottom-10);
}

void WaitView::Draw(BRect update)
{
	DrawString(text.String(),textpt);
}