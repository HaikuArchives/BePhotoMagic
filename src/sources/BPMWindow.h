#ifndef BPM_WINDOW_H
#define BPM_WINDOW_H

#include <Application.h>
#include <Screen.h>
#include <Window.h>
#include <Alert.h>
#include <Bitmap.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Bitmap.h>
#include <FilePanel.h>
#include <ScrollView.h>
#include <ScrollBar.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>
#include <stdio.h>

#include "BBitmapAccessor.h"
#include "ImageManip.h"
#include "SpLocaleApp.h"
#include "BPMFile.h"
#include "BPMPrefs.h"
#include "BPMView.h"
#include "AboutWindow.h"
#include "BrushWindow.h"
#include "ColorWindow.h"
#include "CreateWindow.h"
#include "FilterWindow.h"
#include "OptionsWindow.h"
#include "LayerWindow.h"
#include "LayerWindowMsgs.h"
#include "SavePanel.h"
#include "OpenPanel.h"
#include "TextToolWindow.h"
#include "ToolWindow.h"
#include "MouseCam.h"

#define FORCE_REDRAW 'frrd'

#define FILE_NEW 'flnw'
#define FILE_OPEN 'flop'
#define FILE_OPEN_BPM 'flob'
#define FILE_CLOSE 'flcl'
#define FILE_SAVE 'flsv'
#define FILE_SAVEAS 'flsa'
#define FILE_QUIT 'flqu'
#define FILE_EXPORT_BITMAP 'fxbm'
#define FILE_EXPORT_GIF89 'fxgf'

#define EDIT_UNDO	'edun'
#define EDIT_REDO 	'edre'
#define EDIT_CUT	'edct'
#define EDIT_COPY	'edcp'
#define EDIT_PASTE	'edps'
#define EDIT_PASTE_INTO	'edpi'
#define EDIT_PASTE_AS_NEW	'edpn'

#define HELP_ABOUT 'hpab'
#define HELP_MANUAL 'hpmn'
#define HELP_WHATSTHIS 'hpwt'

#define FILE_CHOSEN 'flch'
#define ZOOM_WINDOW 'zmwn'
#define UPDATE_COLORS 'upcl'
#define UPDATE_PREFERENCES 'uppr'

#ifndef UPDATE_BRUSH
	#define UPDATE_BRUSH 'upbr'
#endif

#ifndef TOGGLE_TOOL_WINDOW
	#define TOGGLE_TOOL_WINDOW 'tgtw'
#endif
#ifndef TOGGLE_LAYER_WINDOW
	#define TOGGLE_LAYER_WINDOW 'tgtw'
#endif

#ifndef FILTERED_BITMAP
	 FILTERED_BITMAP 'fbmp'
#endif
#define SHOW_FILTER_WINDOW 'shfw'
#define SHOW_CONVERT_WINDOW 'shcw'

extern BWindow *bpmwin;

class BPMWindow : public BWindow 
{
public:
	
	BPMWindow(BRect frame);
	~BPMWindow(void);
	
	virtual void FrameMoved(BPoint origin);
	virtual void FrameResized(float width, float height);
	void MessageReceived(BMessage *msg);
	virtual	bool QuitRequested();
	
	//	UI members
	BScrollView *scrollview;
	BScrollBar *hscrollbar,*vscrollbar;
	DesktopView		*desktop;
	BBitmap *wallpaper;
	PicView		*imgview;
	
	bool show_menubar;
	BMenuBar	*menubar;
	BMenu	*filemenu, *editmenu, *imagemenu;
	BMenu	*maskmenu, *windowmenu, *helpmenu;
	BMenu	*displaymenu, *layermenu, *filtermenu;

	// Child windows and related data members
	BPMOpenPanel *openpanel;
	BPMSavePanel *savepanel;

	AboutWindow *aboutwin;
	CreateWindow *createwin;
	LayerWindow *layerwin;
	FilterWindow *filterwin;
	BrushWindow *brushwin;
	ColorWindow *forewin,*backwin;
	OptionsWindow *optionswin;
	TextToolWindow *textwin;
	ToolWindow *toolwin;
	TWindow *mousewin;

//	bool layerwin_open,filterwin_open,createwin_open;
	bool filterwin_open,createwin_open,textwin_open;
//	BRect layerwin_frame;

	BRect normal_frame;	

	// App-related functions
	void OpenImage(const char *tempstr, BBitmap *bmp);
	BRect CreateCenteredRect(float width,float height);
	BRect CenterImageView(bool reset_scrollbars);
	void CopyRect(BRect sourcerect, BBitmap *sourcebmp, 
					 BRect destrect, BBitmap *destbmp, uint8 colorspace_size);
	void SetFullScreen(int32 mode);
	void SaveActive(BMessage *msg);
	bpm_status OpenBPM(const char *path);
	void SetFilterMenu(void);
	void UpdateOptionsWindow(void);
	void UpdateLayerNumber(void);
	void UpdateLayerWindow(void);
	void UpdateToolbar(void);
	void UpdateBrushControls(void);
	void SetColor(bool fore=true, bool fromimage=false);
	void SetUserBrush(BBitmap *brush);
	void UpdateBrush(void);
	void SetBrush(BMessage *msg);
	bool NotifySaveChanges(void);
	void SetFileMenuState(bool state);
	void SetClipboardMenuState(bool state);
	void SetPluginMenuState(bool state);
	
	// data members
	uint8 untitled;
	AppCursor *viewcursor;
	int8 currentcursor;
	bool save_BPM;		// flag to toggle between dumping to translator
						// and saving natively
	bool saveframe;		// Used for fullscreen stuff
	BAlert *alert;
	BPMPrefs app_prefs;
};

#endif
