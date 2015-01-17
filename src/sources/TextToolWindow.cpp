#include "TextToolWindow.h"

TextToolWindow::TextToolWindow(void)
 : BWindow(BRect(100, 100, 600, 545), "Text Tool", B_FLOATING_WINDOW, 
 B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_NOT_RESIZABLE |
 B_WILL_ACCEPT_FIRST_CLICK, B_CURRENT_WORKSPACE)
{
	containerview = new TextToolView(Bounds());
	AddChild(containerview);
}

TextToolWindow::~TextToolWindow(void)
{	be_app->PostMessage(TEXTWIN_CLOSED);
}

void TextToolWindow::MessageReceived(BMessage *msg)
{	BRect winrect;

	switch(msg->what)
	{
		case RENDER_TEXT:
			PostBitmap();
			break;

		case FONTVIEW_TOGGLE:
			winrect=Bounds();
			// Rectangles are same width - font display is hidden
			if(winrect.IntegerWidth()==500)
			{
				ResizeBy(-379.0,0.0);
				containerview->fontviewtoggle->SetLabel(SpTranslate("Show Text"));
			}
			else
			{	ResizeBy(379,0);
				containerview->fontviewtoggle->SetLabel(SpTranslate("Hide Text"));
			}
			break;

		default:
			BWindow::MessageReceived(msg);
	}
	
}

void TextToolWindow::PostBitmap(void)
{
	BRect bmpsize;
	BView *draw;
	BBitmap *bmp;
	
	bmpsize=containerview->fontview->bbox;
	bmpsize.OffsetTo(0.0,0.0);
	
	bmp=new BBitmap(bmpsize,B_RGBA32,true);

	if(bmp==NULL)
		return;
	
	draw=new BView(bmpsize,"drawview",B_FOLLOW_NONE,B_WILL_DRAW);
	if(draw==NULL)
	{	delete bmp;
		return;
	}
	
	draw->SetHighColor(0,0,0,255);

	bmp->Lock();
	bmp->AddChild(draw);
	
	draw->SetFont(&containerview->fontview->currentfont);
	draw->DrawString(containerview->fontview->text,
				BPoint(draw->Bounds().left,draw->Bounds().bottom-
				containerview->fontview->descent) );

	bmp->RemoveChild(draw);
	bmp->Unlock();

	delete draw;

	be_app->PostMessage(Bitmap2Message(bmp));
	delete bmp;
}

/*
bool TextToolWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
*/
