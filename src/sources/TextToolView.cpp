#include "TextToolView.h"

FontListView::FontListView(BRect rect, const char *name, BHandler *target, BMessage *message) :
	BListView(rect, name)
{
	fontinvoker=new BInvoker(message,target);
}

FontListView::~FontListView(void)
{	delete fontinvoker;	
}

void FontListView::SelectionChanged()
{
	fontinvoker->Invoke();
}

TextToolView::TextToolView(BRect rect) : 
	BView (rect, "containerview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED)
{
	BRect srect;
	const int8 height=35;

	SetViewColor(208,208,208);

	// Create all controls for the view
	textedit = new BTextControl( BRect(5,15,95,30),"textedit",NULL,"BePhotoMagic",
				new BMessage(TEXT_CHANGED) );
	

	fontlist = new FontListView(BRect(5,55,95,150),"fontlist",
					this, new BMessage(FONT_FAMILY_CHANGED));
	stylelist = new BListView(BRect(5,185,110,240),"stylelist");
	
	fontscrollview = new BScrollView("fontscrollview", fontlist, B_FOLLOW_LEFT | B_FOLLOW_TOP,
					0,true,true);
	fontscrollview->SetViewColor(208,208,208);
	BScrollBar *bar=fontscrollview->ScrollBar(B_HORIZONTAL);
	bar->SetRange(0,75);

//	srect.Set(5,190,100,190+height);
	srect.Set(5,245,100,245+height);

	sizeslider = new BSlider(srect, "sizeslider", "Size", 
				new BMessage(SIZE_CHANGED), 4, 360, B_TRIANGLE_THUMB);
	sizeslider->SetBarThickness(3);
	sizeslider->SetValue(12);
	sizeslider->SetLabel("Size: 12");
	
	srect.OffsetBy(0,height);

	shearslider = new BSlider(srect, "shearslider", "Shear", 
				new BMessage(SHEAR_CHANGED), 45, 135, B_TRIANGLE_THUMB);
	shearslider->SetBarThickness(3);
	shearslider->SetValue(90);
	shearslider->SetLabel("Shear: 90");
	
	srect.OffsetBy(0,height);

	rotationslider = new BSlider(srect, "rotationslider", "Rotation", 
				new BMessage(ROTATION_CHANGED), 0, 359, B_TRIANGLE_THUMB);
	rotationslider->SetBarThickness(3);
	rotationslider->SetLabel("Rotation: 0");

	srect.OffsetBy(0,height);
	
	aacheckbox = new BCheckBox(srect,"aacheckbox","Antialias Text",
				new BMessage(ANTIALIAS_TEXT));
	// anti-aliased text by default
	aacheckbox->SetValue(B_CONTROL_ON);
	

	srect.bottom=srect.top + floor(srect.IntegerHeight() * .75);
	srect.OffsetBy(0,height-10);
	
	renderbutton = new BButton( srect,"renderbutton",SpTranslate("Create"), new BMessage(RENDER_TEXT));
	
	
	// Set up button which toggles the fontview being shown
	srect.OffsetBy(0,height);

	fontviewtoggle = new BButton( srect,"fontviewtoggle",SpTranslate("Hide Text"),
				 new BMessage(FONTVIEW_TOGGLE));
	

	// Display view
	srect=Bounds();
	srect.left=120;
	fontview = new FontDrawView(srect, (char *)textedit->Text());
	
	// Begin font setup
	int32 i,numFamilies;
	uint32 flags;
	font_family family;

	numFamilies = count_font_families();

	// Get font families installed
	for (i = 0; i < numFamilies; i++ ) 
	{
		if ( get_font_family(i, &family, &flags) == B_OK ) 
			fontlist->AddItem(new BStringItem((char *)&family));
	}
	fontlist->SetInvocationMessage(new BMessage(FONT_FAMILY_CHANGED));
	stylelist->SetInvocationMessage(new BMessage(FONT_STYLE_CHANGED));
	
	fontview->currentfont= be_plain_font;
	fontview->currentfont.SetSize((float)sizeslider->Value());
	fontview->SetFont(&(fontview->currentfont));

	Show();
	SetHighColor(0,0,0);
}

void TextToolView::Draw(BRect rect)
{
	DrawString(SpTranslate("Text:"),BPoint(5,10));
	DrawString(SpTranslate("Font:"),BPoint(5,50));
	DrawString(SpTranslate("Style:"),BPoint(5,180));
}

void TextToolView::AttachedToWindow(void)
{
AddChild(textedit);
AddChild(fontscrollview);
AddChild(stylelist);
AddChild(sizeslider);
AddChild(shearslider);
AddChild(rotationslider);
AddChild(aacheckbox);
AddChild(renderbutton);
AddChild(fontviewtoggle);

AddChild(fontview);
fontviewtoggle->SetTarget(fontview);
textedit->SetTarget(this);
sizeslider->SetTarget(this);
shearslider->SetTarget(this);
rotationslider->SetTarget(this);
aacheckbox->SetTarget(this);
renderbutton->SetTarget(Window());
fontlist->SetTarget(this);
stylelist->SetTarget(this);
}

TextToolView::~TextToolView(void)
{
}

void TextToolView::MessageReceived(BMessage *message)
{	char string1[255];
	font_family family;
	font_style style;
	int32 num_styles,i;
	uint32 flags;
	BStringItem *selected;

	switch(message->what)
	{	case SHEAR_CHANGED:
			fontview->currentfont.SetShear((float)shearslider->Value());
			sprintf(string1,"Shear: %ld",shearslider->Value());
			shearslider->SetLabel(string1);
			fontview->SetFont(&(fontview->currentfont), B_FONT_SHEAR);
			if(!(shearslider->IsFocus()))
				shearslider->MakeFocus();
			fontview->Invalidate();
			break;
		case ROTATION_CHANGED:
			fontview->currentfont.SetRotation((float)rotationslider->Value());
			sprintf(string1,"Rotation: %ld",rotationslider->Value());
			rotationslider->SetLabel(string1);
			fontview->SetFont(&(fontview->currentfont), B_FONT_ROTATION);
			if(!(rotationslider->IsFocus()))
				rotationslider->MakeFocus();
			fontview->Invalidate();
			break;

		case SIZE_CHANGED:
			fontview->currentfont.SetSize((float)sizeslider->Value());
			sprintf(string1,"Size: %ld",sizeslider->Value());
			sizeslider->SetLabel(string1);
			fontview->SetFont(&(fontview->currentfont), B_FONT_SIZE);
			if(!(sizeslider->IsFocus()))
				sizeslider->MakeFocus();
			fontview->Invalidate();
			break;

		case TEXT_CHANGED:
			sprintf(fontview->text,textedit->Text());
			fontview->Invalidate();
			break;

		case FONT_FAMILY_CHANGED:
			//Delete all styles for the old family
			stylelist->RemoveItems(0,stylelist->CountItems());
			
			// Determine current family
			selected=(BStringItem *)fontlist->ItemAt(fontlist->CurrentSelection());
			
			// We can afford to do this only because user can't edit
			// the font names
			sprintf((char *)&family,selected->Text());

			//Repopulate with all styles for new family
			num_styles = count_font_styles(family);

			for (i=0; i < num_styles; i++ ) 
			{
				if ( get_font_style(family, i, &style, &flags) == B_OK ) 
					stylelist->AddItem(new BStringItem((char *)&style));
			}
			
			stylelist->Select(0);
			
//			break;

		case FONT_STYLE_CHANGED:
			// Determine current family
			selected=(BStringItem *)fontlist->ItemAt(fontlist->CurrentSelection());
			sprintf((char *)&family,selected->Text());
	
			// Determine current style
			selected=(BStringItem *)stylelist->ItemAt(stylelist->CurrentSelection());
			sprintf((char *)&style,selected->Text());

			// Set text to new font
			fontview->currentfont.SetFamilyAndStyle(family,style);
			fontview->SetFont(&(fontview->currentfont));
			fontview->Invalidate();
			break;
			
		case ANTIALIAS_TEXT:
			if(aacheckbox->Value()==B_CONTROL_ON)
				fontview->currentfont.SetFlags(B_FORCE_ANTIALIASING);
			else
				fontview->currentfont.SetFlags(B_DISABLE_ANTIALIASING);

			fontview->SetFont(&(fontview->currentfont));
			fontview->Invalidate();
			break;

		default:
			BView::MessageReceived(message);
	}
}

FontDrawView::FontDrawView(BRect rect, char *fonttext) :
		BView (rect, "fontdrawview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_NAVIGABLE)
{	BRect temp_rect;
	temp_rect=Bounds();
	sprintf(text,fonttext);
	fontlocation.Set(temp_rect.IntegerWidth()/4,temp_rect.IntegerHeight()/2);
}

FontDrawView::~FontDrawView(void)
{
}

void FontDrawView::Draw(BRect r)
{
	if (LockLooper() )
  	{
		FillRect(Bounds(),B_SOLID_LOW);
  		CheckStringPlacement();
		DrawString(text,fontlocation);
		UnlockLooper();
 	}
}

void FontDrawView::CheckStringPlacement(void)
{
	GetFont(&currentfont);

	// Construct a bounds rectangle because BoundingBox() doesn't work
	float x,y,r;			// temp vars for calculating rotated values
	int16 rotation=(int16)currentfont.Rotation(),angle;

	// figure out the height of the text
	struct font_height currentfont_height;
	currentfont.GetHeight(&currentfont_height);
	ascent=currentfont_height.ascent;
	descent=currentfont_height.descent;

	// Calculate lower right point offsets
	// x already set from last point
	x=-2;
	y=currentfont_height.descent;
	r=sqrt( (x*x)+(y*y) );
	angle=(int16)InvSine(y/r);

	if(angle+rotation > 359)
		angle -= 360;
	ll.Set(fontlocation.x-(r*(costable[rotation+angle])),fontlocation.y+(r*(sintable[rotation+angle])) );

	// Calculate upper left point offsets. Lower left is considered the origin
	x=0;
	y=currentfont_height.ascent + currentfont_height.descent;
	
	if(rotation+90 > 359)
		rotation -= 360;
		
	ul.Set(ll.x+(y*(costable[rotation+90])),ll.y-(y*(sintable[rotation+90])));
	
	// Calculate lower right point offsets
	rotation=(int16)currentfont.Rotation();

	x=currentfont.StringWidth(text)+5;
	y=0;

	lr.Set(ll.x+(x*(costable[rotation])),ll.y-(x*(sintable[rotation])) );

	// Calculate lower right point offsets
	// x already set from last point
	y=currentfont_height.ascent + currentfont_height.descent;
	r=sqrt( (x*x)+(y*y) );
	angle=(int16)InvSine(y/r);

	if(angle+rotation > 359)
		angle -= 360;
	ur.Set(ll.x+(r*(costable[rotation+angle])),ll.y-(r*(sintable[rotation+angle])) );

	// Figure out the extents of a BRect which would contain the font's boundaries
	float temp1,temp2;
	temp1=MIN(ul.x,ll.x);
	temp2=MIN(ur.x,lr.x);
	bbox.left=MIN(temp1,temp2);
	
	temp1=MIN(ul.y,ll.y);
	temp2=MIN(ur.y,lr.y);
	bbox.top=MIN(temp1,temp2);

	temp1=MAX(ul.x,ll.x);
	temp2=MAX(ur.x,lr.x);
	bbox.right=MAX(temp1,temp2);

	temp1=MAX(ul.y,ll.y);
	temp2=MAX(ur.y,lr.y);
	bbox.bottom=MAX(temp1,temp2);
}

void FontDrawView::MessageReceived(BMessage *message)
{	BView *parentview;
	BWindow *parentwin;
	switch(message->what)
	{
		case FONTVIEW_TOGGLE:
			parentview=Parent();
			parentwin=parentview->Window();
			if(IsHidden())
			{	
				parentwin->PostMessage(FONTVIEW_TOGGLE);
				Show();
			}
			else
			{	
				parentwin->PostMessage(FONTVIEW_TOGGLE);
				Hide();
			}
			break;	

		default:
			BView::MessageReceived(message);
	}
}

void FontDrawView::MouseDown(BPoint where)
{
	BPoint pt;
	uint32 buttons;

	GetMouse(&pt,&buttons);
	if(buttons == B_PRIMARY_MOUSE_BUTTON)
		oldpoint=where;

	if(buttons == B_SECONDARY_MOUSE_BUTTON)
	{	fontlocation=where;
		Draw(Bounds());
	}
}

void FontDrawView::MouseMoved(BPoint where,uint32 trans,const BMessage *drag)
{
	BPoint pt;
	uint32 buttons;

	GetMouse(&pt,&buttons);
	if(buttons == B_PRIMARY_MOUSE_BUTTON)
	{	fontlocation.x += (where.x-oldpoint.x);
		fontlocation.y += (where.y-oldpoint.y);
		oldpoint=where;
		Draw(Bounds());
	}
	if(buttons == B_SECONDARY_MOUSE_BUTTON)
	{	fontlocation=where;
		Draw(Bounds());
	}
}
