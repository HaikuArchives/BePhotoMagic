#include "BPMWindow.h"

/*
	Just like BPMMainF.cpp, this contains all non-hook functions for the
	BPMWindow class to help cut compile time
*/

bool BPMWindow::NotifySaveChanges(void)
{
	// Called before closing the image. Returns false if it's not OK to close
	if(imgview->openimages<1)
		return true;
	
	if(imgview->p_active_image->undolist->CountItems()>1)
	{
		
		int32 alert_choice;
		BAlert *alert=new BAlert("BePhotoMagic",SpTranslate("Save Changes?"),
			SpTranslate("Save"),SpTranslate("Don't Save"),
			SpTranslate("Cancel"),B_WIDTH_AS_USUAL,B_WARNING_ALERT); 
		alert_choice=alert->Go();
		if(alert_choice==2)
			return false;
		if(alert_choice==0)
		{	PostMessage(FILE_SAVE);
			return false;
		}
	}
	return true;
}

BRect BPMWindow::CreateCenteredRect(float width, float height)
{	// This takes the current bounds of the main window and
	// returns a BRect which is centered with respect to it or screen
	BScreen screen;
	BRect workrect(0,0,width-1,height-1),
			screenrect(screen.Frame()),
			winrect(Frame());

	float wdiff,hdiff;
	// Set the upper-left corners equal to each other
	workrect.OffsetBy(winrect.LeftTop());
	
	// if the rect goes offscreen, constrain to screen
	if(!(screenrect.Contains(workrect)))
	{	workrect.LeftTop().ConstrainTo(screenrect);
		workrect.RightBottom().ConstrainTo(screenrect);
	}
	
	// Center to screen if bigger than window
	if( (workrect.Width() > winrect.Width()) || 
		(workrect.Height() > winrect.Height()) )
	{	wdiff=screenrect.Width()-workrect.Width();
		hdiff=screenrect.Height()-workrect.Height();
		workrect.OffsetTo( (screenrect.Width()-workrect.Width())/2,
				(screenrect.Height()-workrect.Height())/2 );
	}
	else	// Center to window
	{	wdiff=winrect.Width()-workrect.Width();
		hdiff=winrect.Height()-workrect.Height();
		workrect.OffsetTo( winrect.left+(winrect.Width()-workrect.Width())/2,
				winrect.top+(winrect.Height()-workrect.Height())/2 );
	}
	return workrect;
}

void BPMWindow::OpenImage(const char *tempstr, BBitmap *bmp)
{
	LockLooper();
	createwin_open=false;

	imgview->CreateNewImageFromBitmap(tempstr,bmp);
	if(imgview->IsHidden())
	{
		imgview->Show();
		SetFileMenuState(true);
		SetPluginMenuState(true);
	}
	windowmenu->AddItem(new BMenuItem(tempstr,new BMessage(FILE_CHOSEN)));
	windowmenu->ItemAt(windowmenu->CountItems()-1)->SetMarked(true);
	CenterImageView(true);
	UnlockLooper();
	PostMessage(FILE_CHOSEN);
	UpdateLayerNumber();
}
/*
BRect BPMWindow::CenterImageView(bool reset_scrollbars)
{
	// This centers the image view, adjusts the scrollbars, etc
	BRect 	vrect=imgview->Bounds(),
			wrect=scrollview->Bounds();

	float 	hdiff=wrect.Width()-vrect.Width(),
			vdiff=wrect.Height()-vrect.Height();

	imgview->SetScale(imgview->p_active_image->zoom);

	// Reset bounds based on differences. We actually may have negative values, too
	vrect.left=int32((hdiff > 10)?hdiff/2:10);
	vrect.right=vrect.left + imgview->Bounds().Width();
	vrect.top=int32((vdiff > 10)?vdiff/2:10);
	vrect.bottom=vrect.top + imgview->Bounds().Height();
	
	imgview->ResizeTo(vrect.Width()-1,vrect.Height()-1);
	imgview->MoveTo(vrect.LeftTop());
	
	// Reset scrollbars if asked
	if(reset_scrollbars)
	{	
		float value,min,max,ratio;
	
		if(vrect.Width()>wrect.Width())
		{	
			// Getting current value so we can keep things approximately the same
			// spot in the image
			hscrollbar->GetRange(&min,&max);
			value=hscrollbar->Value();
			ratio=value/max;
			
			hscrollbar->SetRange(0,(-1 * hdiff)+40);
			hscrollbar->SetValue(0);
		}
		else
			hscrollbar->SetRange(0,0);

		if(vrect.Height()>wrect.Height())
		{	
			vscrollbar->GetRange(&min,&max);
			value=vscrollbar->Value();
			ratio=value/max;
			
			vscrollbar->SetRange(0,(-1 * vdiff)+40);
			vscrollbar->SetValue(0);
		}
		else
			vscrollbar->SetRange(0,0);
	}
	return vrect;
}
*/

