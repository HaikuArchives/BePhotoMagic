#ifndef _DRAW_BUTTON2S_H

#include <Looper.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Rect.h>

#define _DRAW_BUTTON2S_H

class DrawButton2S : public BView
{
public:
	DrawButton2S(BRect frame, const char *name, int32 msg, int32 resize, int32 flags);
	~DrawButton2S(void);

	void Draw(BRect update);
	void MouseUp(BPoint where);
	void AttachedToWindow(void);

	void SetBitmaps(BBitmap *on, BBitmap *off);
	void DisableBitmaps(void);
	void SetTarget(BLooper *tgt);
	void SetValue(bool state);
	bool Value(void);
	
	BBitmap *onbmp,*offbmp;
	BLooper *target;
	int32 msgcmd;
	BRect centered;
	bool usebitmap;
	bool centerbitmap;
	bool onstate;
};

#endif