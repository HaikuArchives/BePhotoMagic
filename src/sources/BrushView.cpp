#include "BrushView.h"

BrushSelectorView::BrushSelectorView(BRect r) : 
	BView(r, "Brush Selector", B_FOLLOW_ALL, B_WILL_DRAW)
{
BRect work_rect;
work_rect.Set(0,0,120,120);

// View to hold & display bitmap
BitmapView=new BView(BRect(0.0, 0.0, 120.0, 120.0),"BitmapView",B_FOLLOW_ALL,B_WILL_DRAW);

// Bitmap which holds the actual bitmap data
BrushBitmap= new BBitmap(BRect(0.0, 0.0, 120.0, 120.0), B_CMAP8,true); 

// Bitmap child view - makes for easy modifications to brush
BitModView=new BView(BRect(0.0, 0.0, 120.0, 120.0),"BitmapView",B_FOLLOW_ALL,B_WILL_DRAW);
BrushBitmap->AddChild(BitModView);
BitModView->Show();

work_rect.Set(0,0,100,45);
work_rect.OffsetBy(0,125);

WidthSlider=new BSlider(work_rect,"WidthSlider","Width",
			new BMessage(BRUSH_WIDTH_CHANGE),1,60);
AddChild(WidthSlider);

work_rect.OffsetBy(100,0);
HeightSlider=new BSlider(work_rect,"HeightSlider","Height",
			new BMessage(BRUSH_HEIGHT_CHANGE),1,60);
AddChild(HeightSlider);

work_rect.OffsetBy(-50,35);
BlurSlider=new BSlider(work_rect,"Blur","Softness",
			new BMessage(BRUSH_BLUR_CHANGE),1,100);
AddChild(BlurSlider);
}

BrushSelectorView::~BrushSelectorView(void)
{
delete BrushBitmap;
}

void BrushSelectorView::Draw(BRect rect)
{	
	BrushBitmap->Lock();
	SetHighColor(0,0,0);
	BitModView->FillEllipse(	BPoint(60,60),
					WidthSlider->Value(),
					HeightSlider->Value(),
					B_SOLID_HIGH
				);
//	BitModView->Sync();
	DrawBitmap(BrushBitmap,BitModView->Bounds());
}

void BrushSelectorView::MessageReceived(BMessage *msg)
{	
	switch(msg->what)
	{	case BRUSH_HEIGHT_CHANGE:
		case BRUSH_WIDTH_CHANGE:
		case BRUSH_BLUR_CHANGE:
			BitModView->Invalidate();
		default:
			BView::MessageReceived(msg);
	}	
}