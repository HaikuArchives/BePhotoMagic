#include <Bitmap.h>
#include <View.h>
#include <Slider.h>
#include <Rect.h>
#include <stdio.h>
#include <string.h>
#include <share.h>

#ifndef _BRUSHVIEW_H_

#define _BRUSHVIEW_H_

#define BRUSH_HEIGHT_CHANGE	'bsvh'
#define BRUSH_WIDTH_CHANGE 	'bsvw'
#define BRUSH_BLUR_CHANGE 	'bsvb'

class BrushSelectorView : public BView
{	
public:
	
	BrushSelectorView(BRect r, share *sh);
	virtual ~BrushSelectorView(void);
	
	void Draw(BRect rect);
	virtual void MessageReceived(BMessage *msg);
	virtual void Pulse();
	void Blur(void);	
	BView *BitmapView;
	BView *BitModView;
	BBitmap *BrushBitmap;
	BSlider	*WidthSlider;
	BSlider *HeightSlider;
	BSlider *BlurSlider;
	
	share *shared;
};

#endif