BRect BPMWindow::CenterImageView(bool reset_scrollbars)
{
	// This centers the image view, adjusts the scrollbars, etc
	BRect 	vrect=imgview->Bounds(),
//			wrect=scrollview->Bounds();
			wrect=desktop->Bounds();
	float 	hdiff=wrect.Width()-vrect.Width(),
			vdiff=wrect.Height()-vrect.Height();

//	imgview->SetScale(imgview->p_active_image->zoom);

	// Reset bounds based on differences. We actually may have negative values, too
	vrect.left=int32((hdiff > 10)?hdiff/2:10);
	vrect.right=vrect.left + imgview->Bounds().Width();
	vrect.top=int32((vdiff > 10)?vdiff/2:10);
	vrect.bottom=vrect.top + imgview->Bounds().Height();
	
//	imgview->ResizeTo(vrect.Width()-1,vrect.Height()-1);
	imgview->MoveTo(vrect.LeftTop());
	
	// Reset scrollbars if asked
	if(reset_scrollbars)
	{	
		float value,min,max,ratio;
	
//		if(vrect.Width()>wrect.Width())
		if(hdiff<10)
		{	
			// Getting current value so we can keep things approximately the same
			// spot in the image
			hscrollbar->GetRange(&min,&max);
			value=hscrollbar->Value();
			ratio=(max!=0)?value/max:0;
			hscrollbar->SetValue(0);
			hscrollbar->SetRange(0,(-1 * hdiff)+40);
			hscrollbar->SetValue(ratio*hdiff*-1);
		}
		else
			hscrollbar->SetRange(0,0);

//		if(vrect.Height()>wrect.Height())
		if(vdiff<10)
		{	
			vscrollbar->GetRange(&min,&max);
			value=vscrollbar->Value();
			ratio=(max!=0)?value/max:0;
			
			vscrollbar->SetValue(0);
			vscrollbar->SetRange(0,(-1 * vdiff)+40);
			vscrollbar->SetValue(value*vdiff*-1);
		}
		else
			vscrollbar->SetRange(0,0);
	}
	return vrect;
}

