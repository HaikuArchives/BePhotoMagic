#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Invoker.h>
#include <Slider.h>
#include <CheckBox.h>
#include <StringView.h>
#include <ListView.h>
#include <ListItem.h>
#include <ScrollView.h>
#include <Button.h>
#include <TextControl.h> 
#include <Font.h>
#include <Alert.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "SpLocaleApp.h"
#include "Utils.h"

#ifndef TEXT_TOOL_VIEW

#define TEXT_TOOL_VIEW

// lone external message
#define RENDER_TEXT 'ttrt'

// internal message defines
#define OUTLINE_CHANGED 'ttol'
#define SIZE_CHANGED 'ttsz'
#define SHEAR_CHANGED 'ttsh'
#define ROTATION_CHANGED 'ttro'
#define SPACING_CHANGED 'ttsp'
#define ANTIALIAS_TEXT 'ttaa'
#define TEXT_CHANGED 'tttc'
#define FONT_FAMILY_CHANGED 'ttff'
#define FONT_STYLE_CHANGED 'ttfs'
#define FONTVIEW_TOGGLE 'tfvt'


class FontDrawView : public BView
{
public:
	FontDrawView(BRect rect, char *fonttext);
	~FontDrawView(void);
	
	void Draw(BRect r);
	void CheckStringPlacement(void);
	virtual void MessageReceived(BMessage *message);
	virtual void MouseDown(BPoint where);
	virtual void MouseMoved(BPoint where,uint32 trans,const BMessage *drag);
	char text[255];
	BFont currentfont;
	BPoint fontlocation,oldpoint;
	BRect bbox;			// BRect containing the string's bounding box
	BPoint ul,ll,ur,lr;	// Points of string's bounding box
	float ascent,descent;
};

class FontListView : public BListView
{
public:
	FontListView(BRect rect, const char *name, BHandler *target, BMessage *message);
	~FontListView(void);
	virtual void SelectionChanged();
	BHandler *recipient;
	BInvoker *fontinvoker;
};

class TextToolView : public BView
{
public:
	TextToolView(BRect rect);
	~TextToolView(void);

	virtual void MessageReceived(BMessage *message);
	virtual void AttachedToWindow(void); 
	virtual void Draw(BRect rect);

	// Controls for font config

	BTextControl *textedit;	
	BListView *fontlist;
	BListView *stylelist;
	BScrollView *fontscrollview;
	FontDrawView *fontview;

	BSlider *outlineslider;
	BSlider *sizeslider;
	BSlider *shearslider;
	BSlider *rotationslider;
	BSlider *spacingslider;

	BButton *renderbutton;
	BCheckBox *aacheckbox;
	BButton *fontviewtoggle;
};

#endif
