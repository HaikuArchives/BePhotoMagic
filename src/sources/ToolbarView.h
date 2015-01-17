
#ifndef _TOOLBAR_VIEW_H_
#define _TOOLBAR_VIEW_H_

#include <Looper.h>
#include <Window.h>
#include <View.h>
#include <Message.h>
#include <Bitmap.h>
#include <List.h>
#include <String.h>

#include "DrawButton2S.h"

#define TOOLBAR_VERTICAL 'tlbv'
#define TOOLBAR_HORIZONTAL 'tlbh'

class ToolbarView : public BView
{
public:
	ToolbarView(BRect frame, const char *name, uint32 resize, uint32 flags);
	~ToolbarView(void);
	void AttachedToWindow(void);
	virtual void MessageReceived(BMessage *msg);
	
	// UI Interface functions
	bool AddButton(const char *name,BBitmap *onbmp, BBitmap *offbmp, 
				BMessage *msg, uint32 index);
	bool AddButton(const char *name,BBitmap *onbmp, BBitmap *offbmp, 
				BMessage *msg);
	bool RemoveButton(uint32 index);
	uint32 CountButtons(void);
	DrawButton2S *FindButton(const char *name);
	void MakeEmpty(void);
	void DeselectAll(void);
	bool SetButton(const char *name,bool value);
	bool SetButton(uint32 index, bool value);
	bool FlipButton(const char *name);
	bool FlipButton(uint32 index);
	void SetLayout(uint32 style=TOOLBAR_HORIZONTAL);
	void DoLayout(void);
	void SetTarget(BLooper *looper);
	
	// data members
	BList *buttonlist;
	BRect buttonframe;
	uint32 mode;
};

#endif