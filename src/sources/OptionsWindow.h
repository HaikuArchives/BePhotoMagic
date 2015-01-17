#ifndef _OPTIONWIN_H_

#define _OPTIONWIN_H_

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <MenuField.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Message.h>

#include "SpLocaleApp.h"
#include "TSlider.h"
#include "TransModes.h"

#ifndef TOOL_BRUSH
// Stolen from ToolWindow message defs
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

#define GRADIENT_LINEAR 0
#define GRADIENT_RADIAL 1
#define GRADIENT_RECTANGULAR 2

#define SCREEN_FULL_NONE	'sfnn'
#define SCREEN_FULL_HALF	'sfhf'
#define SCREEN_FULL_ALL		'sfal'

#endif

#define TOGGLE_OPTIONS_WINDOW 'tgow'
#define	OPTIONSWIN_CLOSED 'owcl'

// External messages
#define UPDATE_OPTIONS 'upop'
#define ALTER_OPTIONWIN_DATA 'aowd'
#define ALTER_BRUSH_DATA 'albd'
#define ALTER_LINE_DATA 'alld'
#define ALTER_TOOL_DATA 'altd'	// used for one-value options views, like fill

// Internal messages
#define UPDATE_BRUSH_TRANS 'upbt'
#define UPDATE_BRUSH_MODE 'upbm'
#define UPDATE_LINE_TRANS 'uplt'
#define UPDATE_LINE_MODE 'uplm'
#define UPDATE_LINE_STEP 'upls'
#define UPDATE_TOLERANCE 'upft'

class BrushOptionsView : public BView 
{
public:
	BrushOptionsView(BRect frame);
	void MessageReceived(BMessage *msg);
	void AttachedToWindow(void);
	void Draw(BRect r);
	
	BMenuField		*mode;		// blending mode for brush
	BMenu *mode_menu;
	TSlider			*opacity;	
};

class LineOptionsView : public BView 
{
public:
	LineOptionsView(BRect frame);
	void MessageReceived(BMessage *msg);
	void AttachedToWindow(void);
	void Draw(BRect r);
	
	BMenuField		*mode;		// blending mode for brush
	BMenu *mode_menu;
	TSlider			*opacity;
	TSlider			*step;
};

class RectOptionsView : public BView 
{
public:
	RectOptionsView(BRect frame);
	void AttachedToWindow(void);
	void MessageReceived(BMessage *msg);
	void Draw(BRect r);
	
	BMenuField		*mode;		// blending mode for brush
	BMenu *mode_menu;
	TSlider			*opacity;
	TSlider			*step;
};

class FillOptionsView : public BView 
{
public:
	FillOptionsView(BRect frame);
	void AttachedToWindow(void);
	void MessageReceived(BMessage *msg);
	void Draw(BRect r);
	
	TSlider			*tolerance;	
};


class NoOptionsView : public BView 
{
public:
	NoOptionsView(BRect frame);
	void Draw(BRect update_rect);
};

class OptionsWindow : public BWindow
{
public:
	OptionsWindow(BRect frame,uint32 tool);
	~OptionsWindow(void);

	virtual void MessageReceived(BMessage *msg);
	virtual void FrameMoved(BPoint origin);

	BrushOptionsView *brushopt;
	LineOptionsView *lineopt;
	FillOptionsView *fillopt;
	RectOptionsView *rectopt;
	NoOptionsView *noopt;

	void SetTool(uint32 tool);
	void AlterOptionsWinData(BMessage *msg);
	uint32 currenttool;
};
#endif