void BPMWindow::SetFullScreen(int32 screenmode)
{
	BRect screenRect = BScreen(this).Frame();

	switch(screenmode)
	{
		case SCREEN_FULL_NONE:
			// Don't bother if we're already there
			if(Look()==B_DOCUMENT_WINDOW_LOOK)
				break;
			Lock();
			SetLook(B_DOCUMENT_WINDOW_LOOK);

			ResizeTo(normal_frame.Width(),normal_frame.Height()); 
			MoveTo(normal_frame.left,normal_frame.top);

			if (show_menubar==false) 
			{
				AddChild(menubar);
				show_menubar=true; 
				scrollview->ResizeBy(0,-20);
				scrollview->MoveBy(0,20);
			}
//			scrollview->ResizeTo(Bounds().Width(),
//						Bounds().Height()-menubar->Bounds().Height());
//			scrollview->MoveTo(0,menubar->Bounds().Height()-1);
//			desktop->MoveTo(0,0);
			CenterImageView(true);
			Unlock();
			break;
								
		case SCREEN_FULL_HALF:
			// Don't bother if we're already there
			if(Look()==B_NO_BORDER_WINDOW_LOOK && show_menubar==true)
				break;

			SetLook(B_NO_BORDER_WINDOW_LOOK);

			ResizeTo(screenRect.Width(),screenRect.Height()); 
			MoveTo(0,0);
	
			Lock();
			if (show_menubar==false) 
			{
				AddChild(menubar);
				show_menubar=true; 
				scrollview->ResizeBy(0,-20);
				scrollview->MoveBy(0,20);
			}
//			scrollview->ResizeTo(Bounds().Width(),
//						Bounds().Height()-menubar->Bounds().Height());
//			scrollview->MoveTo(0,menubar->Bounds().Height());	
								
			CenterImageView(true);
			Unlock();
			break;
			
		case SCREEN_FULL_ALL:
			// Don't bother if we're already there
			if(Look()==B_NO_BORDER_WINDOW_LOOK && show_menubar==false)
				break;

			Lock();
			SetLook(B_NO_BORDER_WINDOW_LOOK);
	
			saveframe=false;
			ResizeTo(screenRect.Width(),screenRect.Height());
			MoveTo(0,0);
			if (show_menubar==true)
			{
				menubar->RemoveSelf();
				show_menubar=false; 
				scrollview->ResizeBy(0,20);
				scrollview->MoveBy(0,-20);
			}
//			scrollview->MoveTo(0,0);	
//			scrollview->ResizeTo(Bounds().Width(),Bounds().Height());
			saveframe=true;

			CenterImageView(true);
			Unlock();
			break;
							
		default: 
			break;
	}
	
}

void BPMWindow::SaveActive(BMessage *msg)
{	
	entry_ref ref;
	BString filename;
	BPath temppath;
	
	msg->FindRef("directory",0,&ref);
	BEntry entry(&ref);
	msg->FindString("name",&filename);

	imgview->p_active_image->path.SetTo(filename);
	entry.GetPath(&temppath);
	
	char path[255];
	sprintf(path,"%s/%s",temppath.Path(),filename.String());

	BPMFile *bpmfile=new BPMFile(imgview->p_active_image);
	bpmfile->WriteProject(path,B_CREATE_FILE);
	
	delete bpmfile;
}


bpm_status BPMWindow::OpenBPM(const char *path)
{		
	BPMFile *bpmfile=new BPMFile();
	BPMImage *tempimage=(BPMImage *)bpmfile->ReadProject(path);
	if(tempimage !=NULL)
	{
		LockLooper();
		createwin_open=false;
		imgview->images[imgview->openimages]=tempimage;
		imgview->active_image=imgview->openimages;
		imgview->p_active_image=tempimage;
		imgview->openimages++;
		imgview->p_active_image->active_layer=0;
		imgview->p_active_image->p_active_layer=imgview->p_active_image->layers[0];

		imgview->ResizeTo(tempimage->width,tempimage->height);
	
		if(imgview->IsHidden())
		{
			imgview->Show();
			SetFileMenuState(true);
			SetPluginMenuState(true);
		}
		windowmenu->AddItem(new BMenuItem(path,new BMessage(FILE_CHOSEN)));
		windowmenu->ItemAt(windowmenu->CountItems()-1)->SetMarked(true);
		CenterImageView(true);
		imgview->p_active_image->UpdateDisplayImage(imgview->Bounds());
		imgview->UpdateLayerWindow();
		imgview->UpdateOptionWindow();
		be_app->PostMessage(UPDATE_COLORS);
		UnlockLooper();
		UpdateLayerNumber();
		UpdateBrush();
		return BPM_OK;
	}
	return BPM_BAD_PARAMETER;
}

