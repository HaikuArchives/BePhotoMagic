#include "BrushWindow.h"

BrushWindow::BrushWindow(BRect frame, char *title, int16 bwidth=16,int16 bheight=16, int8 blur=0, int8 btype=0)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE|
//				B_NOT_H_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
				B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
	BMessage *tempmsg;
	tempmsg=new BMessage(GET_APP_DIR);
	tempmsg->AddInt8("sourcewin",1);
	be_app->PostMessage(tempmsg);
	
	BRect rect  = Bounds();
	rect.right -= B_V_SCROLL_BAR_WIDTH+3;
	rect.bottom -= 23;

	allbrushes = new AllBrushesView(rect);
	scrollview = new BScrollView(SpTranslate("User Brushes"), allbrushes,B_FOLLOW_ALL, B_WILL_DRAW, false,true);
	
	dview = new BrushSelectorView(Bounds(),bwidth,bheight,blur,btype);
	dview->SetViewColor(216,216,216);
	
	BTabView *tabs = new BTabView(Bounds(), "", B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE_JUMP);
	tabs->AddTab(dview, new BTab()); 
	tabs->AddTab(scrollview, new BTab()); 
	AddChild(tabs);
	allbrushes->brushdir.SetTo("");
	
	FrameResized(Bounds().Width(),Bounds().Height());
}

BrushWindow::~BrushWindow()
{
	BMessage *msg=new BMessage(BRUSHWIN_CLOSED);
	msg->AddRect("frame",Frame());
	be_app->PostMessage(msg);
//	bpmwindow->PostMessage(msg);
}

void BrushWindow::FrameResized(float x, float y)
{
}

void BrushWindow::FrameMoved(BPoint origin)
{
	BMessage *msg=new BMessage (CHILDWIN_MOVED);
	msg->AddPoint("brushwin",origin);
	be_app->PostMessage(msg);
}

void BrushWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case BRUSHDIR_SET:
			allbrushes->AddBrushes();
			// Set scrollview range
		break;

		default:
			BWindow::MessageReceived(msg);
	 }
   	 	
}

AllBrushesView::AllBrushesView(BRect r) : 
	BView(r, "allbrushesview", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(216,216,216);
}

AllBrushesView::~AllBrushesView(void)
{
}

void AllBrushesView::AddBrushes()
{	BBitmap *bmp;

	// erase old brushes
	while (ChildAt(0) !=NULL)
		ChildAt(0)->RemoveSelf();

	char name[255];
	char tmp[255];
	
	sprintf(name,brushdir.Path());
	strcat(name,"/brushes/");

	//find # of brushes in directory
	BDirectory the_dir; 
	if(the_dir.SetTo(name) != B_OK)
		return;

	last_brush = the_dir.CountEntries();

	// Minor limitation for now...
	if(last_brush>255)
		last_brush=255;

	BEntry the_entry;
	entry_ref e_ref;
	
	BRect rect;

	int16 heightmax=32;
	char str[255];

	BRect trect(0,0,50,50);
    int32 i = 0;
    if(last_brush)
	{	do
		{	the_dir.GetNextEntry(&the_entry,false);
			the_entry.GetName(tmp);
		
			sprintf(name,brushdir.Path());
			strcat(name,"/brushes/");
			strcat (name,tmp);
			sprintf(str,"brushview%ld",i);
			bmp=LoadBitmap(name);
			if(bmp==NULL)
				continue;

			the_entry.GetRef(&e_ref);

			userbrushes[i]=new ThumbButton(trect,name,SELECT_USER_BRUSH,
					e_ref,bmp, B_FOLLOW_NONE, B_WILL_DRAW);
			if(userbrushes[i] != NULL)
			{	
				
				// Assuming that we got a valid file, move to the appropriate
				// spot in the view
				userbrushes[i]->MoveTo(rect.left,rect.top);

				if(userbrushes[i]->Bounds().IntegerHeight()>heightmax)
					heightmax=userbrushes[i]->Bounds().IntegerHeight();

//				if(!(Parent()->Bounds().Contains(userbrushes[i]->Frame())) )
				if(!(Bounds().Contains(userbrushes[i]->Frame())) )
				{	
					rect.OffsetBy(-rect.left,heightmax+10);
					userbrushes[i]->MoveTo(rect.left,rect.top);
					heightmax=userbrushes[i]->Bounds().IntegerHeight();
				}
				else
					rect.OffsetBy(userbrushes[i]->Bounds().IntegerWidth()+10,0); //horizontal
				rect.right=rect.left+userbrushes[i]->Bounds().IntegerWidth();
				AddChild(userbrushes[i]);
			}
		} while (i++ != last_brush-1);
	}
}

