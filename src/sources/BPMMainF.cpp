#include "BPMMain.h"

void BPMApplication::Shortcuts(BMessage *msg)
{	
	// this collects any B_KEY_DOWN messages and processes them
	BMessage *newmsg;
	int8 bytes[3];
	int32 key;
	int32 modifiers;
	
	msg->FindInt32 ("modifiers", &modifiers); 
	msg->FindInt32 ("key", &key); 
	msg->FindInt8  ("byte", &bytes[0]); 

	switch ( bytes[0] )
	{ 
		case '+':	// zoom in
			newmsg=new BMessage(ZOOM_WINDOW);
			newmsg->AddBool("mode",bool(true));
			mainwin->PostMessage(newmsg);
			break;
			
		case '-':	// zoom out
			newmsg=new BMessage(ZOOM_WINDOW);
			newmsg->AddBool("mode",bool(false));
			mainwin->PostMessage(newmsg);
			break;
	
		case ' ':	// Space toggles zoom mode
	 		mainwin->PostMessage(new BMessage(B_ZOOM));
			break;

		case 'x':	// swap image colors
			if(mainwin->imgview->openimages>0)
			{	rgb_color tempcol;
				mainwin->LockLooper();
				tempcol=mainwin->imgview->p_active_image->forecolor;
				mainwin->imgview->p_active_image->forecolor=mainwin->imgview->p_active_image->backcolor;
				mainwin->imgview->p_active_image->backcolor=tempcol;
				mainwin->UnlockLooper();
				PostMessage(UPDATE_COLORS);
			}
			break;

		case 'd':	// set to black & white defaults
			if(mainwin->imgview->openimages>0)
			{
				mainwin->LockLooper();
				mainwin->imgview->p_active_image->forecolor.red=255;
				mainwin->imgview->p_active_image->forecolor.green=255;
				mainwin->imgview->p_active_image->forecolor.blue=255;
				mainwin->imgview->p_active_image->backcolor.red=0;
				mainwin->imgview->p_active_image->backcolor.green=0;
				mainwin->imgview->p_active_image->backcolor.blue=0;
				mainwin->UnlockLooper();
				PostMessage(UPDATE_COLORS);
			}
			break;

	 	case 'b':	// Brush
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_BRUSH));
			ShortcutToolbarUpdate();
			break;
	 	case 'k': 	// Flood Fill - Bucket
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_FILL));
			ShortcutToolbarUpdate();
			break;
		case 'n': 	// Line tool
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_LINE));
			ShortcutToolbarUpdate();
			break;
		case 'g': 	// Gradient tool
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_GRADIENT));
			ShortcutToolbarUpdate();
			break;
		case 'N':	// Spline
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_SPLINE));
			ShortcutToolbarUpdate();
			break;
		case 'r':	// Rectangle
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_RECT));
			ShortcutToolbarUpdate();
			break;
		case 'e':	// Ellipse
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_ELLIPSE));
			ShortcutToolbarUpdate();
			break;
		case 't':	// Text
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_TEXT));
			ShortcutToolbarUpdate();
			break;
		case 'z':	// Zoom
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_ZOOM));
			ShortcutToolbarUpdate();
			break;
	 	case 'i':	// Eyedropper
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_EYEDROPPER));
			ShortcutToolbarUpdate();
			break;
	 	case 's':	//Stamp
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_STAMP));
			ShortcutToolbarUpdate();
			break;
		case 'w':	//Wand
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_WAND));
			ShortcutToolbarUpdate();
			break;
		case 'R':	// Filled Rectangle
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_FRECT));
			ShortcutToolbarUpdate();
			break;
		case 'E':	// Filled Ellipse
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_FELLIPSE));
			ShortcutToolbarUpdate();
			break;
		case 'm':	// Quick Mask
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_MASK));
			break;
		case 'h':	// Hand
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_MOVE));
			ShortcutToolbarUpdate();
			break;
		case 'a':	// Eraser
	 		mainwin->toolwin->PostMessage(new BMessage(TOOL_ERASER));
			break;
	}
	
	// Function Keys
	switch ( key )
	{   
		case B_F1_KEY:
		{
			break; 
		}  			
		case B_F2_KEY: 
		{
			if(mainwin->app_prefs.mousewin_open==true)
				mainwin->mousewin->PostMessage(MOUSECAM_COPY_IMAGE);
			break; 
		}           
		case B_F3_KEY: 
		{
			if(mainwin->app_prefs.mousewin_open==true)
				mainwin->mousewin->PostMessage(MOUSECAM_FREEZE);
			break; 
		}           
		case B_F4_KEY: 
			break; 
		   
		case B_F5_KEY: 
			break; 
		
		case B_F6_KEY: 
			break; 
		
		case B_F7_KEY:
			break; 
	
	}
	
}

void BPMApplication::ShortcutToolbarUpdate(void)
{
	// This function was written to update the toolbar when tool shortcuts
	// are called. It's kind of a kludge, but it seems to work well enough
	// that this will be used for at least a little while
	if(mainwin->app_prefs.toolwin_open==true)
	{
		mainwin->toolwin->Lock();
		mainwin->toolwin->SelectTool(mainwin->imgview->currenttool,true);
		mainwin->toolwin->Unlock();
	}
}
void BPMApplication::TranslatorExport(BMessage *msg)
{
	entry_ref ref;
	BDirectory dir;
	BFile file;
	BString filename;
	
	msg->FindRef("directory",0,&ref);
	msg->FindString("name",&filename);
	
	dir.SetTo( &ref );
	file.SetTo( &dir, filename.String(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE );
	BNodeInfo fileinfo(&file);

	if( file.InitCheck() == B_OK )
	{
		mainwin->Lock();
		if(mainwin->imgview->p_active_image->typecode==0)
		{	mainwin->imgview->p_active_image->typecode=default_type;
			mainwin->imgview->p_active_image->MIME=default_mime;
		}

		ProgressiveBitmapStream inStream(mainwin->imgview->p_active_image->display_bitmap);
		if( BTranslatorRoster::Default()->Translate( &inStream, NULL, NULL, &file,
				mainwin->imgview->p_active_image->typecode) == B_OK )
		{	sprintf(mainwin->imgview->p_active_image->name,"%s",filename.String());
			fileinfo.SetType(mainwin->imgview->p_active_image->MIME.String());
		}
		
		mainwin->Unlock();
	}
	
}

void BPMApplication::ShowWait(void)
{
	if(waitwin_open==false)
	{	waitwin_open=true;
		BRect waitframe=mainwin->Frame();
		waitframe.right=waitframe.left+200;
		waitframe.bottom=waitframe.top+40;
		
		waitwin=new BWindow(waitframe,"BePhotoMagic",B_FLOATING_WINDOW, 
			B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE |
			B_AVOID_FOCUS);
		waitview=new WaitView(waitwin->Bounds());
		waitwin->AddChild(waitview);
		waitwin->Show();
		mainwin->Lock();
		mainwin->menubar->Hide();
		mainwin->Unlock();
	}
}

void BPMApplication::CloseWait(void)
{
	if(waitwin_open==true)
	{	waitwin_open=false;
		waitwin->Lock();
		waitwin->Close();
		mainwin->Lock();
		mainwin->menubar->Show();
		mainwin->Unlock();
	}
}