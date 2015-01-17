#ifndef BPM_MAIN_H

#define BPM_MAIN_H

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Alert.h>
#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <NodeInfo.h>
#include <Path.h>
#include <TranslatorRoster.h>
#include <Screen.h>

#include <stdio.h>
#include <string.h>
#include <String.h>

#include "SpLocaleApp.h"
#include "yprefs.h"
#include "Utils.h"

#include "BPMPrefs.h"
#include "WaitView.h"
#include "TextToolWindow.h"
#include "FilterWindow.h"
#include "LayerWindow.h"
#include "LayerWindowMsgs.h"
#include "ToolWindow.h"
#include "BrushWindow.h"
#include "ColorWindow.h"
#include "OptionsWindow.h"
#include "BPMWindow.h"
#include "MouseCam.h"

// redundant definition taken from CreateWindow.h
// just so we don't have to include the whole stupid file
#ifndef CREATE_IMAGE
	#define CREATE_IMAGE	'cwci'
#endif

#ifndef UPDATE_BRUSH
	#define UPDATE_BRUSH	'upbr'
#endif

// redundant definition taken from SavePanel.h
// just so we don't have to include the whole stupid file
#ifndef BPM_SAVE_REF
	#define BPM_SAVE_REF	'bsrf'
#endif

//class BPMApplication : public BApplication 
class BPMApplication : public SpLocaleApp
{
public:
	BPMApplication(void);
	~BPMApplication(void);
	virtual void RefsReceived(BMessage *message);
	virtual void ArgvReceived(int32 argc, char **argv);
	void MessageReceived(BMessage *msg);

	// App-specific methods	
	void Shortcuts(BMessage *msg);
	void SetColor(bool fore=true, bool fromimage=false);
	void UpdateBrush(void);
	void SetUserBrush(BBitmap *brush);
	void SetBrush(BMessage *msg);
	void TranslatorExport(BMessage *msg);
	void ShowWait(void);
	void CloseWait(void);
	void ShortcutToolbarUpdate(void);
	
	BPMWindow	*mainwin;
	ToolWindow *toolwin;
//	LayerWindow *layerwin;
	BrushWindow *brushwin;
	ColorWindow *forewin,*backwin;
	OptionsWindow *optionswin;
	AboutWindow *aboutwin;
	FilterWindow *filterwin;
	TextToolWindow *textwin;
	BWindow *waitwin;
	WaitView *waitview;
	
//	void LoadPrefs(void);
//	void SavePrefs(void);
//	yprefs prefs;
	BPMPrefs *app_prefs;

	// Window State data - perhaps later to be stashed in preferences
	bool toolwin_open, layerwin_open, brushwin_open, backwin_open, forewin_open;
	bool optionswin_open,filterwin_open,textwin_open, waitwin_open;
	BRect mainwin_frame, toolwin_frame, layerwin_frame, brushwin_frame;
	BRect optionswin_frame;
	BPoint backpoint,forepoint;
	int32 default_type;		// default translator typecode
	BString default_mime;	// mime signature of default translator
};

#endif
