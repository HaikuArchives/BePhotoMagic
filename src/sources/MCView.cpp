#include "MCView.h"

MCView::MCView(uint32 command)
	   	   : BView(	BRect(0,0,410,280), "MultiColorView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	// Create the TabView for all of this to fit in
	tabview=new BTabView(Bounds(),"MCtabview");
	AddChild(tabview);
	
	hsv=new HSVView();
	tabview->AddTab(hsv);

	rgb= new BColorControl(BPoint(5,5),B_CELLS_32x8,8.0,"RGB",
			new BMessage(RGB_CHANGED));
	tabview->AddTab(rgb);
	rgb_color startcolor={255,0,0,255};
	hsv->SetColor(startcolor);
	msgsender=new BInvoker(new BMessage(command),be_app);
}

MCView::~MCView(void)
{	delete msgsender;
}

void MCView::AttachedToWindow(void)
{	rgb->SetTarget(this);
}
void MCView::MessageReceived(BMessage *msg)
{	
	switch(msg->what)
	{	case SPECTRUM_CHANGED:
			hsv->Update();
			currentcolor=hsv->GetColor();
			rgb->SetValue(currentcolor);
			msgsender->Invoke();
			break;
		case SQUARE_CHANGED:
			hsv->square->Draw(hsv->square->Bounds());
			hsv->UpdateTControls();
			currentcolor=hsv->GetColor();
			rgb->SetValue(currentcolor);
			msgsender->Invoke();
			break;
		case RGB_CHANGED:
			currentcolor=rgb->ValueAsColor();
			hsv->SetColor(currentcolor);
			msgsender->Invoke();
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}

void MCView::SetTarget(BHandler *tgt)
{	msgsender->SetTarget(tgt);
}

void MCView::SetColor(rgb_color color)
{	hsv->SetColor(color);
	rgb->SetValue(color);
	currentcolor=color;
}

void MCView::SetColor(hsv_color color)
{	
	hsv->SetColor(color);
	currentcolor=HSVtoRGB(color);
	rgb->SetValue(currentcolor);
}

HSVView::HSVView(void)
	   	   : BView(BRect(0,0,410,265), "HSV", B_FOLLOW_NONE, B_WILL_DRAW)
{	SetViewColor(216,216,216);
	square=new SatValView(BRect(0,0,255,255),"SVview",B_FOLLOW_NONE, B_WILL_DRAW);
	AddChild(square);
	square->SetViewColor(255,0,0);

	spectrum=new HueView(BRect(270,0,293,255),"Hview",B_FOLLOW_NONE, B_WILL_DRAW);
	AddChild(spectrum);
	square->SetViewColor(0,0,255);
	
	hue=new BTextControl(BRect(300,0,395,19),"huetc",SpTranslate("Hue"),"0",new BMessage(NUMBERS_CHANGED),B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(hue);
	sat=new BTextControl(BRect(300,30,395,49),"sattc",SpTranslate("Saturation"),"0",new BMessage(NUMBERS_CHANGED),B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(sat);
	val=new BTextControl(BRect(300,60,395,79),"valtc",SpTranslate("Value"),"0",new BMessage(NUMBERS_CHANGED),B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(val);
}

void HSVView::AttachedToWindow(void)
{	
	hue->SetTarget(this);
	sat->SetTarget(this);
	val->SetTarget(this);
}

void HSVView::SetColor(hsv_color hsvcol)
{	
	// First, set the views to the proper colors
	spectrum->value=(uint16)spectrum->HueToPoint(hsvcol.hue).y;
	spectrum->Draw(spectrum->Bounds());
	square->cursor=square->ColorToPoint(hsvcol);
	Update();
}

void HSVView::SetColor(rgb_color rgbcol)
{	hsv_color temp=RGBtoHSV(rgbcol);
	SetColor(temp);
}

rgb_color HSVView::GetColor(void)
{	return square->PointToColor(square->cursor);
}

void HSVView::Update(void)
{	hsv_color temphsv;
	rgb_color temprgb;
	
	// First, get the hue
	temphsv.hue=spectrum->PointToHue(BPoint(0,spectrum->value));

	// Convert to RGB
	temphsv.saturation=255;
	temphsv.value=255;
	temprgb=HSVtoRGB(temphsv);

	// Now, draw the new panel
	square->CreateSquare(temprgb);
	UpdateTControls();
	square->Draw(square->Bounds());
}

void HSVView::UpdateTControls(void)
{	char tempstr[255];
	hsv_color color=RGBtoHSV(square->PointToColor(square->cursor));

	sprintf(tempstr,"%u",color.hue);
	hue->SetText(tempstr);

	sprintf(tempstr,"%u",color.saturation);
	sat->SetText(tempstr);

	sprintf(tempstr,"%u",color.value);
	val->SetText(tempstr);
}

void HSVView::MessageReceived(BMessage *msg)
{	
	switch(msg->what)
	{	case NUMBERS_CHANGED:
			SetColor(GetTextControlsColor());
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}

hsv_color HSVView::GetTextControlsColor(void)
{	
	hsv_color hsv;

	hsv.hue=(uint16)CheckIntegralValue(hue, 0L, 360L);
	hsv.saturation=(uint16)CheckIntegralValue(sat, 0L, 255L);
	hsv.value=(uint16)CheckIntegralValue(val, 0L, 255L);
	return hsv;
}

HueView::HueView(BRect frame, const char *name, int32 resize, int32 flags)
	   	   : BView(frame, name, resize, flags)
{
	value=0;
	leftx=0;
	rightx=Bounds().IntegerWidth();

	// Gotta do this the hard way - B_HSV32 unimplemented. DAMN.
	display=new BBitmap(Bounds(),B_RGB32,true);
	if(display != NULL)
		CreateSpectrum();	// shameless ripoff of Photon code
}

HueView::~HueView(void)
{
	delete display;
	delete msgsender;
}

void HueView::AttachedToWindow(void)
{
	BHandler *handler=(BHandler *)Parent();
	if(handler==NULL)
	{	// not attached to a view. Try a Window
		handler=(BHandler *)Window();
	}
	// if we couldn't get either, we deserve to crash (lazy developer...)
	msgsender= new BInvoker(new BMessage(SPECTRUM_CHANGED), handler);
}

void HueView::MouseUp(BPoint where)
{	
	value=(uint8)where.y;
	Draw(Bounds());
	msgsender->Invoke();
}

void HueView::MouseDown(BPoint where)
{	
	value=(uint8)where.y;
	Draw(Bounds());
	msgsender->Invoke();
}

void HueView::MouseMoved(BPoint where, uint32 transit, const BMessage *message)
{	SetMouseEventMask(0,B_NO_POINTER_HISTORY);
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt,&buttons);
	if( (buttons == B_SECONDARY_MOUSE_BUTTON) || (buttons == B_PRIMARY_MOUSE_BUTTON))
	{	value=(uint8)where.y;
		Draw(Bounds());
		msgsender->Invoke();
	}
}

void HueView::Draw(BRect update_rect)
{
	SetDrawingMode(B_OP_COPY);
	DrawBitmap(display,update_rect,update_rect);

	SetHighColor(255,255,255);
	SetDrawingMode(B_OP_INVERT);
	StrokeLine( BPoint(leftx,value),BPoint(rightx,value));
}

int16 HueView::PointToHue(BPoint pt)
{	
	float 	yval=(uint16)pt.y,
			max=Bounds().Height(),
			hue;

	if(yval > max)
	{	yval=max;
	}

	hue=((yval/max)*360);

	return (uint16)hue;
}

BPoint HueView::HueToPoint(uint16 hue)
{	// Returns a BPoint in the form (0,value) which is the closest hue match
	// to the given color
	float yval;

	if(hue>360)
		yval=255;
	else
		yval=255*(float(hue)/360);

	return(BPoint(0,yval));
}

void HueView::CreateSpectrum(void)
{
	// Based on the old Photon code, but this version uses StrokeLine() instead of
	// individually setting points. The speed gain is worth the BView overhead
	int32 y=0;
	int16 r,g,b;

	display->Lock();
	BView *view=new BView(display->Bounds(),"temp",B_FOLLOW_NONE,B_WILL_DRAW);
	display->AddChild(view);
	

	// Red to Yellow
	r=255;g=0;b=0;
	while (g<255)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		g+=6;
	}
	
	// Yellow to Green
	r=255;g=255;b=0;
	while (r>0)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		r-=6;
	}
	
	// Green to Cyan
	r=0;g=255;b=0;
	while (b<255)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		b+=6;
	}
	
	// Cyan to Blue
	r=0;g=255;b=255;
	while (g>0)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		g-=6;
	}
	
	// Blue to Magenta
	r=0;g=0;b=255;
	while (r<255)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		r+=6;
	}
	
	// Magenta to Red
	r=255;g=0;b=255;
	while (b>0)
	{
		view->SetHighColor(r,g,b,255);
		view->StrokeLine(BPoint(0,y),BPoint(24,y));
		y++;
		b-=6;
	}
	
	display->RemoveChild(view);
	display->Unlock();
	delete view;
}

