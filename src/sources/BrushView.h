#include <Bitmap.h>
#include <View.h>
#include <Slider.h>
#include <Rect.h>
#include <iostream.h>

#ifndef _BRUSHVIEW_H_

#define _BRUSHVIEW_H_

#define BRUSH_HEIGHT_CHANGE	'bsvh'
#define BRUSH_WIDTH_CHANGE 	'bsvw'
#define BRUSH_BLUR_CHANGE 	'bsvb'

class BrushSelectorView : public BView
{	
public:
	
	BrushSelectorView(BRect r);
	virtual ~BrushSelectorView(void);
	
	// For antialiasing the brush
	void Blur(BBitmap *bitmap);
	void Draw(BRect rect);
	virtual void MessageReceived(BMessage *msg);
	
	BView *BitmapView;
	BView *BitModView;
	BBitmap *BrushBitmap;
	BSlider	*WidthSlider;
	BSlider *HeightSlider;
	BSlider *BlurSlider;

};

#endif