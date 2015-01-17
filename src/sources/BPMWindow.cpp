#include "BPMWindow.h"

#include "Utils.h"
#include "TransModes.h"

BWindow *bpmwindow;	// global pointer to bypass a lot of be_app posting

BPMWindow::BPMWindow(BRect frame)
	: BWindow(frame,"", B_DOCUMENT_WINDOW,
//	 B_WILL_DRAW | B_ASYNCHRONOUS_CONTROLS | B_WILL_ACCEPT_FIRST_CLICK)
	 B_WILL_DRAW | B_ASYNCHRONOUS_CONTROLS)
{
	SetTitle("BePhotoMagic");
	save_BPM=true;
	saveframe=true;

	Image_Init(NULL);

	BRect rect;
	rect.Set( 0, 0,Bounds().right-1, 15);
	Lock();
	menubar = new BMenuBar(rect, "menubar");

	BMenu *tempmenu;
	
	tempmenu = new BMenu(SpTranslate("Save As"));
	tempmenu->AddItem(new BMenuItem(SpTranslate("via Translator"),new BMessage(FILE_EXPORT_BITMAP),'X',B_COMMAND_KEY|B_SHIFT_KEY));

	filemenu = new BMenu("File");
	filemenu->AddItem(new BMenuItem(SpTranslate("New Project"),new BMessage(FILE_NEW),'N',B_COMMAND_KEY));
	filemenu->AddSeparatorItem();
	filemenu->AddItem(new BMenuItem(SpTranslate("Open"),new BMessage(FILE_OPEN),'O',B_COMMAND_KEY));
	filemenu->AddItem(new BMenuItem(SpTranslate("Close"),new BMessage(FILE_CLOSE),'W',B_COMMAND_KEY));
	filemenu->AddSeparatorItem();
	filemenu->AddItem(new BMenuItem(SpTranslate("Save Project"),new BMessage(FILE_SAVE),'S',B_COMMAND_KEY));
	filemenu->AddItem(tempmenu);
	filemenu->AddSeparatorItem();
	filemenu->AddItem(new BMenuItem(SpTranslate("Quit"),new BMessage(FILE_QUIT),'Q',B_COMMAND_KEY));
	menubar->AddItem(filemenu);
	
	editmenu = new BMenu(SpTranslate("Edit"));
	editmenu->AddItem(new BMenuItem(SpTranslate("Undo"),new BMessage(EDIT_UNDO),'Z',B_COMMAND_KEY));
//	editmenu->AddSeparatorItem();
//	editmenu->AddItem(new BMenuItem("Copy Layer to Clipboard",new BMessage(EDIT_COPY),'C',B_COMMAND_KEY));
//	editmenu->AddItem(new BMenuItem("Paste to New Image",new BMessage(EDIT_PASTE_AS_NEW)));
	menubar->AddItem(editmenu);

//	imagemenu = new BMenu(SpTranslate("Image"));
	tempmenu =	new BMenu(SpTranslate("Merge"));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Merge Down"),new BMessage(LAYER_MERGE_DOWN),'E',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Flatten Image"),new BMessage(LAYER_FLATTEN),'F',B_COMMAND_KEY | B_SHIFT_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Make Composite Layer"),new BMessage(LAYER_COMPOSITE),'C',B_COMMAND_KEY | B_SHIFT_KEY));

	layermenu = new BMenu(SpTranslate("Layer"));
	layermenu->AddItem(new BMenuItem(SpTranslate("New"),new BMessage(LAYER_NEW),'N',B_COMMAND_KEY | B_SHIFT_KEY));
	layermenu->AddItem(new BMenuItem(SpTranslate("Delete"),new BMessage(LAYER_DELETE),'D',B_COMMAND_KEY | B_SHIFT_KEY));
	layermenu->AddItem(new BMenuItem(SpTranslate("Duplicate"),new BMessage(LAYER_DUPLICATE)));
	layermenu->AddSeparatorItem();
	layermenu->AddItem(new BMenuItem(SpTranslate("Move Up"),new BMessage(LAYER_MOVE_UP),B_UP_ARROW,B_COMMAND_KEY | B_SHIFT_KEY));
	layermenu->AddItem(new BMenuItem(SpTranslate("Move Down"),new BMessage(LAYER_MOVE_DOWN),B_DOWN_ARROW,B_COMMAND_KEY | B_SHIFT_KEY));
	layermenu->AddItem(new BMenuItem(SpTranslate("Move to Top"),new BMessage(LAYER_MOVE_TOP),B_UP_ARROW,B_COMMAND_KEY | B_SHIFT_KEY | B_OPTION_KEY));
	layermenu->AddItem(new BMenuItem(SpTranslate("Move to Bottom"),new BMessage(LAYER_MOVE_BOTTOM),B_DOWN_ARROW,B_COMMAND_KEY | B_SHIFT_KEY | B_OPTION_KEY));
	layermenu->AddSeparatorItem();
	layermenu->AddItem(tempmenu);
