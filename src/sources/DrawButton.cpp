#include "DrawButton.h"

DrawButton::DrawButton(BRect frame, const char *name, int32 msg, int32 resize, int32 flags)
	: BView(frame, name, resize, flags)
{	msgcmd=msg;
	usebitmap=false;
	centerbitmap=false;
}

DrawButton::~DrawButton(void)
{
}

void DrawButton::AttachedToWindow(void)
{	target=Window();
}

void DrawButton::SetBitmap(BBitmap *bmp)
{	display=bmp;
	usebitmap=true;
	if(Bounds().Width() <= display->Bounds().Width() && Bounds().Height() <= display->Bounds().Height())
		centerbitmap=false;
	else
	{	if(Bounds().Width() > display->Bounds().Width())
		{	centered.left=(Bounds().Width()-display->Bounds().Width())/2;
			centered.right=centered.left+display->Bounds().Width()-1;
		}
		else
		{	centered.left=0;
			centered.right=display->Bounds().Width()-1;
		}
		if(Bounds().Height() > display->Bounds().Height())
		{	centered.top=(Bounds().Height()-display->Bounds().Height())/2;
			centered.bottom=centered.top+display->Bounds().Height()-1;
		}
		else
		{	centered.top=0;
			centered.bottom=display->Bounds().Height()-1;
		}
		centerbitmap=true;
	}

}

void DrawButton::DisableBitmap(void)
{	usebitmap=false;
	Draw(Bounds());
}

void DrawButton::SetTarget(BLooper *tgt)
{	target=tgt;
}

void DrawButton::MouseUp(BPoint where)
{	if(msgcmd!=0)
		target->PostMessage(new BMessage(msgcmd));
}
#include <stdio.h>
void DrawButton::Draw(BRect update)
{	if(usebitmap && display!=NULL)
	{	
		if(centerbitmap==true)
			DrawBitmap(display,display->Bounds(),centered);
		else
			DrawBitmap(display,display->Bounds(),Bounds());
	}
}

void DrawButton::Update(void)
{	Draw(Bounds());
}