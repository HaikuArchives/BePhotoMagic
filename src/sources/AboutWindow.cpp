#include "AboutWindow.h"

AboutWindow::AboutWindow(BRect frame, share *sh)
				: BWindow(frame, "About", B_MODAL_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{

shared=sh; 

//SetLook(B_NO_BORDER_WINDOW_LOOK);
   
ab_view = new AboutView(Bounds(),shared,Flags());

AddChild(ab_view);
SetPulseRate(50*1000);  

}

void AboutWindow::MessageReceived(BMessage *msg)
{
	//BRect rect;
	switch (msg->what)
	{
		 case 1: break;
   	 }

//send message to parent window
//util.mainWin->PostMessage(new BMessage(About_CHANGED)); 

}//end message received

AboutWindow::~AboutWindow()  {  }
 
void AboutView::MouseDown(BPoint point)
{  
	Window()->Lock();
	Window()->Close();
}


void AboutView::Draw(BRect update_rect) 
{  
	if (title_pic!=NULL) 
		DrawBitmap(title_pic,update_rect,update_rect);


	SetDrawingMode(B_OP_BLEND); //if antialiasing used
	SetHighColor(255,255,255);
	
	BPoint ori;
	ori.Set(26,180);

	SetFont(be_plain_font);
	SetFontSize(12);
	
							DrawString("Photon 0.51 codebase © Santiago Lema",ori); 
	ori+=BPoint(0,17);		DrawString("BePhotoMagic © BPM Dev Team",ori);

	ori+=BPoint(0,29);		DrawString(Language.get("GET_LATEST_VERSION"),ori);
 	ori+=BPoint(0,13);		DrawString("http://sourceforge.net/projects/bpmagic/",ori);


/* buggy and unnecessary in my opinion	--DW
	if (ThePrefs.no_pictures_left==OFF)
	{	//2x ARGB + 2 x MASK 8 bits
	
		float size= shared->act_img->pix_per_line * shared->act_img->pix_per_row * (4+4+1+1) ; 
	
		uint32 pos=0;
		while (shared->act_img->undo_data->undo[pos]!=NULL)
		{	size += shared->act_img->undo_data->undo[pos]->BitsLength();  
			pos++;
		}
	
		size +=0;
	
		size /= 1024;
		char str[255];		
		if (size < 1000) 
			sprintf(str,"%.2f Kb",size);
	 	else 
	 	{	size/=1024; 
	 		sprintf(str,"%.2f Mb",size);
	 	}
 	
 		ori+=BPoint(0,28);		DrawString(Language.get("MEM_USAGE"),ori);
 		ori+=BPoint(0,12);		DrawString(str,ori);
	}
	else 
	{
 	 ori+=BPoint(0,28);		DrawString(Language.get("NO_IMAGE_LOADED"),ori);
	}
*/	
	
	SetFont(be_bold_font);
	SetFontSize(14);
	SetHighColor(232,204,157);

	BFont fnt;
	GetFont(&fnt);
	float wid = fnt.StringWidth(util.version_only_txt);
	ori.Set(Bounds().Width()-120-wid,Bounds().Height()-16);
	DrawString(util.version_only_txt,ori); 
	SetDrawingMode(B_OP_COPY);
 	
}

AboutView::AboutView(BRect rec, share *sh, long flags):
	BView(rec, "About view", flags, B_WILL_DRAW)
{
shared=sh;
SetViewColor(255,255,255); //White

BRect rect(0,0,400-1,300-1);

title_pic = BTranslationUtils::GetBitmap('bits',"bpmagic.jpg");
if (title_pic==NULL) title_pic = new BBitmap(rect,B_RGB32);

rect = Bounds();
rect.top = rect.bottom -12;
stview = new SlideTextView(rect, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
AddChild(stview);

} 


AboutView::~AboutView()  {  }


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

//	if (posi!=0) DrawString("Hello",BPoint(0,10)); 

}	

	
SlideTextView::SlideTextView(BRect rec, long flags):
	BView(rec, "slide_view", flags, B_WILL_DRAW | B_PULSE_NEEDED)
{
SetViewColor(0,0,0);


text = (char *) malloc(512);

sprintf(text,Language.get("MAKES_USE_OF"));
strcat(text," ");
strcat(text,"ProgressiveBitmap class (Edmund Vermeulen)");
strcat(text," - Image Manip Library (Edmund Vermeulen)");
strcat(text," - LanguageClass (Dirk Olbertz)");
strcat(text," - TextMessage (Bjorn Tornqvist)");
strcat(text," - BubbleHelp (Marco Nelissen)");
strcat(text," - DThumbnail (Thorsten Seitz)");


SetDrawingMode(B_OP_ADD); //takes care of antialiasing
SetHighColor(255,255,255);

SetFont(be_plain_font);
SetFontSize(10);

/*
BStringView *stv = new BStringView(Bounds(),"stv",text,B_FOLLOW_ALL,B_WILL_DRAW);
AddChild(stv);
*/

BFont fnt;
GetFont(&fnt);

max_posi = int32(floor(fnt.StringWidth(text)));
float virgule;

virgule  = max_posi / Bounds().Width();
virgule -= floor(virgule);

virgule *= Bounds().Width();
virgule *= 1.7; //il manque encore un peu donc...

max_posi += (int32) floor(virgule+1);

posi = 0;
}