#include "ToolWindow.h"

#define THREE_TOOLBAR_WIDTH 73
#define TWO_TOOLBAR_WIDTH 48

ToolWindow::ToolWindow(BRect frame,bool ismaskmode)
				: BWindow(frame,SpTranslate("Tools"), B_FLOATING_WINDOW,
				B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | 
				B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
toolview = new BView(Bounds(),"",B_FOLLOW_ALL,B_WILL_DRAW);
toolview->SetViewColor(192,192,192);
AddChild(toolview);

toolbara = new ToolbarView(BRect(0,0,23,23),"toolbara",B_FOLLOW_NONE,B_WILL_DRAW);
toolbarb = new ToolbarView(BRect(24,0,48,23),"toolbarb",B_FOLLOW_NONE,B_WILL_DRAW);
//toolbarc = new ToolbarView(BRect(49,0,73,23),"toolbarc",B_FOLLOW_NONE,B_WILL_DRAW);
toolview->AddChild(toolbara);
toolview->AddChild(toolbarb);
//toolview->AddChild(toolbarc);
toolbara->SetLayout(TOOLBAR_VERTICAL);
toolbarb->SetLayout(TOOLBAR_VERTICAL);
//toolbarc->SetLayout(TOOLBAR_VERTICAL);

icondir=new BPath;
AppDir(icondir);
icondir->Append("config");

LoadButton(toolbara,"paintbrush","paintbrush.png","XXpaintbrush.png",new BMessage(TOOL_BRUSH));
LoadButton(toolbara,"bucket","bucket.png","XXbucket.png",new BMessage(TOOL_FILL));
LoadButton(toolbara,"line","line.png","XXline.png",new BMessage(TOOL_LINE));
LoadButton(toolbara,"spline","spline.png","XXspline.png",new BMessage(TOOL_SPLINE));
LoadButton(toolbara,"rect","rect.png","XXrect.png",new BMessage(TOOL_RECT));
LoadButton(toolbara,"ellipse","ellipse.png","XXellipse.png",new BMessage(TOOL_ELLIPSE));
LoadButton(toolbara,"text","text.png","XXtext.png",new BMessage(TOOL_TEXT));
LoadButton(toolbara,"zoom","zoom.png","XXzoom.png",new BMessage(TOOL_ZOOM));
toolbara->SetTarget((BLooper *)this);

LoadButton(toolbarb,"picker","picker.png","XXpicker.png",new BMessage(TOOL_EYEDROPPER));
LoadButton(toolbarb,"stamp","stamp.png","XXstamp.png",new BMessage(TOOL_STAMP));
LoadButton(toolbarb,"wand","wand.png","XXwand.png",new BMessage(TOOL_WAND));
LoadButton(toolbarb,"eraser","eraser.png","XXeraser.png",new BMessage(TOOL_ERASER));
LoadButton(toolbarb,"frect","frect.png","XXfrect.png",new BMessage(TOOL_FRECT));
LoadButton(toolbarb,"fellipse","fellipse.png","XXfellipse.png",new BMessage(TOOL_FELLIPSE));
LoadButton(toolbarb,"mask","mask.png","XXmask.png",new BMessage(TOOL_MASK));
LoadButton(toolbarb,"move","move.png","XXmove.png",new BMessage(TOOL_MOVE));
toolbarb->SetTarget((BLooper *)this);

//LoadButton(toolbarc,"gradient","gradient.png","XXgradient.png",new BMessage(TOOL_GRADIENT));
//toolbarc->SetTarget((BLooper *)this);

BRect rect;
rect.left=0;
rect.top=toolbara->Bounds().Height()+1;
rect.bottom=rect.top+23;
rect.right=23;

forebutton = new DrawButton(rect,"forebutton",TOGGLE_FORECOLOR,B_FOLLOW_NONE,B_WILL_DRAW);
toolview->AddChild(forebutton);

rect.OffsetBy(24,0);
backbutton = new DrawButton(rect,"backbutton",TOGGLE_BACKCOLOR,B_FOLLOW_NONE,B_WILL_DRAW);
backbutton->SetViewColor(0,0,0);
toolview->AddChild(backbutton);

forebutton->SetViewColor(255,255,255);
backbutton->SetViewColor(0,0,0);
forebutton->SetTarget(be_app);
backbutton->SetTarget(be_app);

rect.OffsetBy(-48,25);
rect.left=0;
rect.right = 48;
rect.bottom =rect.top+48;

brushview = new DrawButton(rect,"brushview",TOGGLE_BRUSH_WINDOW,
			B_FOLLOW_NONE,B_WILL_DRAW);
toolview->AddChild(brushview);

brushview->SetTarget(be_app);

rect.top=rect.bottom+1;
rect.bottom=rect.top+12;
rect.left=0;
rect.right=48;
fullscreen = new ToolbarView(rect,"fullscreen",B_FOLLOW_NONE,B_WILL_DRAW);
toolview->AddChild(fullscreen);

// Windowed mode
LoadButton(fullscreen,"full_none","full_none_up.png","full_none_down.png",new BMessage(SCREEN_FULL_NONE));

// Fullscreen + menu
LoadButton(fullscreen,"full_half","full_half_up.png","full_half_down.png",new BMessage(SCREEN_FULL_HALF));

// Fullscreen - menu
LoadButton(fullscreen,"full_full","full_full_up.png","full_full_down.png",new BMessage(SCREEN_FULL_ALL));
fullscreen->SetTarget((BLooper *)this);

ResizeTo(TWO_TOOLBAR_WIDTH,rect.bottom);	

// Center the fullscreen toolbar
if(fullscreen->Bounds().Width()<TWO_TOOLBAR_WIDTH)
	fullscreen->MoveBy((TWO_TOOLBAR_WIDTH-fullscreen->Bounds().right)/2,0);

be_app->PostMessage(TOOL_BRUSH);
}