void BPMWindow::SetFilterMenu(void)
{
	BMenu *tempmenu;
	BMenuItem *tempitem;
 	uint16 categorycount;
	BMessage *msg;
	
	if(filtermenu->CountItems()!=0)
		filtermenu->RemoveItems(0,filtermenu->CountItems());

	// Create bitmap accessor object to pass along, so that we
	// only get the add-ons that support this bitmap in the menu
	BBitmap *bmp=new BBitmap(BRect(0,0,320,240),B_RGBA32,true);
	BBitmapAccessor accessor(bmp);
	accessor.SetDispose(false);
	
	const char *addonName;
	const char *addonInfo;
	const char *addonCategory;
	int32 addonVersion;
	image_addon_id *outList;
	int32 outCount = 0;
	
	// Get Manipulator filters
	if (Image_GetManipulators(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			// Get plugin
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				// Does it belong to a new category
				categorycount=filtermenu->CountItems();

				if(categorycount!=0)
				{	
					// New category?
					tempitem=filtermenu->FindItem(addonCategory);
					if(tempitem!=NULL)
					{
						tempmenu=tempitem->Submenu();
						
						// nothing new, so just add the item
						msg = new BMessage(SHOW_FILTER_WINDOW);
						msg->AddInt32("addon_id", outList[i]);
	
						// Add at position 0 so it's sorted properly
						tempmenu->AddItem(new BMenuItem(addonName, msg),0);
					}
					else
					{	// New category - add category and item
						msg = new BMessage(SHOW_FILTER_WINDOW);
						msg->AddInt32("addon_id", outList[i]);
	
						tempmenu=new BMenu(addonCategory);
	
						// Add at position 0 so it's sorted properly
						tempmenu->AddItem(new BMenuItem(addonName, msg),0);
						filtermenu->AddItem(tempmenu);
					}
				}
				else
				{	
					// Nothing's been added yet, so add the category and
					// the filter into it
					msg = new BMessage(SHOW_FILTER_WINDOW);
					msg->AddInt32("addon_id", outList[i]);

					tempmenu=new BMenu(addonCategory);

					// Add at position 0 so it's sorted properly
					tempmenu->AddItem(new BMenuItem(addonName, msg),0);
					filtermenu->AddItem(tempmenu);
				}
			}
		}
		delete[] outList;
	}

	// Get Converter filters
	if (Image_GetConverters(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			// Get plugin
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				// Does it belong to a new category
				categorycount=filtermenu->CountItems();

				if(categorycount!=0)
				{	
					// New category?
					tempitem=filtermenu->FindItem(addonCategory);
					if(tempitem!=NULL)
					{
						tempmenu=tempitem->Submenu();
						
						// nothing new, so just add the item
						msg = new BMessage(SHOW_CONVERT_WINDOW);
						msg->AddInt32("addon_id", outList[i]);
	
						// Add at position 0 so it's sorted properly
						tempmenu->AddItem(new BMenuItem(addonName, msg),0);
					}
					else
					{	// New category - add category and item
						msg = new BMessage(SHOW_CONVERT_WINDOW);
						msg->AddInt32("addon_id", outList[i]);
	
						tempmenu=new BMenu(addonCategory);
	
						// Add at position 0 so it's sorted properly
						tempmenu->AddItem(new BMenuItem(addonName, msg),0);
						filtermenu->AddItem(tempmenu);
					}
				}
				else
				{	
					// Nothing's been added yet, so add the category and
					// the filter into it
					msg = new BMessage(SHOW_FILTER_WINDOW);
					msg->AddInt32("addon_id", outList[i]);

					tempmenu=new BMenu(addonCategory);

					// Add at position 0 so it's sorted properly
					tempmenu->AddItem(new BMenuItem(addonName, msg),0);
					filtermenu->AddItem(tempmenu);
				}
			}
		}
		delete[] outList;
	}

	delete bmp;
}

void BPMWindow::UpdateLayerWindow(void)
{
	// This does a full update on all controls in the window
	if(app_prefs.layerwin_open==false)
		return;
	layerwin->LockLooper();
	layerwin->SetControlNotification(false);
	if(imgview->openimages>0)
	{	
		layerwin->SetLayers(imgview->p_active_image->number_layers);
		layerwin->SetActive(imgview->p_active_image->active_layer,true);
		layerwin->SetBlendMode(imgview->p_active_image->p_active_layer->blendmode);
		layerwin->SetOpacity(imgview->p_active_image->p_active_layer->opacity);
	}
	else
	{
		layerwin->SetLayers(0);
		layerwin->SetActive(0,true);
		layerwin->SetBlendMode(TNORMAL);
		layerwin->SetOpacity(255);
	}
	layerwin->SetControlNotification(true);
	layerwin->UnlockLooper();
	
}

