#ifndef _TOOLWIN_H_
#define _TOOLWIN_H_

#include <Looper.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Path.h>
#include <String.h>
#include <TranslationUtils.h>
#include <stdio.h>

#include "DrawButton.h"
#include "ToolbarView.h"
#include "Utils.h"
#include "SpLocaleApp.h"

//#ifndef TOOL_BRUSH
#define TOGGLE_TOOL_WINDOW 'tgtw'
#define TOOLWIN_CLOSED 'tlwc'
#define TOGGLE_BACKCOLOR 'tgbc'
#define TOGGLE_FORECOLOR 'tgfc'
#define TOGGLE_BRUSH_WINDOW 'tgbw'

#define TOOL_BRUSH	'tbsh'
#define TOOL_FILL	'tfil'
#define TOOL_LINE	'tlin'
#define TOOL_SPLINE	'tspl'
#define TOOL_RECT	'trct'
#define TOOL_ELLIPSE	'tell'
#define TOOL_TEXT	'ttxt'
#define TOOL_ZOOM	'tzom'
#define TOOL_EYEDROPPER	'teye'
#define TOOL_STAMP	'tstm'
#define TOOL_WAND	'twnd'
#define TOOL_FRECT	'tfrc'
#define TOOL_FELLIPSE	'tfel'
#define TOOL_MASK	'tmsk'
#define TOOL_MOVE	'tmov'
#define TOOL_ERASER	'ters'
#define TOOL_GRADIENT 'tgrd'

#define SCREEN_FULL_NONE	'sfnn'
#define SCREEN_FULL_HALF	'sfhf'
#define SCREEN_FULL_ALL		'sfal'

#define GRADIENT_LINEAR 0
#define GRADIENT_RADIAL 1
#define GRADIENT_RECTANGULAR 2

//#endif

// "Borrowed" from BrushView.h
#ifndef BRUSH_TYPE_ELLIPSE
	#define BRUSH_TYPE_ELLIPSE	0
	#define BRUSH_TYPE_RECT		1
#endif

#ifndef TEXTWIN_CLOSED
	#define TEXTWIN_CLOSED 'txwc'
#endif

class ToolWindow : public BWindow 
{
public:
	ToolWindow(BRect frame,bool ismaskmode);
	~ToolWindow();
//	bool QuitRequested();

	void MessageReceived(BMessage *msg);
	virtual void FrameMoved(BPoint origin);

	BView *toolview;
	ToolbarView *toolbara,*toolbarb,*toolbarc,*fullscreen;
	
	DrawButton *forebutton,*backbutton;
	DrawButton *brushview;
	BBitmap *brushbitmap;
	
	rgb_color GetColor(bool fore=true);
	void SetColor(rgb_color color, bool fore=true);
	bool LoadButton(ToolbarView *toolbar,const char *name, const char *onname,
		const char *offname, BMessage *msg);
	void SelectTool(int32 tool,bool value);
	
	bool maskmode;
	int32 currenttool;
   	BPath *icondir;
};

extern BWindow *bpmwindow;

#endif