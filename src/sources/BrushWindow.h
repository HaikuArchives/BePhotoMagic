#ifndef BRUSH_WINDOW_H
#define BRUSH_WINDOW_H

#include <Invoker.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <ScrollView.h>
#include <TabView.h>
#include <Bitmap.h>
#include <PictureButton.h>
#include <Picture.h>
#include <Path.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <TranslatorRoster.h>
#include <stdio.h>
#include <string.h>
#include <String.h>

#include "SpLocaleApp.h"
#include "PBitmapModif.h"
#include "BrushView.h"
#include "Utils.h"

#ifndef CHILDWIN_MOVED
	#define CHILDWIN_MOVED 'cwmv'
#endif

#define TOGGLE_BRUSH_WINDOW 'tgbw'
#define BRUSHWIN_CLOSED 'bwcl'
#define SELECT_USER_BRUSH 'slub'
#define SELECT_BRUSH 'sldb'
#define BRUSHDIR_SET 'bdst'

// Duplicated for BPMMain
#ifndef GET_APP_DIR
	#define GET_APP_DIR	'gapd'
#endif

class ThumbButton : public BView
{
public:
	ThumbButton(BRect frame, const char *name, int32 msg, entry_ref entry,
				BBitmap *bmp, int32 resize, int32 flags);
	~ThumbButton(void);

	void Draw(BRect update);
	void MouseUp(BPoint where);
	void AttachedToWindow(void);

	BBitmap *RescaleBitmap(BBitmap *src, BRect outputsize, bool center_if_smaller);
	BBitmap *display;
	BLooper *target;
	int32 msgcmd;
	entry_ref ref;
	BInvoker *msgsender;
};

class AllBrushesView : public BView 
{
public:
	AllBrushesView(BRect r);
	~AllBrushesView(void);
	void AddBrushes(void);
//	BBitmap *LoadBitmap(char *filename);
	BPath brushdir;
	ThumbButton *userbrushes[255];
	int32 last_brush;
};


class BrushWindow : public BWindow 
{
public:
	BrushWindow(BRect frame, char *title, int16 bwidth=16,int16 bheight=16, int8 blur=0, int8 btype=0);
	virtual ~BrushWindow();

	BrushSelectorView *dview;
	AllBrushesView *allbrushes;
	BScrollView *scrollview;
	BTabView *tabs;

	virtual void FrameMoved(BPoint origin);
	virtual void FrameResized(float x, float y);
	virtual void MessageReceived(BMessage *msg);
};

extern BWindow *bpmwindow;
#endif