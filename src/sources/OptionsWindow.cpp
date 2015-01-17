#include "OptionsWindow.h"
#include <stdio.h>

extern BWindow *bpmwindow;

OptionsWindow::OptionsWindow(BRect frame, uint32 tool)
	:	BWindow(frame, SpTranslate("Options"), B_FLOATING_WINDOW, B_NOT_MINIMIZABLE
		| B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
	brushopt=new BrushOptionsView(Bounds());
	lineopt=new LineOptionsView(Bounds());
	fillopt=new FillOptionsView(Bounds());
	rectopt=new RectOptionsView(Bounds());
	noopt=new NoOptionsView(Bounds());
	SetTool(tool);
}

OptionsWindow::~OptionsWindow(void)
{
	if(ChildAt(0) != NULL)
		RemoveChild(ChildAt(0));

	delete brushopt;
	delete lineopt;
	delete fillopt;
	delete rectopt;
	delete noopt;
	BMessage *msg=new BMessage(OPTIONSWIN_CLOSED);
	msg->AddRect("frame",Frame());
	be_app->PostMessage(msg);
}

void OptionsWindow::MessageReceived(BMessage *msg)
{
BMessage *updatemsg;
	switch(msg->what)
	{
		case UPDATE_BRUSH_TRANS:
		{	int8 tempint8=brushopt->opacity->Value();
			updatemsg=new BMessage(ALTER_BRUSH_DATA);
			updatemsg->AddInt8("opacity",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		case UPDATE_BRUSH_MODE:
		{	int8 tempint8;
			msg->FindInt8("blendmode",0,&tempint8);
			updatemsg=new BMessage(ALTER_BRUSH_DATA);
			updatemsg->AddInt8("blendmode",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		case UPDATE_LINE_TRANS:
		{	int8 tempint8=lineopt->opacity->Value();
			updatemsg=new BMessage(ALTER_LINE_DATA);
			updatemsg->AddInt8("opacity",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		case UPDATE_LINE_MODE:
		{	int8 tempint8=lineopt->mode_menu->IndexOf(lineopt->mode_menu->FindMarked());
			updatemsg=new BMessage(ALTER_LINE_DATA);
			updatemsg->AddInt8("blendmode",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		case UPDATE_LINE_STEP:
		{	int8 tempint8=lineopt->step->Value();
			updatemsg=new BMessage(ALTER_LINE_DATA);
			updatemsg->AddInt8("step",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}
		case UPDATE_TOLERANCE:
		{	int8 tempint8=fillopt->tolerance->Value();
			updatemsg=new BMessage(ALTER_TOOL_DATA);
			updatemsg->AddInt8("tolerance",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}
		case ALTER_OPTIONWIN_DATA:
			AlterOptionsWinData(msg);
			break;
		case TOOL_BRUSH:
		case TOOL_GRADIENT:
		case TOOL_FILL:
		case TOOL_LINE:
		case TOOL_SPLINE:
		case TOOL_TEXT:
		case TOOL_ZOOM:
		case TOOL_EYEDROPPER:
		case TOOL_STAMP:
		case TOOL_WAND:
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_FRECT:
		case TOOL_FELLIPSE:
		case TOOL_MASK:
		case TOOL_MOVE:
		case TOOL_ERASER:
			SetTool(msg->what);
			break;
		default:
			BWindow::MessageReceived(msg);
	}
}

void OptionsWindow::SetTool(uint32 tool)
{
	if(ChildAt(0) != NULL)
		RemoveChild(ChildAt(0));

	switch(tool)
	{
		case TOOL_RECT:
			AddChild(brushopt);
			SetTitle(SpTranslate("Options: Rectangle"));
			break;

		case TOOL_ELLIPSE:
			AddChild(brushopt);
			SetTitle(SpTranslate("Options: Ellipse"));
			break;

		case TOOL_FRECT:
			AddChild(brushopt);
			SetTitle(SpTranslate("Options: Filled Rectangle"));
			break;

		case TOOL_FELLIPSE:
			AddChild(brushopt);
			SetTitle(SpTranslate("Options: Filled Ellipse"));
			break;

		case TOOL_BRUSH:
			AddChild(brushopt);
			SetTitle(SpTranslate("Options: Paintbrush"));
			break;

		case TOOL_LINE:
			AddChild(lineopt);
			SetTitle(SpTranslate("Options: Line"));
			break;

		case TOOL_SPLINE:
			AddChild(lineopt);
			SetTitle(SpTranslate("Options: Spline"));
			break;

		case TOOL_EYEDROPPER:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Eyedropper"));
			break;
/*		case TOOL_ERASER:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Eraser"));
			break;
*/		case TOOL_FILL:
			AddChild(fillopt);
			SetTitle(SpTranslate("Options: Fill"));
			break;
		case TOOL_TEXT:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Text"));
			break;
		case TOOL_ZOOM:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Zoom"));
			break;
		case TOOL_STAMP:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Stamp"));
			break;
		case TOOL_WAND:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Wand"));
			break;
		case TOOL_MASK:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Mask"));
			break;
		case TOOL_MOVE:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Hand"));
			break;
		case TOOL_GRADIENT:
			AddChild(noopt);
			SetTitle(SpTranslate("Options: Gradient"));
			break;
		default:
			AddChild(noopt);
			SetTitle("");
			break;
	}
	currenttool=tool;
}

void OptionsWindow::AlterOptionsWinData(BMessage *msg)
{
	int32 tool, option;
	msg->FindInt32("tool",0,&tool);

	SetTool(tool);
	
	switch(tool)
	{
		case TOOL_BRUSH:
			msg->FindInt32("mode",0,&option);
			brushopt->mode_menu->ItemAt(option)->SetMarked(true);
			msg->FindInt32("opacity",0,&option);
			brushopt->opacity->SetValue(option);
			break;
		case TOOL_LINE:
			break;
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_FRECT:
		case TOOL_FELLIPSE:
		case TOOL_SPLINE:
		case TOOL_ERASER:
		case TOOL_FILL:
		case TOOL_TEXT:
//		case TOOL_ZOOM:
		case TOOL_STAMP:
		case TOOL_WAND:
		case TOOL_MASK:
		case TOOL_MOVE:
		case TOOL_GRADIENT:
		default:
			break;
	}
}
#ifndef CHILDWIN_MOVED
	#define CHILDWIN_MOVED 'cwmv'
#endif

void OptionsWindow::FrameMoved(BPoint origin)
{
	BMessage *msg=new BMessage(CHILDWIN_MOVED);
	msg->AddPoint("optionswin",origin);
	be_app->PostMessage(msg);
}

BrushOptionsView::BrushOptionsView(BRect frame) : 
	BView(frame, "brush_options", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
SetViewColor(216,216,216);

BRect rect;

BMenuItem *it;
mode_menu = new BMenu(SpTranslate("Paint Mode"));

BMessage *a = new BMessage(UPDATE_BRUSH_MODE); a->AddInt8("blendmode",TNORMAL);
mode_menu->AddItem(it = new BMenuItem(SpTranslate("Normal"), a));

BMessage *b = new BMessage(UPDATE_BRUSH_MODE); b->AddInt8("blendmode",TMULTIPLY);
mode_menu->AddItem(new BMenuItem(SpTranslate("Multiply"),  b));

BMessage *m = new BMessage(UPDATE_BRUSH_MODE); m->AddInt8("blendmode",TDIVIDE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Divide"),    m));

BMessage *f = new BMessage(UPDATE_BRUSH_MODE); f->AddInt8("blendmode",TDIFFERENCE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Difference"),f));

BMessage *c = new BMessage(UPDATE_BRUSH_MODE); c->AddInt8("blendmode",TEXCLUSION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Exclusion"),   c));

BMessage *d = new BMessage(UPDATE_BRUSH_MODE); d->AddInt8("blendmode",TLIGHTEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Lighten"),    d));

BMessage *g = new BMessage(UPDATE_BRUSH_MODE); g->AddInt8("blendmode",TDARKEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Darken"),    g));

BMessage *h = new BMessage(UPDATE_BRUSH_MODE); h->AddInt8("blendmode",TOR);
mode_menu->AddItem(new BMenuItem(SpTranslate("OR"),    h));

BMessage *i = new BMessage(UPDATE_BRUSH_MODE); i->AddInt8("blendmode",TAVERAGE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Average"),    i));

BMessage *j = new BMessage(UPDATE_BRUSH_MODE); j->AddInt8("blendmode",TADDITION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Addition"),    j));

BMessage *k = new BMessage(UPDATE_BRUSH_MODE); k->AddInt8("blendmode",TSUBTRACT);
mode_menu->AddItem(new BMenuItem(SpTranslate("Subtract"),    k));

//rect.Set(5,5,85,30);
//rect.OffsetBy(Bounds().right-rect.right,0);

rect.Set(4,12,74,24);
mode = new BMenuField(rect,"",NULL,mode_menu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW | B_NAVIGABLE);
mode_menu->SetLabelFromMarked(true);
mode_menu->ItemAt(0)->SetMarked(true);
AddChild(mode);

/*
rect = Bounds();
rect.bottom = 36;
rect.right -= 5;
rect.OffsetBy(0,mode->Bounds().Height());
*/
rect.OffsetBy(75,0);
rect.right=Bounds().right-10;
opacity = new TSlider(rect,"brush_opacity","Opacity","%d",new BMessage(UPDATE_BRUSH_TRANS),1,255);
opacity->SetValue(255.0);

rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
opacity->SetBarColor(a_col);
opacity->UseFillColor(true,&a_colb);
opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
opacity->SetHashMarkCount(10+1);
AddChild (opacity);

}
void BrushOptionsView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		default:
			BView::MessageReceived(msg);
	}
}

void BrushOptionsView::Draw(BRect r)
{
	DrawString(SpTranslate("Blending Mode:"),BPoint(5,9));
}

void BrushOptionsView::AttachedToWindow(void)
{
	mode_menu->SetTargetForItems(Window());
	opacity->SetTarget(Window());
}

LineOptionsView::LineOptionsView(BRect frame) : 
	BView(frame, "line_options",  B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
SetViewColor(216,216,216);

BRect rect;

BMenuItem *it;
mode_menu = new BMenu(SpTranslate("Paint Mode"));

BMessage *a = new BMessage(UPDATE_BRUSH_MODE); a->AddInt8("blendmode",TNORMAL);
mode_menu->AddItem(it = new BMenuItem(SpTranslate("Normal"), a));

BMessage *b = new BMessage(UPDATE_BRUSH_MODE); b->AddInt8("blendmode",TMULTIPLY);
mode_menu->AddItem(new BMenuItem(SpTranslate("Multiply"),  b));

BMessage *m = new BMessage(UPDATE_BRUSH_MODE); m->AddInt8("blendmode",TSUBTRACT);
mode_menu->AddItem(new BMenuItem(SpTranslate("Divide"),    m));

BMessage *f = new BMessage(UPDATE_BRUSH_MODE); f->AddInt8("blendmode",TDIFFERENCE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Difference"),f));

BMessage *c = new BMessage(UPDATE_BRUSH_MODE); c->AddInt8("blendmode",TEXCLUSION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Exclusion"),   c));

BMessage *d = new BMessage(UPDATE_BRUSH_MODE); d->AddInt8("blendmode",TLIGHTEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Lighten"),    d));

BMessage *g = new BMessage(UPDATE_BRUSH_MODE); g->AddInt8("blendmode",TDARKEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Darken"),    g));

BMessage *h = new BMessage(UPDATE_BRUSH_MODE); h->AddInt8("blendmode",TOR);
mode_menu->AddItem(new BMenuItem(SpTranslate("OR"),    h));

BMessage *i = new BMessage(UPDATE_BRUSH_MODE); i->AddInt8("blendmode",TAVERAGE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Average"),    i));

BMessage *j = new BMessage(UPDATE_BRUSH_MODE); j->AddInt8("blendmode",TADDITION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Addition"),    j));

BMessage *k = new BMessage(UPDATE_BRUSH_MODE); k->AddInt8("blendmode",TSUBTRACT);
mode_menu->AddItem(new BMenuItem(SpTranslate("Subtract"),    k));

//rect.Set(5,5,85,30);
//rect.OffsetBy(Bounds().right-rect.right,0);

rect.Set(4,12,74,24);
mode = new BMenuField(rect,"",NULL,mode_menu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW | B_NAVIGABLE);
mode_menu->SetLabelFromMarked(true);
mode_menu->ItemAt(0)->SetMarked(true);
mode_menu->SetTargetForItems(this);
AddChild(mode);

/*
rect = Bounds();
rect.bottom = 36;
rect.right -= 5;
rect.OffsetBy(0,mode->Bounds().Height());
*/
rect.OffsetBy(75,0);
rect.right=Bounds().right-10;
opacity = new TSlider(rect,"line_opacity",SpTranslate("Opacity"),"%d",new BMessage(UPDATE_LINE_TRANS),1,255);
opacity->SetValue(255.0);

rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
opacity->SetBarColor(a_col);
opacity->UseFillColor(true,&a_colb);
opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
opacity->SetHashMarkCount(10+1);
AddChild (opacity);

rect.OffsetBy(0,40);
step = new TSlider(rect,"line_step",SpTranslate("Spacing"),"%d",new BMessage(UPDATE_LINE_STEP),1,50);
step->SetValue(1.0);
step->SetBarColor(a_col);
step->UseFillColor(true,&a_colb);
step->SetHashMarks(B_HASH_MARKS_BOTTOM); 
step->SetHashMarkCount(10+1);
AddChild (step);

}

void LineOptionsView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		case UPDATE_LINE_TRANS:
		case UPDATE_LINE_STEP:
		case UPDATE_LINE_MODE:
			Window()->PostMessage(msg);
			break;
		default:
			BView::MessageReceived(msg);
	}
}

void LineOptionsView::Draw(BRect r)
{
	DrawString(SpTranslate("Blending Mode:"),BPoint(5,9));
}

void LineOptionsView::AttachedToWindow(void)
{
	mode_menu->SetTargetForItems(Window());
	step->SetTarget(Window());
	opacity->SetTarget(Window());
}

FillOptionsView::FillOptionsView(BRect frame) : 
	BView(frame, "fill_options",  B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
SetViewColor(216,216,216);

BRect rect= Bounds();
rect.bottom = 36;
rect.right -= 5;
tolerance = new TSlider(rect,"fill_tolerance","Tolerance","%d",new BMessage(UPDATE_TOLERANCE),0,255);
tolerance->SetValue(0.0);

rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
tolerance->SetBarColor(a_col);
tolerance->UseFillColor(true,&a_colb);
tolerance->SetHashMarks(B_HASH_MARKS_BOTTOM); 
tolerance->SetHashMarkCount(10+1);
AddChild (tolerance);
}

void FillOptionsView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		default:
			BView::MessageReceived(msg);
	}
}

void FillOptionsView::Draw(BRect r)
{
}

void FillOptionsView::AttachedToWindow(void)
{
	tolerance->SetTarget(Window());
}

RectOptionsView::RectOptionsView(BRect frame) : 
	BView(frame, "rect_options", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
SetViewColor(216,216,216);

BRect rect;

BMenuItem *it;
mode_menu = new BMenu(SpTranslate("Paint Mode"));

BMessage *a = new BMessage(UPDATE_BRUSH_MODE); a->AddInt8("blendmode",TNORMAL);
mode_menu->AddItem(it = new BMenuItem(SpTranslate("Normal"), a));

BMessage *b = new BMessage(UPDATE_BRUSH_MODE); b->AddInt8("blendmode",TMULTIPLY);
mode_menu->AddItem(new BMenuItem(SpTranslate("Multiply"),  b));

BMessage *m = new BMessage(UPDATE_BRUSH_MODE); m->AddInt8("blendmode",TSUBTRACT);
mode_menu->AddItem(new BMenuItem(SpTranslate("Divide"),    m));

BMessage *f = new BMessage(UPDATE_BRUSH_MODE); f->AddInt8("blendmode",TDIFFERENCE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Difference"),f));

BMessage *c = new BMessage(UPDATE_BRUSH_MODE); c->AddInt8("blendmode",TEXCLUSION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Exclusion"),   c));

BMessage *d = new BMessage(UPDATE_BRUSH_MODE); d->AddInt8("blendmode",TLIGHTEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Lighten"),    d));

BMessage *g = new BMessage(UPDATE_BRUSH_MODE); g->AddInt8("blendmode",TDARKEN);
mode_menu->AddItem(new BMenuItem(SpTranslate("Darken"),    g));

BMessage *h = new BMessage(UPDATE_BRUSH_MODE); h->AddInt8("blendmode",TOR);
mode_menu->AddItem(new BMenuItem(SpTranslate("OR"),    h));

BMessage *i = new BMessage(UPDATE_BRUSH_MODE); i->AddInt8("blendmode",TAVERAGE);
mode_menu->AddItem(new BMenuItem(SpTranslate("Average"),    i));

BMessage *j = new BMessage(UPDATE_BRUSH_MODE); j->AddInt8("blendmode",TADDITION);
mode_menu->AddItem(new BMenuItem(SpTranslate("Addition"),    j));

BMessage *k = new BMessage(UPDATE_BRUSH_MODE); k->AddInt8("blendmode",TSUBTRACT);
mode_menu->AddItem(new BMenuItem(SpTranslate("Subtract"),    k));

rect.Set(5,5,85,30);
rect.OffsetBy(Bounds().right-rect.right,0);
mode = new BMenuField(rect,"",NULL,mode_menu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW | B_NAVIGABLE);
mode_menu->SetLabelFromMarked(true);
mode_menu->ItemAt(0)->SetMarked(true);
mode_menu->SetTargetForItems(this);
AddChild(mode);

rect = Bounds();
rect.bottom = 36;
rect.right -= 5;
rect.OffsetBy(0,mode->Bounds().Height());
opacity = new TSlider(rect,"rect_opacity",SpTranslate("Opacity"),"%d",new BMessage(UPDATE_LINE_TRANS),1,255);
opacity->SetValue(255.0);

rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
opacity->SetBarColor(a_col);
opacity->UseFillColor(true,&a_colb);
opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
opacity->SetHashMarkCount(10+1);
AddChild (opacity);

rect.OffsetBy(0,40);
step = new TSlider(rect,"rect_step",SpTranslate("Spacing"),"%d",new BMessage(UPDATE_LINE_STEP),1,50);
step->SetValue(1.0);
step->SetBarColor(a_col);
step->UseFillColor(true,&a_colb);
step->SetHashMarks(B_HASH_MARKS_BOTTOM); 
step->SetHashMarkCount(10+1);
AddChild (step);

}

void RectOptionsView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{	
		case UPDATE_LINE_TRANS:
		case UPDATE_LINE_STEP:
		case UPDATE_LINE_MODE:
			break;
		default:
			BView::MessageReceived(msg);
	}
}

void RectOptionsView::Draw(BRect r)
{
	DrawString(SpTranslate("Blending Mode:"),BPoint(5,9));
}

void RectOptionsView::AttachedToWindow(void)
{
	mode_menu->SetTargetForItems(Window());
	step->SetTarget(Window());
	opacity->SetTarget(Window());
}

NoOptionsView::NoOptionsView(BRect frame) :
	BView(frame, "no_options", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
SetViewColor(216,216,216);
}

void NoOptionsView::Draw(BRect update_rect)
{
	DrawString(SpTranslate("No options for this tool"),BPoint(10,20));
}
