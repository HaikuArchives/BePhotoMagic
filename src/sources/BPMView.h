
#ifndef BPM_VIEW_H

#define BPM_VIEW_H

#include <Application.h>
#include <Message.h>
#include <Window.h>
#include <View.h>
#include <TranslationUtils.h>
#include <string.h>

#include "BPMImage.h"
#include "LayerWindowMsgs.h"
#include "AppCursor.h"
#include "Utils.h"

// More redefined definitions from OptionWindow
#ifndef ALTER_OPTION_DATA
	#define ALTER_OPTIONWIN_DATA 'aowd'
#endif

// Stolen from FilterWindow
#ifndef FILTERED_BITMAP
	#define FILTERED_BITMAP 'fbmp'
#endif

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

#define SCREEN_FULL_NONE	'sfnn'
#define SCREEN_FULL_HALF	'sfhf'
#define SCREEN_FULL_ALL		'sfal'

#define GRADIENT_LINEAR 0
#define GRADIENT_RADIAL 1
#define GRADIENT_RECTANGULAR 2

#endif

// View-specific message defines
#define CENTER_VIEW 'cnvw'

#ifndef SHOW_WAIT
	#define SHOW_WAIT 'shww'
	#define CLOSE_WAIT 'hdww'
#endif

class PicView : public BView 
{
public:
	PicView(BRect r, long flags);
	void AttachedToWindow(void);
	virtual	~PicView();

	// UI functions
	virtual void MouseDown(BPoint where);
	virtual void MouseUp(BPoint where);
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage *dragdrop);
	virtual void Draw(BRect update_rect);
	virtual void Draw(void);
	virtual void MessageReceived(BMessage *msg);

	// Data-related functions
	bool CreateNewImage(const char *namestr, uint16 imgwidth, uint16 imgheight);
	bool CreateNewImageFromBitmap(const char *namestr,BBitmap *bmp);
	bool CloseActiveImage(void);
	void UpdateLayerWindow(void);
	void UpdateOptionWindow(void);
	void ClipboardCopy(void);
	void ClipboardPaste(void);
	void ClipboardCut(void);
	void ClipboardPasteAsNew(void);
	void RadialGradient(BPoint startpt,BPoint endpt);
	void RectGradient(BPoint startpt,BPoint endpt);
	
	// Tool functions
	void SetZoom(bool mode);
	static int32 ThreadUpdater(void *data);
	
	 // exists only for a while...
//	void ApplyBeGraphics(BPoint oldpt, BPoint newpt, bool fore=true);
	
	// Data
	BPMImage *images[255];
	uint8 active_image;
	BPMImage *p_active_image;
	
	int32 updaterID,
		stamp_xoffset,stamp_yoffset,
		currenttool,previoustool;
	BRect stamp_rect;
	uint8 openimages;
	BPoint old_mouse_pos,new_mouse_pos;
	BRect old_preview_frame,extents;
//	float zoom;
	
	bool mousedown;
	bool foremode;
	bool zoomin;
	uint8 tool_blendmode;
	uint8 tool_alpha;
	uint8 gradientmode;
	BBitmap *backbitmap;
};

class DesktopView : public BView
{
public:
	DesktopView(BRect rect, PicView *child);
	~DesktopView(void);
	void AttachedToWindow(void);
	void MouseUp(BPoint where);
	void MouseDown(BPoint where);
	
	PicView *childview;
};
#endif
