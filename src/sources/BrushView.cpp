#include "BrushView.h"
#include "Utils.h"
#include "defs.h"

BrushSelectorView::BrushSelectorView(BRect r,share *sh) : 
	BView(r, "Brush Selector", B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE)
{
BRect work_rect;
work_rect.Set(0,0,120,120);

//Get access to shared class
shared=sh;

// View to hold & display bitmap
//BitmapView=new BView(BRect(0.0, 0.0, 120.0, 120.0),"BitmapView",B_FOLLOW_ALL,B_WILL_DRAW);
//BitmapView->SetHighColor(255,255,255);
//BrushBitmap= new BBitmap(BRect(0,0,110,110),B_RGB32,true);
//BitModView=new BView(BrushBitmap->Bounds(),"bitmodview",B_FOLLOW_ALL,B_WILL_DRAW);
//BrushBitmap->AddChild(BitModView);

work_rect.Set(0,5,100,50);
WidthSlider=new BSlider(work_rect,"WidthSlider","Width",
			new BMessage(BRUSH_WIDTH_CHANGE),2,60);

work_rect.Set(100,5,200,50);
HeightSlider=new BSlider(work_rect,"HeightSlider","Height",
			new BMessage(BRUSH_HEIGHT_CHANGE),2,60);

work_rect.Set(50,50,150,95);
BlurSlider=new BSlider(work_rect,"Blur","Softness",
			new BMessage(BRUSH_BLUR_CHANGE),1,100);
WidthSlider->SetValue(shared->current_brush);
HeightSlider->SetValue(32);
BlurSlider->SetValue(0);

work_rect.Set(10,95,80,115);

RectBrush= new BCheckBox(work_rect, "RectBrush", "Rectangle", new BMessage(BRUSH_SET_RECT));

work_rect.Set(85,95,655,115);
LockAspect= new BCheckBox(work_rect, "LockAspect", "Lock Aspect", new BMessage(BRUSH_LOCK_ASPECT));
LockAspect->SetEnabled(false);

work_rect.OffsetBy(0,20);
EvenAspect= new BCheckBox(work_rect, "EvenAspect", "Even Aspect", new BMessage(BRUSH_EVEN_ASPECT));

childrenadded=false;
}

BrushSelectorView::~BrushSelectorView(void)
{
//BrushBitmap->RemoveChild(BitModView);
//delete BrushBitmap;
}

void BrushSelectorView::AttachedToWindow(void)
{
if(!childrenadded)
{
	AddChild(WidthSlider);
	AddChild(HeightSlider);
	AddChild(BlurSlider);
	AddChild(RectBrush);
	RectBrush->SetTarget(this);
	AddChild(LockAspect);
	AddChild(EvenAspect);
//	AddChild(BitmapView);

	WidthSlider->SetTarget(this);
	HeightSlider->SetTarget(this);
	BlurSlider->SetTarget(this);
	childrenadded=true;
}
}

void BrushSelectorView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		case BRUSH_LOCK_ASPECT:
			break;
			
		case BRUSH_EVEN_ASPECT:
			if(LockAspect->Value()==B_CONTROL_ON)
			{	LockAspect->SetValue(B_CONTROL_OFF);
			}
			break;
		case BRUSH_SET_RECT:
			// exists for brush outline drawing
			if(RectBrush->Value()==B_CONTROL_ON)
				shared->brushtype=BRUSH_TYPE_RECT;
			else
				shared->brushtype=BRUSH_TYPE_ELLIPSE;
			
		case BRUSH_WIDTH_CHANGE:
/*			if(LockAspect->Value())
			{	
			}
*/			if(EvenAspect->Value())
			{	HeightSlider->SetValue(WidthSlider->Value());
			}
			makebrush(WidthSlider->Value(), HeightSlider->Value());
			util.mainWin->PostMessage(new BMessage(UPDATE_TITLE));
			util.mainWin->PostMessage(new BMessage(TOOL_CHANGED));
			break;

		case BRUSH_HEIGHT_CHANGE:
