#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Application.h>
#include <Screen.h>
#include <Window.h>
#include <View.h>
#include <TranslationUtils.h>
#include <Bitmap.h>
#include <ScrollView.h>
#include <ScrollBar.h>

#include "SpLocaleApp.h"

class SlideTextView : public BView
{
public:
	SlideTextView(BRect frame, long flags);
	virtual void Draw(BRect update_rect);
	virtual void Pulse();
	int32 max_posi;
	int32 posi;

	char *text;
};



class AboutView : public BView
{
public:

	AboutView(BRect frame, long flags);
	virtual	~AboutView();
	virtual void Draw(BRect update_rect);

	virtual	void MouseDown(BPoint point);
	BBitmap *title_pic;

	SlideTextView *stview;
	bool locked;
};

class AboutWindow : public BWindow
{
public:
	AboutWindow(bool locked=false);
	virtual	~AboutWindow();
	virtual void MessageReceived(BMessage *msg);
	AboutView *ab_view;
};

#endif
