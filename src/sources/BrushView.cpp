#include "BrushView.h"

BrushSelectorView::BrushSelectorView(BRect r,uint16 width,uint16 height,int8 softness,int8 type) : 
	BView(r, "Brush Designer", B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE)
{
BRect work_rect;
work_rect.Set(0,0,120,120);
brush_width=width;
brush_height=height;
brush_softness=softness;
brushtype=type;

work_rect.Set(0,5,100,50);
WidthSlider=new TSlider(work_rect,"WidthSlider",SpTranslate("Width"),"%d",
			new BMessage(BRUSH_WIDTH_CHANGE),2,60);
WidthSlider->SetValue((int32)brush_width);
AddChild(WidthSlider);

work_rect.Set(100,5,200,50);
HeightSlider=new TSlider(work_rect,"HeightSlider",SpTranslate("Height"),"%d",
			new BMessage(BRUSH_HEIGHT_CHANGE),2,60);
HeightSlider->SetValue((int32)brush_height);
AddChild(HeightSlider);

work_rect.Set(50,50,150,95);
BlurSlider=new TSlider(work_rect,"Blur",SpTranslate("Softness"),"%d",
			new BMessage(BRUSH_BLUR_CHANGE),1,100);
BlurSlider->SetValue((int32)brush_softness);
AddChild(BlurSlider);

work_rect.Set(10,85,80,105);
RectBrush= new BCheckBox(work_rect, "RectBrush",SpTranslate("Rectangle"), new BMessage(BRUSH_SET_RECT));
if(brushtype==BRUSH_TYPE_RECT)
{	RectBrush->SetValue(B_CONTROL_ON);
}
AddChild(RectBrush);

//work_rect.Set(85,85,175,105);
work_rect.Set(85,85,Bounds().right,105);
LockAspect= new BCheckBox(work_rect, "LockAspect",SpTranslate("Lock Proportions"), new BMessage(BRUSH_LOCK_ASPECT));
AddChild(LockAspect);

work_rect.OffsetBy(0,20);
EvenAspect= new BCheckBox(work_rect, "EvenAspect",SpTranslate("Equal Proportions"), new BMessage(BRUSH_EVEN_ASPECT));
AddChild(EvenAspect);

}

BrushSelectorView::~BrushSelectorView(void)
{
}

void BrushSelectorView::AttachedToWindow(void)
{
	RectBrush->SetTarget(this);
	WidthSlider->SetTarget(this);
	HeightSlider->SetTarget(this);
	BlurSlider->SetTarget(this);
	LockAspect->SetTarget(this);
}

void BrushSelectorView::PostBrush(void)
{	
	BMessage *brushsender = new BMessage(SET_BRUSH);
	brushsender->AddInt8("softness",brush_softness);
	brushsender->AddInt8("type",brushtype);
	brushsender->AddInt16("width",brush_width);
	brushsender->AddInt16("height",brush_height);
	be_app->PostMessage(brushsender);
}

void BrushSelectorView::SetControls(BMessage *msg)
{
	int16 t_int16;
	int8 t_int8;
	
	if(msg->FindInt16("width",&t_int16) != B_NAME_NOT_FOUND)
		WidthSlider->SetValue((float)t_int16);
	if(msg->FindInt16("height",&t_int16) != B_NAME_NOT_FOUND)
		HeightSlider->SetValue((float)t_int16);
	if(msg->FindInt8("softness",&t_int8) != B_NAME_NOT_FOUND)
		BlurSlider->SetValue((float)t_int8);
		
	// Amazing that the conditional statement here actually compiles & works...
	if(msg->FindInt8("shape",&t_int8) != B_NAME_NOT_FOUND)
	{	(t_int8 == BRUSH_TYPE_RECT)?
			RectBrush->SetValue(B_CONTROL_ON):RectBrush->SetValue(B_CONTROL_OFF);
	}
	
	// Messages originating from Undo() calls do not send either of these,
	// but they're included in case such flexibility is needed
	if(msg->FindInt8("lockaspect",&t_int8) != B_NAME_NOT_FOUND)
	{	(t_int8 == 1)?
			LockAspect->SetValue(B_CONTROL_ON):LockAspect->SetValue(B_CONTROL_OFF);
	}
	if(msg->FindInt8("evenaspect",&t_int8) != B_NAME_NOT_FOUND)
	{	(t_int8 == 1)?
			EvenAspect->SetValue(B_CONTROL_ON):EvenAspect->SetValue(B_CONTROL_OFF);
	}
	
}

