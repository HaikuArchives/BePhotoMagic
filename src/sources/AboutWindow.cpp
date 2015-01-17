#include "AboutWindow.h"

// Global to file - invisible to rest of program
BRect CenteredRect(float width, float height);

AboutWindow::AboutWindow(bool locked=false)
				: BWindow(CenteredRect(400,300), SpTranslate("About"), B_MODAL_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
	ab_view = new AboutView(Bounds(),Flags());
	ab_view->locked=locked;	
	AddChild(ab_view);
	SetPulseRate(50*1000);  
}

void AboutWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case 1: break;
	}
}

AboutWindow::~AboutWindow()
{
}
 
void AboutView::MouseDown(BPoint point)
{
	if(locked==false)
	{	Window()->Lock();
		Window()->Close();
	}
}


void AboutView::Draw(BRect update_rect) 
{  
	if (title_pic!=NULL) 
		DrawBitmap(title_pic,update_rect,update_rect);

	BPoint ori;
	ori.Set(26,180);
/*
	SetDrawingMode(B_OP_BLEND); //if antialiasing used
	SetHighColor(255,255,255);
	SetFont(be_plain_font);
	SetFontSize(12);
	
	DrawString("Original Photon codebase © Santiago Lema",ori); 
	ori+=BPoint(0,17);
	DrawString("BePhotoMagic © BPM Dev Team",ori);
	ori+=BPoint(0,29);
	DrawString(SpTranslate("Get latest version at"),ori);
 	ori+=BPoint(0,13);
 	DrawString("http://home.columbus.rr.com/bpmagic/",ori);
*/
	SetDrawingMode(B_OP_BLEND);
	SetFont(be_bold_font);
	SetFontSize(14);
	SetHighColor(64,64,64);

	BFont fnt;
	GetFont(&fnt);
	float wid = fnt.StringWidth("0.62");
//	ori.Set(Bounds().Width()-120-wid,Bounds().Height()-16);
	ori.Set(Bounds().Width()-1-wid,12);
	DrawString("0.62",ori); 
	SetDrawingMode(B_OP_COPY);
 	
}

AboutView::AboutView(BRect rec, long flags):
	BView(rec, "About view", flags, B_WILL_DRAW)
{
	locked=false;
	SetViewColor(255,255,255);
	
	BRect rect(0,0,399,299);
	
	title_pic = BTranslationUtils::GetBitmap('bits',"bpmagic.jpg");
	if (title_pic==NULL) 
		title_pic = new BBitmap(rect,B_RGB32);
	
	rect = Bounds();
	rect.top = rect.bottom -12;
	stview = new SlideTextView(rect, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	AddChild(stview);
} 


AboutView::~AboutView()
{
}

void SlideTextView::Pulse() 
{  
	if (posi==0)
	{	ScrollTo(0,0);
		ScrollBy(Bounds().Width()*-1, 0);
	}

	if (posi < max_posi+2) 
	{
		posi++;
		ScrollBy(2,0);
	}
	else
		posi= 0;
}



void SlideTextView::Draw(BRect update_rect) 
{  
	if (posi!=0)
		DrawString(text,BPoint(0,10)); 
}	

	
SlideTextView::SlideTextView(BRect rec, long flags):
	BView(rec, "slide_view", flags, B_WILL_DRAW | B_PULSE_NEEDED)
{
	SetViewColor(0,0,0);
	
	text = (char *) malloc(512);
	
	sprintf(text,SpTranslate("Makes Use Of: "));
	strcat(text," ");
	strcat(text,"ProgressiveBitmap class (Edmund Vermeulen)");
	strcat(text," - Image Manip Library (Edmund Vermeulen)");
	strcat(text," - SpLocale (Bernard Krummenacher)");
	
	SetDrawingMode(B_OP_ADD); //takes care of antialiasing
	SetHighColor(255,255,255);
	
	SetFont(be_plain_font);
	SetFontSize(10);

	BFont fnt;
	GetFont(&fnt);
	
	max_posi = int32(floor(fnt.StringWidth(text)));
	float virgule;
	
	virgule  = max_posi / Bounds().Width();
	virgule -= floor(virgule);
	
	virgule *= Bounds().Width();
	virgule *= 1.7;
	
	max_posi += (int32) floor(virgule+1);
	
	posi = 0;
}

BRect CenteredRect(float width, float height)
{	BScreen screen;
	BRect rect;
	float dx,dy;
	
	if(screen.IsValid())
	{	rect=screen.Frame();
	
		dx=(rect.Width()>width)?(rect.Width()-width):0;
		dy=(rect.Height()>height)?(rect.Height()-height):0;
		rect.InsetBy(dx/2,dy/2);
		return rect;
	}
	return BRect(0,0,0,0);
}

