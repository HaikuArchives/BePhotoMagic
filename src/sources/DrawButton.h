#ifndef _DRAW_BUTTON_H

#include <Looper.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Rect.h>

#define _DRAW_BUTTON_H

class DrawButton : public BView
{
public:
	DrawButton(BRect frame, const char *name, int32 msg, int32 resize, int32 flags);
	~DrawButton(void);

	void Draw(BRect update);
	void MouseUp(BPoint where);
	void AttachedToWindow(void);

	void SetBitmap(BBitmap *bmp);
	void DisableBitmap(void);
	void SetTarget(BLooper *tgt);
	void Update(void);
	
	BBitmap *display;
	BLooper *target;
	int32 msgcmd;
	BRect centered;
	bool usebitmap;
	bool centerbitmap;
};

#endif