void BrushSelectorView::MessageReceived(BMessage *msg)
{	
	switch(msg->what)
	{	
		case BRUSH_LOCK_ASPECT:
			if(LockAspect->Value()==B_CONTROL_ON)
			{	
				if(EvenAspect->Value()==B_CONTROL_ON)
					EvenAspect->SetValue(B_CONTROL_OFF);
				aspect=float(WidthSlider->Value())/float(HeightSlider->Value());
			}
			break;
			
		case BRUSH_EVEN_ASPECT:
			if(LockAspect->Value()==B_CONTROL_ON)
			{	LockAspect->SetValue(B_CONTROL_OFF);
			}
			break;

		case BRUSH_SET_RECT:
			// exists for brush outline drawing
			if(RectBrush->Value()==B_CONTROL_ON)
				brushtype=BRUSH_TYPE_RECT;
			else
				brushtype=BRUSH_TYPE_ELLIPSE;
			PostBrush();
			break;
			// fall through to force update
		case BRUSH_WIDTH_CHANGE:
			if(LockAspect->Value()==B_CONTROL_ON)
			{	HeightSlider->SetValue(WidthSlider->Value()/aspect);
				brush_height=HeightSlider->Value();
			}
			else
				if(EvenAspect->Value())
				{	HeightSlider->SetValue(WidthSlider->Value());
					brush_height=HeightSlider->Value();
				}
			brush_width=WidthSlider->Value();
//			makebrush(WidthSlider->Value(), HeightSlider->Value());
			PostBrush();
			break;

		case BRUSH_HEIGHT_CHANGE:
			if(LockAspect->Value()==B_CONTROL_ON)
			{	WidthSlider->SetValue(HeightSlider->Value()*aspect);
				brush_width=WidthSlider->Value();
			}
			else
				if(EvenAspect->Value())
				{	WidthSlider->SetValue(HeightSlider->Value());
					brush_width=WidthSlider->Value();
				}
			brush_height=HeightSlider->Value();
//			makebrush(WidthSlider->Value(), HeightSlider->Value());
			PostBrush();
			break;

		case BRUSH_BLUR_CHANGE:
			brush_softness=BlurSlider->Value();
			brush_softness=BlurSlider->Value();
//			makebrush(WidthSlider->Value(), HeightSlider->Value());
			PostBrush();
			break;

		default:
			BView::MessageReceived(msg);
	}	
}
/*	Not even needed unless we decide to include yet another preview view
void BrushSelectorView::makebrush(int16 width, int16 height)
{	
	if (width < 1) 
		width=1;
	if (height<1)
		height=1;
	
	BRect rect; 
	rect.Set(0,0,width-1,height-1);
	
	BBitmap *tmp_brush = new BBitmap(rect,B_RGB32,true);
	
	tmp_brush->Lock(); //important!
	BView virtualView( tmp_brush->Bounds(), NULL, B_FOLLOW_NONE, 0 );
	tmp_brush->AddChild( &virtualView );


   	//draw the brush
   	float tmp_width = floor(width/2); //get radius
   	float tmp_height = floor(height/2);
   	int16 val = 255;
   	float centerx = tmp_width;
   	float centery = tmp_height;
   	float step;
   	if(brush_softness==0)
   		step=255;
   	else
   		step=255/brush_softness;
  
  	BPicture *my_pict; 
  	virtualView.BeginPicture(new BPicture);

	if(RectBrush->Value()==B_CONTROL_ON)
	{	// Set up the rectangle
		BRect *work_rect=new BRect(tmp_brush->Bounds());
	   	do 
		{
			// starts out white and goes to black
  			virtualView.SetHighColor(val,val,val);
   			virtualView.FillRect(*work_rect,B_SOLID_HIGH);
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

	tmp_brush->RemoveChild( &virtualView );
	tmp_brush->Unlock();
	
	brush_width=width;
	brush_height=height;
	brush_softness=brush_softness;

	PostBrush(tmp_brush);
	delete tmp_brush;	// delete because we do nothing with the actual data
}
*/