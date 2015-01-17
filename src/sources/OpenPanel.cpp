#include "OpenPanel.h"

BPMOpenPanel::BPMOpenPanel(void):
	 BFilePanel(B_OPEN_PANEL,NULL,NULL,B_FILE_NODE,true)
{
	BWindow *win=Window();
	BView *poseview=win->FindView("PoseView"),*tempview;
	BRect rect,poserect;

	win->LockLooper();

	// Move everything up 50 pixels to make room for the preview
	if(poseview->Bounds().Height()<80)
	{	win->ResizeBy(0,50);
	}
	poseview->ResizeBy(0,-50);
	poserect=poseview->ConvertToParent(poseview->Bounds());

	tempview=win->FindView("CountVw");
	if(tempview!=NULL)
		tempview->MoveBy(0,-50);

	tempview=win->FindView("HScrollBar");
	if(tempview!=NULL)
		tempview->MoveBy(0,-50);

	tempview=win->FindView("VScrollBar");
	if(tempview!=NULL)
		tempview->ResizeBy(0,-50);

	rect.left=poserect.left;
	rect.top=poserect.bottom+25;
	rect.right=rect.left+75;
	rect.bottom=rect.top+75;
	
	preview=new DrawButton(rect,"preview",'abcd',B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_NAVIGABLE);
	pbitmap=new BBitmap(preview->Bounds(),B_RGB32);
	preview->SetBitmap(pbitmap);

	win->AddChild(preview);
	win->UnlockLooper();
}

BPMOpenPanel::~BPMOpenPanel(void)
{
	delete pbitmap;
}

void BPMOpenPanel::SelectionChanged(void)
{
	entry_ref ref;
	GetNextSelectedRef(&ref);
	if(ref.name!=NULL)
	{
		BFile file;
		BEntry entry;
		BDirectory dir;
		BPath path;

		// Check to see if it's a directory
		dir.SetTo(&ref);
		if(dir.InitCheck()==B_OK)
		{	Rewind();
			return;
		}

		file.SetTo(&ref,B_READ_ONLY);
		if(file.InitCheck()!=B_OK)
		{	Rewind();
			return;
		}

		// We got this far, so make a new ref from the old one
		// in case we were given a symlink
		entry.SetTo(&ref,true);

		if(entry.InitCheck()!=B_OK)
		{	Rewind();
			return;
		}
		entry.GetRef(&ref);

		if(entry.GetPath(&path)!=B_OK)
		{	Rewind();
			return;
		}

		BBitmap *temp=LoadBitmap((char *)path.Path());
		if(temp!=NULL)
		{
			delete pbitmap;
			pbitmap=temp;
			preview->SetBitmap(pbitmap);
			preview->Update();
		}
	}
	Rewind();
}

 