#include "BrushView.h"
#include "Utils.h"
#include "defs.h"

BrushSelectorView::BrushSelectorView(BRect r,share *sh) : 
	BView(r, "Brush Selector", B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED)
{
BRect work_rect;
work_rect.Set(0,0,120,120);

//Get access to shared class
shared=sh;

// View to hold & display bitmap
BitmapView=new BView(BRect(0.0, 0.0, 120.0, 120.0),"BitmapView",B_FOLLOW_ALL,B_WILL_DRAW);

// Bitmap which holds the actual bitmap data
//BrushBitmap= new BBitmap(BRect(0.0, 0.0, 120.0, 120.0), B_RGB32,true); 
BrushBitmap= new BBitmap(BRect(0.0, 0.0, 120.0, 120.0), B_RGB32,true); 

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
	BitModView->SetHighColor(255,255,255);
	BitModView->FillRect(BitModView->Bounds());
	BitModView->SetHighColor(0,0,0);
	BitModView->FillEllipse( BPoint(60,60),	WidthSlider->Value(),
					HeightSlider->Value(), B_SOLID_HIGH);
	Blur();
	BitModView->Sync();
	DrawBitmap(BrushBitmap,BitModView->Bounds());
	BrushBitmap->Unlock();
	shared->UseAsBrush(BrushBitmap);

	DrawBitmap(BrushBitmap);
}

void BrushSelectorView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		case BRUSH_HEIGHT_CHANGE:
//			BitModView->Invalidate();
//			shared->UseAsBrush(BrushBitmap);
//			break;
		default:
			BView::MessageReceived(msg);
	}	
}

void BrushSelectorView::Pulse(void)
{	Draw(Bounds());
	util.mainWin->PostMessage(new BMessage(UPDATE_TITLE));
	util.mainWin->PostMessage(new BMessage(TOOL_CHANGED));
	
}	

void BrushSelectorView::Blur(void)
{	// this will likely be used elsewhere as well.
	// Blurs a bitmap based on a radius. Gaussian? I don't know

	// Init such that it will be easy to adapt elsewhere
	uint8 blurfactor=BlurSlider->Value();
	uint8 *bitdata;
	uint8 red=0, green=0, blue=0;
	uint32 pos, end=BrushBitmap->BitsLength();
	
	bitdata=(uint8 *) BrushBitmap->Bits();

	if(blurfactor)
	{
			
	
	
	}
}