SatValView::SatValView(BRect frame, const char *name, int32 resize, int32 flags)
	   	   : BView(frame, name, resize, flags)
{
	cursor.Set(0,0);
	
	// Gotta do this the hard way - B_HSV32 unimplemented. DAMN.
	display=new BBitmap(Bounds(),B_RGB32,true);

	target=Window();
	rgb_color col;
	CreateSquare(col);		// shameless ripoff of Photon code
}

SatValView::~SatValView(void)
{
	delete display;
	delete msgsender;
}

void SatValView::AttachedToWindow(void)
{
	BHandler *handler=(BHandler *)Parent();
	if(handler==NULL)
	{	// not attached to a view. Try a Window
		handler=(BHandler *)Window();
	}
	// if we couldn't get either, we deserve to crash (lazy developer...)
	msgsender= new BInvoker(new BMessage(SQUARE_CHANGED), handler);
}

void SatValView::MouseUp(BPoint where)
{	
	cursor=where;
	Draw(Bounds());
	msgsender->Invoke();
}

void SatValView::MouseDown(BPoint where)
{	
	cursor=where;
	Draw(Bounds());
	msgsender->Invoke();
}

void SatValView::MouseMoved(BPoint where, uint32 transit, const BMessage *message)
{	
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt,&buttons);
	if( (buttons == B_SECONDARY_MOUSE_BUTTON) || (buttons == B_PRIMARY_MOUSE_BUTTON))
	{	cursor=where;
		Draw(Bounds());
		msgsender->Invoke();
	}
}

