#include <Bitmap.h>
#include <View.h>
#include <Slider.h>
#include <Rect.h>
#include <CheckBox.h>
#include <stdio.h>
#include <string.h>
#include <share.h>

#ifndef _BRUSHVIEW_H_

#define _BRUSHVIEW_H_

#define BRUSH_HEIGHT_CHANGE	'bsvh'
#define BRUSH_WIDTH_CHANGE 	'bsvw'
#define BRUSH_BLUR_CHANGE 	'bsvb'
#define BRUSH_SET_RECT		'bssr'
#define BRUSH_EVEN_ASPECT	'bsea'
#define BRUSH_LOCK_ASPECT	'bsla'

class BrushSelectorView : public BView
{	
public:
	
	BrushSelectorView(BRect r, share *sh);
	virtual ~BrushSelectorView(void);
	void AttachedToWindow(void);
	void makebrush(int16 width, int16 height);

	virtual void MessageReceived(BMessage *msg);

//	BView *BitmapView;
//	BView *BitModView;
//	BBitmap *BrushBitmap;
	BSlider	*WidthSlider;
	BSlider *HeightSlider;
	BSlider *BlurSlider;
	BCheckBox *EvenAspect;
	BCheckBox *LockAspect;
	BCheckBox *RectBrush;

	bool childrenadded;	
	share *shared;
	int8 brush_hardness;
	float aspect;
};

#endif