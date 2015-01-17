#ifndef MC_VIEW_H
#define MC_VIEW_H

#include <Looper.h>
#include <Invoker.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <TabView.h>
#include <TextControl.h>
#include <ColorControl.h>
#include <Bitmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SpLocaleApp.h"
#include "Utils.h"

// Passes to Target
#define MC_COLOR_CHANGED 'mccc'

// Internal Messages

// BColorControl
#define RGB_CHANGED	'rgbc'

// HSVView
#define NUMBERS_CHANGED 'nbch'
#define SQUARE_CHANGED 'sqch'
#define SPECTRUM_CHANGED 'spch'

class SatValView : public BView
{
public:
	SatValView(BRect frame, const char *name, int32 resize, int32 flags);
	~SatValView(void);

	void Draw(BRect update_rect);
	void AttachedToWindow(void);
	void MouseUp(BPoint where);
	void MouseDown(BPoint where);
	void MouseMoved(BPoint where, uint32 transit, const BMessage *message);
	
	BLooper *target;
	BInvoker *msgsender;
	BPoint cursor;
	
	rgb_color PointToColor(BPoint pt);
	BPoint ColorToPoint(rgb_color color);
	BPoint ColorToPoint(hsv_color color);
	void CreateSquare(rgb_color col);
	void CreateSquare(uint8 r,uint8 g,uint8 b);
	BBitmap *display;
};

class HueView : public BView
{
public:
	HueView(BRect frame, const char *name, int32 resize, int32 flags);
	~HueView(void);

	// Overloaded hook functions
	void Draw(BRect update_rect);
	void AttachedToWindow(void);
	void MouseUp(BPoint where);
	void MouseDown(BPoint where);
	void MouseMoved(BPoint where, uint32 transit, const BMessage *message);

	// Methods	
	void CreateSpectrum(void);
	int16 PointToHue(BPoint pt);
	BPoint HueToPoint(uint16 hue);

	// Properties
//	BLooper *target;
	BInvoker *msgsender;
	BBitmap *display;
	int16 value;
	int8 leftx,rightx;
};

class HSVView : public BView 
{
public:
	HSVView(void);
	void MessageReceived(BMessage *msg);
	void AttachedToWindow(void);

	// Controls
	BTabView *tabview;
	BColorControl *rgb;
	SatValView *square;
	HueView *spectrum;
	BTextControl *hue,*sat,*val;

	// Methods	
	void SetColor(hsv_color hsvcol);
	void SetColor(rgb_color rgbcol);
	rgb_color GetColor(void);
	void Update(void);
	void UpdateTControls(void);
	hsv_color GetTextControlsColor(void);
};

class MCView : public BView 
{
public:
	MCView(uint32 command);
	~MCView(void);
	void MessageReceived(BMessage *msg);
	void AttachedToWindow(void);
	
	// Controls
	BTabView *tabview;
	BColorControl *rgb;
	HSVView *hsv;

	// Methods	
	void SetTarget(BHandler *tgt);
	void SetColor(rgb_color color);
	void SetColor(hsv_color color);
	
	rgb_color currentcolor;
	BInvoker *msgsender;
};

#endif
