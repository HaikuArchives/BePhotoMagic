#include "ToolbarView.h"
#include <stdio.h>		// just for printf debugging

ToolbarView::ToolbarView(BRect frame, const char *name, uint32 resize, uint32 flags)
	 : BView(frame,name,resize,flags)
{
	buttonlist=new BList();
	SetLayout();
}

ToolbarView::~ToolbarView(void)
{
	MakeEmpty();
}

void ToolbarView::AttachedToWindow(void)
{
}

void ToolbarView::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		default:
			BView::MessageReceived(msg);
	}
}

bool ToolbarView::AddButton(const char *name,BBitmap *onbmp, BBitmap *offbmp, 
				BMessage *msg, uint32 index)
{
	DrawButton2S *button;
	
	// Validity check
	if(onbmp==NULL || offbmp==NULL || name==NULL || msg==NULL)
		return false;
	
	if(buttonlist->CountItems()==0)
		buttonframe=onbmp->Bounds();
		
	button=new DrawButton2S(onbmp->Bounds(),name,msg->what,
						B_FOLLOW_NONE,B_WILL_DRAW|B_NAVIGABLE);
	if(button==NULL)
		return false;
	button->SetBitmaps(onbmp,offbmp);
	buttonlist->AddItem((void*)button,index);
	AddChild(button);
	
	// We take ownership of the passed message.
	// I feel like breaking something.... ;^)
	delete msg;
	
	DoLayout();
	
	return true;
}

bool ToolbarView::AddButton(const char *name,BBitmap *onbmp, BBitmap *offbmp, 
				BMessage *msg)
{
	DrawButton2S *button;
	
	// Validity check
	if(onbmp==NULL || offbmp==NULL || name==NULL || msg==NULL)
		return false;
	
	if(buttonlist->CountItems()==0)
		buttonframe=onbmp->Bounds();
		
	button=new DrawButton2S(onbmp->Bounds(),name,msg->what,
						B_FOLLOW_NONE,B_WILL_DRAW|B_NAVIGABLE);
	if(button==NULL)
		return false;
	button->SetBitmaps(onbmp,offbmp);
	buttonlist->AddItem((void*)button);
	AddChild(button);
	
	// We take ownership of the passed message.
	// I feel like breaking something.... ;^)
	delete msg;
	
	DoLayout();
	
	return true;
}

bool ToolbarView::RemoveButton(uint32 index)
{
	DrawButton2S *button=(DrawButton2S *)buttonlist->RemoveItem(index);
	if(button==NULL)
		return false;
	else
	{	delete button;
		return true;
	}
}

uint32 ToolbarView::CountButtons(void)
{
	return (uint32)buttonlist->CountItems();
}

DrawButton2S * ToolbarView::FindButton(const char *name)
{	// This will only return the first item matching the name

	DrawButton2S *button;
	if(name==NULL)
		return NULL;

	BString str;
	
	for(int32 i=0;i<buttonlist->CountItems();i++)
	{
		button=(DrawButton2S *)buttonlist->ItemAt(i);
		if(button==NULL)
			continue;
		str=button->Name();
		if(str.Compare(name)==0)
			return button;
	}
	return NULL;
}

void ToolbarView::MakeEmpty(void)
{
	DrawButton2S *button;
	
	// Empty the list
	if(buttonlist->CountItems()!=0)
	{	int32 i;
		for(i=0;i<buttonlist->CountItems();i++)
		{	button=(DrawButton2S *)buttonlist->RemoveItem(i);
			if(button==NULL)
				continue;
			button->RemoveSelf();
			delete button;
		}
	}
}

void ToolbarView::SetLayout(uint32 style=TOOLBAR_HORIZONTAL)
{
	mode=style;
}

void ToolbarView::DoLayout(void)
{
	// Sets up the button frames for drawing
	// Toolbar is only one row/column, but unlimited in its moded direction
	// (ie TOOLBAR_HORIZONTAL=1 row,TOOLBAR_VERTICAL=1 column

	uint32 max=buttonlist->CountItems(),
		offset,i;
	DrawButton2S *button;

	if(mode==TOOLBAR_HORIZONTAL)
	{
		offset=buttonframe.IntegerWidth()+1;
		ResizeTo(offset*max,buttonframe.Height());
		for(i=0;i<max;i++)
		{
			button=(DrawButton2S *)buttonlist->ItemAt(i);
			button->MoveTo(i*offset,0);
		}
	}
	else
	{
		offset=buttonframe.IntegerHeight()+1;
		ResizeTo(buttonframe.Width(),offset*max);
		for(i=0;i<max;i++)
		{
			button=(DrawButton2S *)buttonlist->ItemAt(i);
			button->MoveTo(0,i*offset);
		}
	}
}

void ToolbarView::DeselectAll(void)
{
	DrawButton2S *button;
	
	for(int32 i=0;i<buttonlist->CountItems();i++)
	{
		button=(DrawButton2S *)buttonlist->ItemAt(i);
		if(button==NULL)
			continue;
		if(button->Value()==true)
			button->SetValue(false);
	}
}

bool ToolbarView::SetButton(const char *name,bool value)
{	// Allows setting a button by name
	if(name==NULL)
		return false;
		
	DrawButton2S *button=FindButton(name);
	if(button==NULL)
		return false;

	button->SetValue(value);
	return true;
}

bool ToolbarView::SetButton(uint32 index, bool value)
{
	DrawButton2S *button=(DrawButton2S *)buttonlist->ItemAt(index);
	if(button==NULL)
		return false;
	if(button->Value()!=value)
		button->SetValue(value);
	
	return true;
}

bool ToolbarView::FlipButton(const char *name)
{	// Toggles the button. Returns true if successful
	if(name==NULL)
		return false;
		
	DrawButton2S *button=FindButton(name);
	if(button==NULL)
		return false;

	button->SetValue((button->Value())?false:true);
	return true;
}

bool ToolbarView::FlipButton(uint32 index)
{	// Toggles the button. Returns true if successful

	DrawButton2S *button=(DrawButton2S *)buttonlist->ItemAt(index);
	if(button==NULL)
		return false;
	
	button->SetValue((button->Value())?false:true);
	return true;
}

void ToolbarView::SetTarget(BLooper *looper)
{
	DrawButton2S *button;
	int32 max=buttonlist->CountItems();
	
	for(int32 i=0;i<max;i++)
	{
		button=(DrawButton2S *)buttonlist->ItemAt(i);
		if(button==NULL)
			continue;
		button->SetTarget(looper);
	}
}
