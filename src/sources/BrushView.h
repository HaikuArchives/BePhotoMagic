#include <Application.h>
#include <View.h>
#include <Rect.h>
#include <CheckBox.h>
#include <Message.h>
//#include <Picture.h>
//#include <Bitmap.h>
#include <stdio.h>
#include <string.h>

#include "TSlider.h"
#include "SpLocaleApp.h"
//#include "DrawButton.h"

#ifndef _BRUSHVIEW_H_

#define _BRUSHVIEW_H_

// lone externa message
#define SET_BRUSH		'bsup'

// Internal messages
#define BRUSH_HEIGHT_CHANGE	'bsvh'
#define BRUSH_WIDTH_CHANGE 	'bsvw'
#define BRUSH_BLUR_CHANGE 	'bsvb'
#define BRUSH_SET_RECT		'bssr'
#define BRUSH_EVEN_ASPECT	'bsea'
#define BRUSH_LOCK_ASPECT	'bsla'

#define BRUSH_TYPE_ELLIPSE	0
#define BRUSH_TYPE_RECT		1

// Defined also in BPMImage.h
#ifndef SET_BRUSH_CONTROLS
	#define SET_BRUSH_CONTROLS 'sbwc'
#endif

class BrushSelectorView : public BView
{	
public:
	
	BrushSelectorView(BRect r,uint16 width,uint16 height,int8 softness, int8 type);
	virtual ~BrushSelectorView(void);
	void AttachedToWindow(void);
//	void makebrush(int16 width, int16 height);
	void PostBrush(void);
	void SetControls(BMessage *msg);
	
	virtual void MessageReceived(BMessage *msg);

	TSlider	*WidthSlider;
	TSlider *HeightSlider;
	TSlider *BlurSlider;
	BCheckBox *EvenAspect;
	BCheckBox *LockAspect;
	BCheckBox *RectBrush;
	
	int8 brush_softness;
	float aspect;
	uint16 brush_width,brush_height;
	int8 brushtype;
};

#endif