void SatValView::Draw(BRect update_rect)
{
	SetDrawingMode(B_OP_COPY);
	DrawBitmap(display,update_rect,update_rect);
	SetDrawingMode(B_OP_INVERT);
	SetHighColor(255,255,255);
	StrokeLine(BPoint(cursor.x-1,cursor.y),BPoint(cursor.x+1,cursor.y));
	StrokeLine(BPoint(cursor.x,cursor.y-1),BPoint(cursor.x,cursor.y+1));
	SetDrawingMode(B_OP_COPY);
	StrokeLine(cursor,cursor);
}

rgb_color SatValView::PointToColor(BPoint pt)
{
	float 	x=pt.x,
			y=pt.y;

	// Bounds check
	if(x<0)	x=0;
	if(x>Bounds().IntegerWidth())	x=Bounds().IntegerWidth();
	if(y<0)	y=0;
	if(y>Bounds().IntegerHeight())	y=Bounds().IntegerHeight();

	rgb_color color;

	uint32 offset=uint32 ((display->BytesPerRow()*y) + (x*4));
	uint8	*bmpdata=(uint8*) display->Bits() + offset;
	
	color.blue=*bmpdata;
	bmpdata++;
	color.green=*bmpdata;
	bmpdata++;
	color.red=*bmpdata;
	bmpdata++;
	color.alpha=*bmpdata;

	return color;
}

BPoint SatValView::ColorToPoint(rgb_color color)
{	// this will return the approximate coordinates
	float sat,val;
	hsv_color hc=RGBtoHSV(color);

	sat=float(hc.saturation);
	val=float(hc.value);
	val = 255-val;

	return BPoint(sat,val);
}

BPoint SatValView::ColorToPoint(hsv_color color)
{	// this will return the approximate coordinates
	float sat,val;

	sat=color.saturation;
	val=color.value;
	
	val = 255-val;
	return BPoint(sat,val);
}

void SatValView::CreateSquare(rgb_color col)
{
	int8 color_selector_size=1;
	
	uint8 *bits,*temp;
	bits= (uint8*)display->Bits();
	
	int32 pos=0;
	int32 line_ct=0;
	
	float a=0;
	float r=0;
	float g=0;
	float b=0;
	
	rgb_color base_col = col;
	uint8 maxi=255;
	
	//base values for comparison
	int16 r_base,g_base,b_base;
	
	r_base = base_col.red;
	g_base = base_col.green;
	b_base = base_col.blue;
	
	while (line_ct!=(256/color_selector_size))
	{
		r = maxi;
		g = maxi;
		b = maxi;		
	
		pos=(255/color_selector_size);
		while (pos!=-1)
		{
			if (r > r_base) 	r -= 1/(pos/(r-r_base)); 
			if (g > g_base)		g -= 1/(pos/(g-g_base)); 
			if (b > b_base) 	b -= 1/(pos/(b-b_base)); 
	
			//copy over RGBA quad
			temp=bits;  *temp = (uint8) b;
			temp++;		*temp = (uint8) g;
			temp++;		*temp = (uint8) r;
			temp++;		*temp = (uint8) a;
	
			bits+=4; pos--;
			
		}
		line_ct++;
		maxi-=1*color_selector_size;
	
		if (r_base > maxi) 	r_base --; 
		if (g_base > maxi)	g_base --; 
		if (b_base > maxi) 	b_base --; 
	}
}