//	layermenu->AddSeparatorItem();
//	layermenu->AddItem(new BMenuItem(SpTranslate("Copy to Clipboard"),new BMessage(EDIT_COPY),'C',B_COMMAND_KEY));
	menubar->AddItem(layermenu);

	maskmenu = new BMenu(SpTranslate("Mask"));
	maskmenu->AddItem(new BMenuItem(SpTranslate("Select All"),new BMessage(SELECT_ALL),'a',B_COMMAND_KEY));
	maskmenu->AddItem(new BMenuItem(SpTranslate("Select None"),new BMessage(SELECT_NONE),'d',B_COMMAND_KEY));
	menubar->AddItem(maskmenu);
	
	filtermenu = new BMenu("Plugin");
	menubar->AddItem(filtermenu);
	
	windowmenu = new BMenu(SpTranslate("Image in Memory"));
	windowmenu->SetRadioMode(true);

	displaymenu = new BMenu(SpTranslate("Display"));
	displaymenu->AddItem(windowmenu);
	displaymenu->AddSeparatorItem();

	tempmenu = new BMenu(SpTranslate("Window"));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Tool Window"),new BMessage(TOGGLE_TOOL_WINDOW),'1',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Brush Window"),new BMessage(TOGGLE_BRUSH_WINDOW),'2',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Layer Window"),new BMessage(TOGGLE_LAYER_WINDOW),'3',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle MouseCam Window"),new BMessage(TOGGLE_MOUSECAM_WINDOW),'4',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Options Window"),new BMessage(TOGGLE_OPTIONS_WINDOW),'5',B_COMMAND_KEY));
	tempmenu->AddSeparatorItem();
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Foreground Window"),new BMessage(TOGGLE_FORECOLOR),'6',B_COMMAND_KEY));
	tempmenu->AddItem(new BMenuItem(SpTranslate("Toggle Background Window"),new BMessage(TOGGLE_BACKCOLOR),'7',B_COMMAND_KEY));
	displaymenu->AddItem(tempmenu);
	displaymenu->AddSeparatorItem();

	displaymenu->AddItem(new BMenuItem(SpTranslate("Windowed Mode"),new BMessage(SCREEN_FULL_NONE),',',B_COMMAND_KEY | B_SHIFT_KEY));
	displaymenu->AddItem(new BMenuItem(SpTranslate("Full Screen"),new BMessage(SCREEN_FULL_HALF),'.',B_COMMAND_KEY | B_SHIFT_KEY));
	displaymenu->AddItem(new BMenuItem(SpTranslate("Full Screen, No Menu"),new BMessage(SCREEN_FULL_ALL),'/',B_COMMAND_KEY | B_SHIFT_KEY));
	displaymenu->AddSeparatorItem();
	displaymenu->AddItem(new BMenuItem("Force Redraw",new BMessage(FORCE_REDRAW),'r',B_COMMAND_KEY));
	menubar->AddItem(displaymenu);
	
	helpmenu = new BMenu(SpTranslate("Help"));
