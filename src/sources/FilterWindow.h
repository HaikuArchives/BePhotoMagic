#ifndef _FILTERWINDOW_H_

#define _FILTERWINDOW_H_

#include <Message.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <Invoker.h>	// Needed for Preview updates
#include <string.h>

#include "SpLocaleApp.h"
#include "ImageManip.h"
#include "BBitmapAccessor.h"
#include "Utils.h"

// the lone external message
#define FILTERED_BITMAP 'fbmp'
#define FLATTENED_BITMAP 'flbm'

#ifndef SHOW_WAIT
	#define SHOW_WAIT 'shww'
	#define CLOSE_WAIT 'hdww'
#endif

class PreView : public BView
{
public:
	PreView(BRect frame,const char *name,uint32 resize, uint32 flags);
	~PreView(void);
	void Draw(BRect update_rect);
	void MouseDown(BPoint pt);
	void MouseMoved(BPoint pt,uint32 transit,BMessage *msg);
	
	void SetBitmap(BBitmap *bmp);
	void DisableBitmap(void);
	void SetDisplay(BRect rect);
	void SetZoom(bool zoomin);
	BRect view,boundsrect,unzoomed;
	BBitmap *display;
	bool use_bitmap;
	float zoom;
	BPoint oldpt,newpt;
};

class FilterWindow: public BWindow
{
public:
	FilterWindow(int32 idfilter,bool convert);
	~FilterWindow(void);
	void MessageReceived(BMessage *msg);
	void AllAttached(void);
	BBitmap *CreatePreview(BBitmap *source);
	BBitmap *MergeSelection(BBitmap *bitmap,BBitmap *selection);

	BView *configview;
	PreView *preview;
	BView *desktop;
	BInvoker *preview_invoker;

	BButton *ok,*cancel,*about,*btpreview,*zoomin,*zoomout;

	image_addon_id	plugin_id;
	BBitmap *workbmp,		// flattens selection and bitmap into one
			*bmp,*select,	// data passed to filter from app
			*preview_bmp;	// scaled bitmap specifically for fast previews
	BMessage pluginconfig;
	bool uses_view;
	const char *plugin_name,*plugin_info,*plugin_category;
	int32 plugin_version;
	float scale;

};

#endif