void BPMWindow::UpdateLayerNumber(void)
{
	// Just updates the number of layers in the window
	
	if(app_prefs.layerwin_open==false)
		return;
	layerwin->LockLooper();
	if(imgview->openimages>0)
	{	
		layerwin->SetControlNotification(false);
		layerwin->SetLayers(imgview->p_active_image->number_layers);
		layerwin->SetActive(imgview->p_active_image->active_layer,true);
		layerwin->SetControlNotification(true);
	}
	else
	{
		layerwin->SetControlNotification(false);
		layerwin->SetLayers(0);
		layerwin->SetActive(0,true);
		layerwin->SetControlNotification(true);
	}
	layerwin->UnlockLooper();
}

void BPMWindow::UpdateOptionsWindow(void)
{
	// Updates the options window to be in sync with the data
	if(app_prefs.optionswin_open==false)
		return;
	optionswin->LockLooper();
	if(imgview->openimages>0)
		optionswin->SetTool(imgview->currenttool);
	else
		optionswin->SetTool(TOOL_BRUSH);
	optionswin->UnlockLooper();
}

void BPMWindow::UpdateBrushControls(void)
{
	if(app_prefs.brushwin_open==false)
		return;
	brushwin->LockLooper();
	if(imgview->openimages>0)
	{
		brushwin->dview->WidthSlider->SetValue(float(imgview->p_active_image->brush_width));
		brushwin->dview->HeightSlider->SetValue(float(imgview->p_active_image->brush_height));
		brushwin->dview->BlurSlider->SetValue(float(imgview->p_active_image->brush_softness));
//		brushwin->dview->EvenAspect->SetValue(B_CONTROL_OFF);
//		brushwin->dview->LockAspect->SetValue(B_CONTROL_OFF);
		if(imgview->p_active_image->brushtype==BRUSH_TYPE_RECT)
			brushwin->dview->RectBrush->SetValue(B_CONTROL_ON);
		else
			brushwin->dview->RectBrush->SetValue(B_CONTROL_OFF);
	}
	else
	{
		brushwin->dview->WidthSlider->SetValue(16.0);
		brushwin->dview->HeightSlider->SetValue(16.0);
		brushwin->dview->BlurSlider->SetValue(0.0);
//		brushwin->dview->EvenAspect->SetValue(B_CONTROL_OFF);
//		brushwin->dview->LockAspect->SetValue(B_CONTROL_OFF);
		brushwin->dview->RectBrush->SetValue(B_CONTROL_OFF);
	}
	brushwin->UnlockLooper();
}

void BPMWindow::SetColor(bool fore=true,bool fromimage=false)
{	rgb_color tempcol;
	if(fromimage==true)
	{
		// This is if the color is set from the image or the eyedropper tool
		if(imgview->openimages>0)
		{
			// Update toolbar if open
			if(app_prefs.toolwin_open==true)
			{	
				toolwin->LockLooper();
				if(fore==true)
					toolwin->SetColor(imgview->p_active_image->forecolor);
				else	// background mode
					toolwin->SetColor(imgview->p_active_image->backcolor,false);
				toolwin->UnlockLooper();
			}
			// Update color pickers if open
			if(app_prefs.forewin_open==true && fore==true)
			{	forewin->LockLooper();
				forewin->SetColor(imgview->p_active_image->forecolor);
				forewin->UnlockLooper();
			}
			else
			{	if(app_prefs.backwin_open==true && fore==false)
				{	backwin->LockLooper();
					backwin->SetColor(imgview->p_active_image->backcolor);
					backwin->UnlockLooper();
				}
			}
		}
	}
	else
	{
		// This is if the color is set from the picker
		if(app_prefs.toolwin_open==true)
		{	toolwin->LockLooper();
			if(fore==true)
			{	toolwin->SetColor(forewin->GetColor());
				if(imgview->openimages>0)
				{	tempcol=forewin->GetColor();
					tempcol.alpha=imgview->p_active_image->forecolor.alpha;
					imgview->p_active_image->forecolor=tempcol;
				}
			}
			else
			{	toolwin->SetColor(backwin->GetColor(),false);
				if(imgview->openimages>0)
				{	tempcol=backwin->GetColor();
					tempcol.alpha=imgview->p_active_image->backcolor.alpha;
					imgview->p_active_image->backcolor=tempcol;
				}
			}
			toolwin->UnlockLooper();
		}
	}
}