//	helpmenu->AddItem(new BMenuItem(SpTranslate("What's This?"),new BMessage(HELP_WHATSTHIS)));
//	helpmenu->AddItem(new BMenuItem(SpTranslate("User's Manual"),new BMessage(HELP_MANUAL)));
//	helpmenu->AddSeparatorItem();
	helpmenu->AddItem(new BMenuItem(SpTranslate("About"),new BMessage(HELP_ABOUT)));
	menubar->AddItem(helpmenu);
	
	AddChild(menubar);
	SetFileMenuState(false);
	SetPluginMenuState(false);
	
	// Set up the two views - one for the desktop and one for the images
	// The desktop one will allow us to customize the background, amongst
	// other things. There are probably better ways to do this...
	imgview=new PicView( BRect(100,100,300,300), B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER);
	imgview->Hide();
	
	BRect dsr(Bounds());
	dsr.top=20;
	dsr.right=dsr.left+Bounds().Width()-B_V_SCROLL_BAR_WIDTH;
	dsr.bottom=dsr.top+Bounds().Height()-B_H_SCROLL_BAR_HEIGHT-20;
	
	desktop=new DesktopView(dsr,imgview);
	desktop->SetViewColor(192,192,192);
	
	// Gots to be able to scroll these bad boys around, too.
	scrollview = new BScrollView("scrollview", desktop, B_FOLLOW_ALL, B_WILL_DRAW,TRUE, TRUE);
	desktop->AddChild(imgview);
	AddChild(scrollview);
	hscrollbar 	= scrollview->ScrollBar(B_HORIZONTAL);
	vscrollbar 	= scrollview->ScrollBar(B_VERTICAL);

	Unlock();
	
	untitled=0;
	createwin_open=false;
	openpanel=new BPMOpenPanel();

	// tweak panels so that it can "compete" with the child windows
	BWindow *panelwin;
	panelwin=openpanel->Window();
	panelwin->SetFeel(B_FLOATING_APP_WINDOW_FEEL);
	viewcursor=new AppCursor();
	show_menubar=true;

	savepanel=new BPMSavePanel(new BMessage(BPM_SAVE_REF));
	panelwin=savepanel->Window();
	panelwin->SetFeel(B_FLOATING_APP_WINDOW_FEEL);
	filterwin_open=false;

	SetFilterMenu();

	// Load and set application preferences
	app_prefs.Load("/boot/home/config/settings/BePhotoMagic-0.60");
	
	// Reset main window size
	ResizeTo(app_prefs.mainwin_frame.Width(),app_prefs.mainwin_frame.Height());
	MoveTo(app_prefs.mainwin_frame.LeftTop());

	normal_frame=app_prefs.mainwin_frame;
	
	if(app_prefs.toolwin_open==true)
	{	toolwin=new ToolWindow(app_prefs.toolwin_frame, false);
		toolwin->toolbara->SetButton("paintbrush",true);
		toolwin->currenttool=TOOL_BRUSH;
		toolwin->Show();
		toolwin->toolbara->SetButton("full_none",true);
	}
	if(app_prefs.layerwin_open==true)
	{
		layerwin=new LayerWindow(app_prefs.layerwin_frame,
						(char *)SpTranslate("Layers"));
		layerwin->Show();
	}
	if(app_prefs.brushwin_open==true)
	{
		brushwin=new BrushWindow(app_prefs.brushwin_frame,(char *)SpTranslate("Brushes"));
		brushwin->Show();
	}

	if(app_prefs.optionswin_open==true)
	{	optionswin=new OptionsWindow(app_prefs.optionswin_frame,TOOL_BRUSH);
		optionswin->Show();
	}

	if(app_prefs.forewin_open==true)
	{	forewin=new ColorWindow(app_prefs.forepoint,true);
		if(imgview->openimages>0)
			SetColor(true,true);
		forewin->Show();
	}

	if(app_prefs.backwin_open==true)
	{	backwin=new ColorWindow(app_prefs.backpoint,false);
		if(imgview->openimages>0)
			SetColor(false,true);
		backwin->Show();
	}

	if(app_prefs.mousewin_open==true)
	{
		mousewin=new TWindow(10);
		mousewin->MoveTo(app_prefs.mousepoint);
		mousewin->Show();
	}
	
	bpmwindow=(BWindow *)this;
	textwin_open=false;
}

BPMWindow::~BPMWindow()
{	
	app_prefs.Save("/boot/home/config/settings/BePhotoMagic-0.60");
	Image_Shutdown();
	delete openpanel;
	delete savepanel;
}

bool BPMWindow::QuitRequested()
{
	if(filterwin_open==true)
	{	// Close the filter window here
		filterwin_open=false;
		filterwin->Lock();
		filterwin->Quit();
	}
	if(find_thread("BPMViewUpdater")!=B_NAME_NOT_FOUND)
		return false;

	if(NotifySaveChanges()==false)
		return false;

	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}

void BPMWindow::FrameResized(float width, float height)
{	
	CenterImageView(true);
	if(saveframe==true)
		app_prefs.mainwin_frame=Frame();
}

void BPMWindow::FrameMoved(BPoint origin)
{
	if(saveframe==true)
		app_prefs.mainwin_frame=Frame();
}