#include "DrawButton2S.h"

DrawButton2S::DrawButton2S(BRect frame, const char *name, int32 msg, int32 resize, int32 flags)
	: BView(frame, name, resize, flags)
{	msgcmd=msg;
	usebitmap=false;
	centerbitmap=false;
	onstate=false;
}

DrawButton2S::~DrawButton2S(void)
{	delete onbmp;
	delete offbmp;
}

void DrawButton2S::AttachedToWindow(void)
{	target=Window();
}

void DrawButton2S::SetValue(bool state)
{
	onstate=state;
	Invalidate();
}

bool DrawButton2S::Value(void)
{
	return onstate;
}

void DrawButton2S::SetBitmaps(BBitmap *on,BBitmap *off)
{	
	onbmp=on;
	offbmp=off;
	usebitmap=true;
/*	if(Bounds().Width() <= display->Bounds().Width() && Bounds().Height() <= display->Bounds().Height())
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
*/
}

void DrawButton2S::DisableBitmaps(void)
{	usebitmap=false;
	Draw(Bounds());
}

void DrawButton2S::SetTarget(BLooper *tgt)
{	target=tgt;
}

void DrawButton2S::MouseUp(BPoint where)
{	if(msgcmd!=0)
		target->PostMessage(new BMessage(msgcmd));
	onstate=(onstate==true)?false:true;
	Draw(Bounds());
}

void DrawButton2S::Draw(BRect update)
{	if(usebitmap && onbmp!=NULL && offbmp!=NULL)
	{	
//		if(centerbitmap==true)
//			DrawBitmap(display,display->Bounds(),centered);
//		else
		if(onstate==true)
			DrawBitmap(offbmp,offbmp->Bounds(),Bounds());
		else
			DrawBitmap(onbmp,onbmp->Bounds(),Bounds());
	}
}