ToolWindow::~ToolWindow()
{
	delete icondir;
}
/*
bool ToolWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}
*/
void ToolWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
	   	case TOOL_BRUSH:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_FILL:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_LINE:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_SPLINE:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

		case TOOL_RECT:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_ELLIPSE:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_TEXT:
			be_app->PostMessage(msg);
   	 		break; 	

		case TEXTWIN_CLOSED:
			toolbara->SetButton("text",false);
			break;
			
	   	case TOOL_ZOOM:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

		// Second column of buttons

	   	case TOOL_EYEDROPPER:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_STAMP:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_WAND:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_ERASER:
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_FRECT:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_FELLIPSE:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break; 	

	   	case TOOL_MASK:
			be_app->PostMessage(msg);
	   	 	break; 	 
		
		case TOOL_MOVE:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
   	 		break;

		case TOOL_GRADIENT:
			SelectTool(currenttool,false);
	   		currenttool=msg->what;
			be_app->PostMessage(msg);
			break;

   	 	case SCREEN_FULL_NONE:
		   	fullscreen->DeselectAll();
			be_app->PostMessage(msg);
   	 		break;
   	 	
   	 	case SCREEN_FULL_HALF:
		   	fullscreen->DeselectAll();
			be_app->PostMessage(msg);
   		 	break;
   	 	
   	 	case SCREEN_FULL_ALL:
		   	fullscreen->DeselectAll();
			be_app->PostMessage(msg);
   		 	break;

		default:
			BWindow::MessageReceived(msg);
   	}
}

#ifndef CHILDWIN_MOVED
	#define CHILDWIN_MOVED 'cwmv'
#endif

void ToolWindow::FrameMoved(BPoint origin)
{
	BMessage *msg=new BMessage(CHILDWIN_MOVED);
	msg->AddPoint("toolwin",origin);
	be_app->PostMessage(msg);
}

rgb_color ToolWindow::GetColor(bool fore=true)
{	
	if(fore)
		return forebutton->ViewColor();
	else
		return backbutton->ViewColor();
}

void ToolWindow::SetColor(rgb_color color, bool fore=true)
{	
	if(fore)
	{	forebutton->SetViewColor(color);
		forebutton->Invalidate();
	}
	else
	{	backbutton->SetViewColor(color);
		backbutton->Invalidate();
	}
}

bool ToolWindow::LoadButton(ToolbarView *toolbar,const char *name,const char *onname, 
			const char *offname, BMessage *msg)
{
	BString iconpath;
	BBitmap *on,*off;

	iconpath=icondir->Path();
	iconpath+="/"; iconpath+=onname;
	on=LoadBitmap((char *)iconpath.String());
	if(on==NULL)
		on=BTranslationUtils::GetBitmap('bits',onname);

	iconpath=icondir->Path();
	iconpath+="/"; iconpath+=offname;
	off=LoadBitmap((char *)iconpath.String());
	if(off==NULL)
		off=BTranslationUtils::GetBitmap('bits',offname);

	toolbar->AddButton(name,on,off,msg);

	return true;
}

void ToolWindow::SelectTool(int32 tool,bool value)
{
	switch(currenttool)
	{
		case TOOL_BRUSH:
			toolbara->SetButton("paintbrush",value);
			break;
	   	case TOOL_FILL:
			toolbara->SetButton("bucket",value);
   	 		break; 	

	   	case TOOL_LINE:
			toolbara->SetButton("line",value);
   	 		break; 	

	   	case TOOL_SPLINE:
			toolbara->SetButton("spline",value);
   	 		break; 	

		case TOOL_RECT:
			toolbara->SetButton("rect",value);
   	 		break; 	

	   	case TOOL_ELLIPSE:
			toolbara->SetButton("ellipse",value);
   	 		break; 	

	   	case TOOL_ZOOM:
			toolbara->SetButton("zoom",value);
   	 		break; 	

		// Second column of buttons

	   	case TOOL_EYEDROPPER:
			toolbarb->SetButton("picker",value);
   	 		break; 	

	   	case TOOL_STAMP:
			toolbarb->SetButton("stamp",value);
   	 		break; 	

	   	case TOOL_WAND:
			toolbarb->SetButton("wand",value);
   	 		break; 	

	   	case TOOL_FRECT:
			toolbarb->SetButton("frect",value);
   	 		break; 	

	   	case TOOL_FELLIPSE:
			toolbarb->SetButton("fellipse",value);
   	 		break; 	

		case TOOL_MOVE:
			toolbarb->SetButton("move",value);
   	 		break;
   	 	
   	 	//  Third column of buttons
   	 	
   	 	case TOOL_GRADIENT:
 	 		toolbarc->SetButton("gradient",value);
   	 		break;
		default:
			break;
	}
}