void BPMWindow::SetUserBrush(BBitmap *brush)
{
	if(app_prefs.toolwin_open)
	{	toolwin->LockLooper();
		toolwin->brushview->SetBitmap(brush);
		toolwin->brushview->Invalidate();
		toolwin->UnlockLooper();
	}
	if(imgview->openimages > 0)
	{	LockLooper();
		imgview->p_active_image->SaveUndo(UNDO_BRUSH,BRect(0,0,0,0));
		delete imgview->p_active_image->currentbrush;
		imgview->p_active_image->currentbrush=brush;
		imgview->p_active_image->userbrush=true;
		UnlockLooper();
	}
}

void BPMWindow::UpdateBrush(void)
{
	if(imgview->openimages < 1)
		return;

	if(app_prefs.toolwin_open)
	{
		toolwin->LockLooper();
		toolwin->brushview->SetBitmap(imgview->p_active_image->currentbrush);
		toolwin->brushview->Invalidate();
		toolwin->UnlockLooper();
	}
	UpdateBrushControls();
}

void BPMWindow::SetBrush(BMessage *msg)
{	
	uint8 softness,type;
	uint16 width,height;
	msg->FindInt8("type",0,(int8 *)&type);
	msg->FindInt8("softness",0,(int8 *)&softness);
	msg->FindInt16("width",0,(int16 *)&width);
	msg->FindInt16("height",0,(int16 *)&height);
	
	if(imgview->openimages > 0)
	{
		imgview->p_active_image->SaveUndo(UNDO_BRUSH,BRect(0,0,0,0));
		imgview->p_active_image->MakeBrush(width,height,softness,type);
		imgview->p_active_image->userbrush=false;
	
		if(app_prefs.toolwin_open)
		{	toolwin->LockLooper();
			toolwin->brushview->SetBitmap(imgview->p_active_image->currentbrush);
			toolwin->brushview->Invalidate();
			toolwin->UnlockLooper();
		}
	}

}

void BPMWindow::UpdateToolbar(void)
{
	if(app_prefs.toolwin_open==true)
	{
		toolwin->Lock();
		if(imgview->openimages>0)
		{
			if(imgview->p_active_image->erasemode==true)
//				toolwin->eraserbutton->SetValue(B_CONTROL_ON);
				toolwin->toolbarb->SetButton("eraser",true);
			else
				toolwin->toolbarb->SetButton("eraser",false);
//				toolwin->eraserbutton->SetValue(B_CONTROL_OFF);
			
			if(imgview->p_active_image->maskmode==true)
				toolwin->toolbarb->SetButton("mask",true);
//				toolwin->maskbutton->SetValue(B_CONTROL_ON);
			else
				toolwin->toolbarb->SetButton("mask",false);
//				toolwin->maskbutton->SetValue(B_CONTROL_OFF);
		}
		toolwin->Unlock();
	}
}

void BPMWindow::SetFileMenuState(bool state)
{	// NOTE: this doesn't toggle all the File menu items - just the ones
	// which should be disabled when there are no files open
	filemenu->ItemAt(5)->SetEnabled(state);	// Save Project
	filemenu->ItemAt(6)->SetEnabled(state);	// Save As
}

void BPMWindow::SetClipboardMenuState(bool state)
{	// No clipboard ops working, so do nothing
}

void BPMWindow::SetPluginMenuState(bool state)
{	filtermenu->SetEnabled(state);
}