/*
BBitmap * AllBrushesView::LoadBitmap(char *filename)
{
	BFile file;
	
	if (file.SetTo(filename, B_READ_ONLY) == B_OK)
	{
		BTranslatorRoster *roster = BTranslatorRoster::Default();
		ProgressiveBitmapStream stream(NULL, NULL, false,false);
		
		roster->Translate(&file, 0, 0, &stream, B_TRANSLATOR_BITMAP);
		
		stream.SetDispose(false);
		return stream.Bitmap();
	}
	else
		return NULL;
}
*/
ThumbButton::ThumbButton(BRect frame, const char *name, int32 msg, entry_ref entry,
	BBitmap *bmp, int32 resize, int32 flags): BView(frame, name, resize, flags)
{	
	msgcmd=msg;
	display=bmp;
	ref=entry;

	BMessage *tempmsg;
	tempmsg=new BMessage(SELECT_USER_BRUSH);
	tempmsg->AddRef("ref",&ref);
	msgsender=new BInvoker(tempmsg,be_app);
	
//	if(Bounds().Contains(display->Bounds()))
//		ResizeTo(display->Bounds().Width(),display->Bounds().Height());
}

ThumbButton::~ThumbButton(void)
{	delete display;
	
	if(msgsender!=NULL)
		delete msgsender;
}

void ThumbButton::AttachedToWindow(void)
{
}

void ThumbButton::MouseUp(BPoint where)
{	msgsender->Invoke();
}

void ThumbButton::Draw(BRect update)
{
	if(Bounds().Contains(display->Bounds()))
	{	
		if(Parent()!=NULL)
		{	SetHighColor(Parent()->ViewColor());
			FillRect(Bounds());
		}
		DrawBitmap(display,display->Bounds(),display->Bounds());
	}
	else
		DrawBitmap(display,display->Bounds(),Bounds());
}

BBitmap *ThumbButton::RescaleBitmap(BBitmap *src, BRect outputsize, bool center_if_smaller)
{
// Rescales source bitmap to passed BRect size and returns a bitmap of the
// same color space

	BBitmap *dest;
	dest= new BBitmap(outputsize,src->ColorSpace(),true);

	if(dest==NULL)
		return dest;
	
	BPicture *destpic;

	dest->Lock();
	BView *virtualView = new BView(outputsize, NULL, B_FOLLOW_NONE, 0 );
	dest->AddChild( virtualView );

	virtualView->BeginPicture(new BPicture); 
   	
   	BRect rect = outputsize;
//	rect.InsetBy(1,1);
	
	
	if (center_if_smaller==true) //center without rescaling
	{
		if ( dest->Bounds().Width() > src->Bounds().Width() )  
		{ 
			rect.right = src->Bounds().right;
			rect.OffsetBy( (dest->Bounds().Width()-rect.right)/2,0);
		}
			
		if ( dest->Bounds().Height() > src->Bounds().Height() ) 
		{
			rect.bottom = src->Bounds().bottom;
			rect.OffsetBy( 0, (dest->Bounds().Height()-rect.bottom)/2);
		}
	}
	
	virtualView->DrawBitmap(src, src->Bounds(), rect);
		 
   	destpic = virtualView->EndPicture();
   
	virtualView->DrawPicture(destpic);
  	dest->RemoveChild(virtualView );
  	dest->Unlock();
  	return dest;
}