/*			if(LockAspect->Value())
			{	
			}
*/			if(EvenAspect->Value())
			{	WidthSlider->SetValue(HeightSlider->Value());
			}
			makebrush(WidthSlider->Value(), HeightSlider->Value());
			util.mainWin->PostMessage(new BMessage(UPDATE_TITLE));
			util.mainWin->PostMessage(new BMessage(TOOL_CHANGED));
			break;

		case BRUSH_BLUR_CHANGE:
			brush_hardness=BlurSlider->Value();
			makebrush(WidthSlider->Value(), HeightSlider->Value());
			util.mainWin->PostMessage(new BMessage(UPDATE_TITLE));
			util.mainWin->PostMessage(new BMessage(TOOL_CHANGED));
			break;

		default:
			BView::MessageReceived(msg);
	}	
}

void BrushSelectorView::makebrush(int16 width, int16 height)
{	
	if (width < 1) 
		width=1;
	if (height<1)
		height=1;
	
	BRect rect; 
//	rect = BitModView->Bounds();
//   	float BMcenter= rect.IntegerWidth()/2;

	rect.Set(0,0,width-1,height-1);
	
	BBitmap *tmp_brush = new BBitmap(rect,B_RGB32,true);

	tmp_brush->Lock(); //important!
//	BrushBitmap->Lock();
//	BitModView->LockLooper();
	
	BView virtualView( tmp_brush->Bounds(), NULL, B_FOLLOW_NONE, 0 );
	tmp_brush->AddChild( &virtualView );


   	//draw the brush
   	float tmp_width = floor(width/2); //get radius
   	float tmp_height = floor(height/2);
   	int16 val = 255;
   	float centerx = tmp_width;
   	float centery = tmp_height;
   	float step;
   	if(brush_hardness==0)
   		step=255;
   	else
   		step=255/brush_hardness;
  
  	BPicture *my_pict; 
  	virtualView.BeginPicture(new BPicture);

//  	BitModView->SetHighColor(255,255,255);
//	BitModView->FillRect(BitModView->Bounds());

	if(RectBrush->Value()==B_CONTROL_ON)
	{	// Set up the rectangle
		BRect *work_rect=new BRect(tmp_brush->Bounds());
//		work_rect.Set(centerx-(tmp_width/2),centery-(tmp_height/2),
//						centerx+(tmp_width/2),centery+(tmp_height/2));
	   	do 
		{
			// starts out white and goes to black
  			virtualView.SetHighColor(val,val,val);
   			virtualView.FillRect(*work_rect,B_SOLID_HIGH);
//  		BitModView->SetHighColor(val,val,val);
//   		BitModView->FillRect(work_rect,B_SOLID_HIGH);
	   		val -= (int16) step;
   			if (val < 0) 
   				val=0;

			// kept around just for the loop
	   		if(tmp_width>-1)
		   		tmp_width--;
   			if(tmp_height>-1)
  				tmp_height--;

			work_rect->InsetBy(1,1);   			
		} while ( (tmp_width != -1) && (tmp_height != -1) );

		delete work_rect;
	}
	else
	{
	   	do 
		{
			// starts out white and goes to black
  			virtualView.SetHighColor(val,val,val);
   			virtualView.FillEllipse(BPoint(centerx,centery),tmp_width,tmp_height,B_SOLID_HIGH);
//  		BitModView->SetHighColor(val,val,val);
//   		BitModView->FillEllipse(BPoint(55,55),tmp_width,tmp_height,B_SOLID_HIGH);
	   		val -= (int16) step;
   			if (val < 0) 
   				val=0;
	   		if(tmp_width>-1)
		   		tmp_width--;
   			if(tmp_height>-1)
  				tmp_height--;
   			
		} while ( (tmp_width != -1) && (tmp_height != -1) );
	}

   	my_pict = virtualView.EndPicture();
   	virtualView.DrawPicture(my_pict);

	// Draw the bitmap centered in the view
/*	BitmapView->BeginPicture(new BPicture);
  	BitmapView->SetHighColor(255,255,255);
	BitmapView->FillRect(BitmapView->Bounds() );
	BitmapView->DrawBitmap(BrushBitmap);
	my_pict = BitmapView->EndPicture();
   	BitmapView->DrawPicture(my_pict);
	
   	BrushBitmap->Unlock();
   	BitModView->UnlockLooper();
*/
 	tmp_brush->RemoveChild( &virtualView );
   	tmp_brush->Unlock();
   	
   	shared->current_brush=width;
	shared->UseAsBrush(tmp_brush);
}