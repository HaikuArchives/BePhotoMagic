#include "BPMWindow.h"

void BPMWindow::MessageReceived(BMessage *msg)
{
char tempstr[255];
bool tempbool;
int32 tempint32;
int8 tempint8;
int16 w,h;			// for Create Image width & height

	switch (msg->what)
	{	
		case FORCE_REDRAW:
			if(imgview->openimages<1)
				break;
			imgview->p_active_image->UpdateDisplayImage(imgview->Bounds());
			imgview->Draw(imgview->Bounds());
			break;
		case FILE_NEW:
			if(createwin_open == false)
			{	createwin = new CreateWindow( CreateCenteredRect(320,220) );
				createwin_open=true;
			}			
			break;
		case CREATE_IMAGE:
		{
			createwin_open=false;
			msg->FindInt16("width",0,&w);
			msg->FindInt16("height",0,&h);
			
			w=(w<1)?1:w;
			h=(h<1)?1:h;
			sprintf(tempstr,"Untitled %d",untitled);
			untitled++;

			if(uint32(w*h)>0x753000)	// number of pixels in a 30MB image
			{
				BAlert *alert = new BAlert("BePhotoMagic",
					SpTranslate("BePhotoMagic is limited to images no larger than 30MB"),
					SpTranslate("OK"),NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT); 
				alert->Go();
				break;
			}

//			imgview->CreateNewImage(tempstr,w,h);
			imgview->CreateNewImage(" ",w,h);
			if(imgview->IsHidden())
			{	imgview->Show();
				SetFileMenuState(true);
				SetPluginMenuState(true);
			}
			windowmenu->AddItem(new BMenuItem(tempstr,new BMessage(FILE_CHOSEN)));
			windowmenu->ItemAt(windowmenu->CountItems()-1)->SetMarked(true);
			CenterImageView(true);

//			UpdateLayerNumber();
			UpdateLayerWindow();
			imgview->UpdateOptionWindow();
			be_app->PostMessage(UPDATE_COLORS);
			be_app->PostMessage(UPDATE_BRUSH);
			UpdateToolbar();
			UpdateBrushControls();
			break;
		}	
		case B_KEY_DOWN:
//			printf("MainWin received shortcut msg\n");
			be_app->PostMessage(msg);
			break;
			
		case FILE_OPEN:
			openpanel->Show();
			break;

		case FILE_CLOSE:
		{
			// If there aren't any images, close app
			if(imgview->openimages==0)
				QuitRequested();
			else
			{
				if(NotifySaveChanges()==false)
					break;
			}

			// This GetMouse stuff is here to prevent a crash
			// by not allowing mouse button to be down when a close is
			// requested. The UpdaterThread gets destroyed that way
			BPoint pt;
			uint32 buttons;
			imgview->GetMouse(&pt,&buttons);
			if(buttons!=0)
				break;
				
			// Remove from menu and tie up loose ends in actually closing image
			if(windowmenu->ItemAt(imgview->active_image) !=NULL)
				windowmenu->RemoveItem(imgview->active_image);
			imgview->CloseActiveImage();

			// Reset scrollbars
			if(imgview->openimages==0)
			{	if(imgview->IsHidden() == false)
				{
					imgview->Hide();
					SetFileMenuState(false);
					SetPluginMenuState(false);
				}
				hscrollbar->SetRange(0,0);
				vscrollbar->SetRange(0,0);
				// Added this to prevent another crasher - New-Close-Open-*CRASH*
				if(app_prefs.toolwin_open==true)
				{	toolwin->LockLooper();
					toolwin->brushview->DisableBitmap();
					toolwin->UnlockLooper();
				}
				UpdateLayerNumber();
			}
			else
			{	
				windowmenu->ItemAt(imgview->active_image)->SetMarked(true);
				PostMessage(FILE_CHOSEN);
				// Notify child windows
	//			imgview->UpdateLayerWindow();
				UpdateLayerNumber();
				imgview->UpdateOptionWindow();
			}
			
			break;
		}	
		case FILE_SAVE:
			if(imgview->openimages>0)
			{	save_BPM=true;
				savepanel->Window()->Lock();
				savepanel->Window()->SetTitle(SpTranslate("Save as BPM"));
				savepanel->format->SetEnabled(false);
				savepanel->Window()->Unlock();
				savepanel->Show();
				imgview->p_active_image->ClearUndo();
			}
			break;
		
		case FILE_EXPORT_BITMAP:
			if(imgview->openimages>0)
			{	save_BPM=false;
				savepanel->Window()->Lock();
				savepanel->Window()->SetTitle(SpTranslate("Save As"));
				savepanel->format->SetEnabled(true);
				savepanel->Window()->Unlock();
				savepanel->Show();
			}
			break;

		case FILE_QUIT:
			QuitRequested();
			break;

		case EDIT_UNDO:
		{	if(imgview->openimages>0)
			{	imgview->p_active_image->Undo();
				imgview->Draw();
			}
			break;
		}

		case EDIT_COPY:
		{	if(imgview->openimages>0)
				imgview->ClipboardCopy();
			break;
		}

		case EDIT_PASTE_AS_NEW:
		{	
			imgview->ClipboardPasteAsNew();
			break;
		}

		case HELP_ABOUT:
			aboutwin=new AboutWindow();
			aboutwin->Show();
			break;
			
		case FILE_CHOSEN:	// called when we switch images via the menu
		{	imgview->active_image=windowmenu->IndexOf(windowmenu->FindMarked());
			imgview->p_active_image=imgview->images[imgview->active_image];
			imgview->ResizeTo(imgview->p_active_image->display_bitmap->Bounds().Width(),
							imgview->p_active_image->display_bitmap->Bounds().Height());
			CenterImageView(true);
			imgview->UpdateOptionWindow();
			be_app->PostMessage(UPDATE_COLORS);
			be_app->PostMessage(UPDATE_BRUSH);
//			UpdateLayerNumber();
			UpdateLayerWindow();
			UpdateToolbar();
			UpdateBrushControls();
			break;
		}
		case LAYER_NEW:
		{	if(imgview->openimages>0)
			{	imgview->p_active_image->CreateNewLayer("", imgview->p_active_image->width,
						 imgview->p_active_image->height);
				UpdateLayerWindow();
			}
			break;
		}
		case LAYER_DELETE:
		{	if(imgview->openimages>0)
			{
				imgview->p_active_image->SaveUndo(UNDO_LAYER,BRect(0,0,0,0));
				imgview->p_active_image->DeleteLayer();
//				imgview->UpdateLayerWindow();
//				UpdateLayerNumber();
				UpdateLayerWindow();
				imgview->Draw();
			}
			break;
		}
		case LAYER_DUPLICATE:
		{	if(imgview->openimages>0)
			{	imgview->p_active_image->DuplicateLayer();
//				imgview->UpdateLayerWindow();
//				UpdateLayerNumber();
				UpdateLayerWindow();
			}
			break;
		}
		case LAYER_FLATTEN:
		{	if(imgview->openimages<1)
				break;
			imgview->p_active_image->FlattenImage();
//			UpdateLayerNumber();
			UpdateLayerWindow();
			break;
		}	
		case LAYER_MERGE_DOWN:
		{	if(imgview->openimages<1)
				break;
			imgview->p_active_image->MergeDown();
//			UpdateLayerNumber();
			UpdateLayerWindow();
			imgview->Draw();
			break;
		}	
		case LAYER_COMPOSITE:
		{	if(imgview->openimages<1)
				break;
			imgview->p_active_image->MakeComposite();
//			UpdateLayerNumber();
			UpdateLayerWindow();
			imgview->Draw();
			break;
		}	
			
		// When LayerWindow can support drag-and-drop layer moves, these will
		// become deprecated
		case LAYER_MOVE_UP:
		case LAYER_MOVE_DOWN:
		case LAYER_MOVE_TOP:
		case LAYER_MOVE_BOTTOM:
		case FILTERED_BITMAP:
			filterwin_open=false;
			imgview->MessageReceived(msg); // I'm a baaaad boy....  ;^P
			break;

		case SELECT_ALL:
			if(imgview->openimages>0)
			{	imgview->p_active_image->SelectAll();
				imgview->Draw();
			}	
			break;
		case SELECT_NONE:
			if(imgview->openimages>0)
			{	imgview->p_active_image->SelectNone();
				imgview->Draw();
			}	
			break;

		case CENTER_VIEW:
			CenterImageView(true);
			break;

		case ZOOM_WINDOW:
			msg->FindBool("mode",0,&tempbool);
			imgview->SetZoom(tempbool);
			break;
		
		case TOOL_TEXT:
		{
			if(textwin_open==true)
			{	textwin->Lock();
				textwin->Close();
				textwin_open=false;
			}
			else
			{
				textwin=new TextToolWindow();
				textwin->Show();
				textwin_open=true;
			}
			break;
		}
		case TOOL_ERASER:
			if(imgview->openimages<1)
				break;
			imgview->p_active_image->erasemode=(imgview->p_active_image->erasemode==true)?false:true;
			break;
			
		case TOOL_MASK:
			if(imgview->openimages<1)
				break;
			imgview->p_active_image->maskmode=(imgview->p_active_image->maskmode==true)?false:true;
			imgview->Draw();
//			if(app_prefs.optionswin_open==true)
//				PostMessage(msg);
			break;
			
		case TOOL_ZOOM:
			imgview->previoustool=imgview->currenttool;
			currentcursor=CURSOR_ZOOM_IN;
			viewcursor->SetCursor(CURSOR_ZOOM_IN);
			imgview->currenttool=msg->what;
			if(app_prefs.optionswin_open==true)
				optionswin->PostMessage(msg);
			break;

		case TOOL_EYEDROPPER:
			imgview->previoustool=imgview->currenttool;
			currentcursor=CURSOR_PICK;
			viewcursor->SetCursor(CURSOR_PICK);
			imgview->currenttool=msg->what;
			if(app_prefs.optionswin_open==true)
				optionswin->PostMessage(msg);
			break;
		case TOOL_BRUSH:
		{	imgview->previoustool=imgview->currenttool;
			if(imgview->openimages>0)
			{	
				if(imgview->previoustool==TOOL_LINE)
					imgview->p_active_image->linealpha=imgview->p_active_image->forecolor.alpha;
				
				imgview->p_active_image->forecolor.alpha=imgview->p_active_image->brushalpha;
				imgview->p_active_image->backcolor.alpha=imgview->p_active_image->brushalpha;
			}
			currentcursor=CURSOR_CROSS;
			viewcursor->SetCursor(CURSOR_CROSS);
			imgview->currenttool=msg->what;
			if(app_prefs.optionswin_open==true)
				optionswin->PostMessage(msg);
			break;
		}
		case TOOL_LINE:
		{	imgview->previoustool=imgview->currenttool;
			if(imgview->openimages>0)
			{	
				if(imgview->previoustool==TOOL_BRUSH)
					imgview->p_active_image->brushalpha=imgview->p_active_image->forecolor.alpha;
				imgview->p_active_image->forecolor.alpha=imgview->p_active_image->linealpha;
				imgview->p_active_image->backcolor.alpha=imgview->p_active_image->linealpha;
			}
			currentcursor=CURSOR_CROSS;
			viewcursor->SetCursor(CURSOR_CROSS);
			imgview->currenttool=msg->what;
			if(app_prefs.optionswin_open==true)
				optionswin->PostMessage(msg);
			break;
		}
		case TOOL_GRADIENT:
		case TOOL_FILL:
		case TOOL_SPLINE:
		case TOOL_STAMP:
		case TOOL_WAND:
		case TOOL_RECT:
		case TOOL_FRECT:
		case TOOL_ELLIPSE:
		case TOOL_FELLIPSE:
		case TOOL_MOVE:
			imgview->previoustool=imgview->currenttool;
			currentcursor=CURSOR_CROSS;
			viewcursor->SetCursor(CURSOR_CROSS);
			imgview->currenttool=msg->what;
			if(app_prefs.optionswin_open==true)
				optionswin->PostMessage(msg);
			break;

		// Currently not implemented
		case SCREEN_FULL_NONE:
		case SCREEN_FULL_HALF:
		case SCREEN_FULL_ALL:
			SetFullScreen(msg->what);
			break;

/*		case TOGGLE_TOOL_WINDOW:
		case TOGGLE_BRUSH_WINDOW:
		case TOGGLE_OPTIONS_WINDOW:
			be_app->PostMessage(msg);
//			imgview->UpdateLayerWindow();
			UpdateLayerNumber();
			break;
*/
		case TOGGLE_TOOL_WINDOW:
		{	if(app_prefs.toolwin_open)
			{	toolwin->Lock();
				toolwin->Close();
				app_prefs.toolwin_open=false;
			}
			else
			{
				if(imgview->openimages>0)	
					toolwin=new ToolWindow(app_prefs.toolwin_frame,
							imgview->p_active_image->maskmode);
				else
					toolwin=new ToolWindow(app_prefs.toolwin_frame,false);
				
				toolwin->Show();
				app_prefs.toolwin_open=true;

				if(imgview->openimages>0)
				{
					toolwin->LockLooper();
					toolwin->brushview->SetBitmap(imgview->p_active_image->currentbrush);
					toolwin->brushview->Invalidate();
					toolwin->SelectTool(imgview->currenttool,true);
					SetColor(true,true);
					SetColor(false,true);
					UpdateToolbar();
					toolwin->UnlockLooper();
				}
				if(Look()==B_DOCUMENT_WINDOW_LOOK)
				{
					toolwin->LockLooper();
					toolwin->fullscreen->SetButton("full_none",true);
					toolwin->UnlockLooper();
				}
				if(Look()==B_NO_BORDER_WINDOW_LOOK && show_menubar==true)
				{
					toolwin->LockLooper();
					toolwin->fullscreen->SetButton("full_half",true);
					toolwin->UnlockLooper();
				}
				if(Look()==B_NO_BORDER_WINDOW_LOOK && show_menubar==false)
				{
					toolwin->LockLooper();
					toolwin->fullscreen->SetButton("full_full",true);
					toolwin->UnlockLooper();
				}
			}
			break;
		}	
		case TOGGLE_BRUSH_WINDOW:
		{
			if(app_prefs.brushwin_open==true)
			{	brushwin->Lock();
				brushwin->Close();
				app_prefs.brushwin_open=false;
			}
			else
			{	if(imgview->openimages>0)
				{	brushwin=new BrushWindow(app_prefs.brushwin_frame,"Brushes",
					imgview->p_active_image->brush_width,
					imgview->p_active_image->brush_height,
					imgview->p_active_image->brush_softness,
					imgview->p_active_image->brushtype);
				}
				else
				{	//brushwin=new BrushWindow(brushwin_frame,"Brushes");
					brushwin=new BrushWindow(app_prefs.brushwin_frame,(char *)SpTranslate("Brushes"));
				}
				brushwin->Show();
				app_prefs.brushwin_open=true;
			}
			break;
		}
		case TOGGLE_OPTIONS_WINDOW:
		{
			if(app_prefs.optionswin_open)
			{	optionswin->Lock();
				optionswin->Close();
				app_prefs.optionswin_open=false;
			}
			else
			{
				if(imgview->openimages>0)
					optionswin=new OptionsWindow(app_prefs.optionswin_frame,imgview->currenttool);
				else
					optionswin=new OptionsWindow(app_prefs.optionswin_frame,TOOL_BRUSH);
				optionswin->Show();
				app_prefs.optionswin_open=true;
				optionswin->PostMessage(imgview->currenttool);
			}
			break;
		}
   	 	case TOGGLE_BACKCOLOR:
		{
			if(app_prefs.backwin_open)
			{	backwin->Lock();
				backwin->Close();
				app_prefs.backwin_open=false;
			}
			else
			{	backwin=new ColorWindow(app_prefs.backpoint,false);
				app_prefs.backwin_open=true;
				if(imgview->openimages>0)
					SetColor(false,true);
				backwin->Show();
			}
   	 		break;
		}
   	 	case TOGGLE_FORECOLOR:
		{
			if(app_prefs.forewin_open)
			{	forewin->Lock();
				forewin->Close();
				app_prefs.forewin_open=false;
			}
			else
			{	forewin=new ColorWindow(app_prefs.forepoint);
				app_prefs.forewin_open=true;
				if(imgview->openimages>0)
					SetColor(true,true);
				forewin->Show();
			}
   	 		break;
   	 	}
		case TOGGLE_LAYER_WINDOW:
		{	
			if(app_prefs.layerwin_open)
			{	app_prefs.layerwin_frame=layerwin->Frame();
				layerwin->Lock();
				layerwin->Close();
				app_prefs.layerwin_open=false;
			}
			else
			{	layerwin=new LayerWindow(app_prefs.layerwin_frame,
							(char *)SpTranslate("Layers"));
				layerwin->Show();
				app_prefs.layerwin_open=true;
				if(imgview->openimages>0)
				{	
					layerwin->LockLooper();
					layerwin->SetLayers(imgview->p_active_image->number_layers);
					// need to sync the layerwin visible with the data here
					layerwin->SetActive(imgview->p_active_image->active_layer,true);
					layerwin->SetBlendMode(imgview->p_active_image->p_active_layer->blendmode);
					layerwin->UnlockLooper();
				}
			}
			break;
		}
		case TOGGLE_MOUSECAM_WINDOW:
		{	
			if(app_prefs.mousewin_open)
			{	app_prefs.mousepoint=mousewin->Frame().LeftTop();
				mousewin->Lock();
				mousewin->Close();
				app_prefs.mousewin_open=false;
			}
			else
			{	mousewin=new TWindow(10);
				mousewin->MoveTo(app_prefs.mousepoint);
				mousewin->Show();
				app_prefs.mousewin_open=true;
			}
			break;
		}
		case ALTER_TOOL_DATA:
			if(imgview->openimages >0)
			{
				if(msg->FindInt8("tolerance",0,&tempint8)!=B_NAME_NOT_FOUND)
					imgview->p_active_image->tolerance=tempint8;
			}
			break;
			
		case ALTER_LAYERWINDOW_DATA:
			UpdateLayerWindow();
			break;
		case ALTER_LAYER_DATA:
		{
			if(imgview->openimages >0)
			{	if(msg->FindInt8("opacity",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->p_active_layer->opacity=tempint8;
				}
				if(msg->FindInt8("blendmode",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->p_active_layer->blendmode=tempint8;
				}
				if(msg->FindInt8("active_layer",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->active_layer=tempint8;
					imgview->p_active_image->p_active_layer=imgview->p_active_image->layers[tempint8];
					layerwin->LockLooper();
					layerwin->SetControlNotification(false);
					layerwin->SetBlendMode(imgview->p_active_image->p_active_layer->blendmode);
					layerwin->SetOpacity(imgview->p_active_image->p_active_layer->opacity);
					layerwin->SetControlNotification(true);
					layerwin->UnlockLooper();
				}
				if(msg->FindInt8("num_hidden_layers",0,&tempint8)!=B_NAME_NOT_FOUND);
				{	int8 hidden_index=tempint8,i;

					// Reset all layers to visible
					for(i=0;i<imgview->p_active_image->number_layers;i++)
					{	imgview->p_active_image->layers[i]->visible=true;
					}

					// Hide only the ones specified
					for(i=0; i<hidden_index; i++)
					{	
						if(msg->FindInt8("hidden_layer",i,&tempint8)!=B_NAME_NOT_FOUND)
							imgview->p_active_image->layers[tempint8]->visible=false;
					}
				}
				imgview->p_active_image->UpdateDisplayImage(Bounds());
				imgview->Draw(imgview->Bounds());
			}
			break;
		}
		case ALTER_BRUSH_DATA:	
		{
			if(imgview->openimages >0)
			{
				if(msg->FindInt8("opacity",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->forecolor.alpha=tempint8;
					imgview->p_active_image->backcolor.alpha=tempint8;
					imgview->p_active_image->brushalpha=tempint8;
				}
				if(msg->FindInt8("blendmode",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->brushblend=tempint8;
					imgview->p_active_image->blendmode=tempint8;
				}
			}
			break;
		}
		case ALTER_LINE_DATA:	
		{
			if(imgview->openimages >0)
			{	
				if(msg->FindInt8("opacity",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->forecolor.alpha=tempint8;
					imgview->p_active_image->backcolor.alpha=tempint8;
					imgview->p_active_image->linealpha=tempint8;
				}
				if(msg->FindInt8("blendmode",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->lineblend=tempint8;
					imgview->p_active_image->blendmode=tempint8;
				}
				if(msg->FindInt8("step",0,&tempint8)!=B_NAME_NOT_FOUND)
				{	imgview->p_active_image->linestep=tempint8;
				}
			}
			break;
		}
		case ALTER_OPTIONWIN_DATA:
		{	UpdateOptionsWindow();
		}
		case TOOLWIN_CLOSED:
		{
			BRect r;
			app_prefs.toolwin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.toolwin_frame=r;
			break;
		}
		case FOREWIN_CLOSED:
		{
			BRect r;
			app_prefs.forewin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.forepoint=r.LeftTop();
			break;
		}
		case BACKWIN_CLOSED:
		{
			BRect r;
			app_prefs.backwin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.backpoint=r.LeftTop();
			break;
		}
		case BRUSHWIN_CLOSED:
		{
			BRect r;
			app_prefs.brushwin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.brushwin_frame=r;
			break;
		}
		case LAYERWIN_CLOSED:
		{
			BRect r;
			app_prefs.layerwin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.layerwin_frame=r;
			break;
		}
		case OPTIONSWIN_CLOSED:
		{
			BRect r;
			app_prefs.optionswin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.optionswin_frame=r;
			break;
		}
		case TEXTWIN_CLOSED:
			textwin_open=false;
			if(app_prefs.toolwin_open==true)
				toolwin->PostMessage(msg);
			break;

		case MOUSECAM_CLOSED:
		{
			BRect r;
			app_prefs.mousewin_open=false;
			if(msg->FindRect("frame",&r)!=B_OK)
				break;
			app_prefs.mousepoint=r.LeftTop();
			break;
		}
		case CHILDWIN_MOVED:
		{
			BPoint temppt;
			
			if(msg->FindPoint("brushwin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.brushwin_frame.OffsetTo(temppt);
			
			if(msg->FindPoint("forewin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.forepoint=temppt;
			
			if(msg->FindPoint("backwin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.backpoint=temppt;
			
			if(msg->FindPoint("layerwin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.layerwin_frame.OffsetTo(temppt);
			
			if(msg->FindPoint("toolwin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.toolwin_frame.OffsetTo(temppt);
			
			if(msg->FindPoint("optionswin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.optionswin_frame.OffsetTo(temppt);
			
			if(msg->FindPoint("mousewin",0,&temppt)!=B_NAME_NOT_FOUND)
				app_prefs.mousepoint=temppt;
			
			break;
		}
		case UPDATE_COLORS:
		{	if(imgview->openimages < 1)
				break;
			SetColor(false,true);
			SetColor(true,true);
			break;
		}
		case FORECOLOR_CHANGED:
			SetColor(true);
			break;
			
		case BACKCOLOR_CHANGED:
			SetColor(false);
			break;

		case GET_APP_DIR:
		{	BPath temppath;
			msg->FindInt8("sourcewin",0,&tempint8);
			AppDir(&temppath);
			switch(tempint8)
			{	case 1:	// brush window
					brushwin->allbrushes->brushdir=temppath.Path();
					brushwin->PostMessage(new BMessage(BRUSHDIR_SET));
					break;
				default:
					break;
			}
			break;
		}
		case SELECT_USER_BRUSH:
		{	// Get reference to path and 
			entry_ref brush_ref;
			BBitmap *brushbmp;
			msg->FindRef("ref",0,&brush_ref);
			msg->MakeEmpty();
			BPath *brushpath=(BPath *)new BPath(&brush_ref);
			brushbmp=LoadBitmap((char *)brushpath->Path());
			if(brushbmp != NULL)
				SetUserBrush(brushbmp);
			delete brushpath;
			break;
		}	
		case UPDATE_BRUSH:
			UpdateBrush();
			break;

		case SET_BRUSH:
			SetBrush(msg);
			break;

		case SHOW_FILTER_WINDOW:
			if(filterwin_open==false && imgview->openimages >0)
			{	msg->FindInt32("addon_id",0,&tempint32);
				filterwin_open=true;
				filterwin=new FilterWindow(tempint32,false);
				Lock();
				BMessage *source=Bitmap2Message(imgview->p_active_image->p_active_layer->bitmap);
				Unlock();
				source->what=(int32)FLATTENED_BITMAP;
				filterwin->PostMessage(source);
			}
			else
			{	BAlert *alert=new BAlert("BePhotoMagic",SpTranslate("You need an image first."),SpTranslate("OK"));
				alert->Go();
			}
			break;			

		case SHOW_CONVERT_WINDOW:
			if(filterwin_open==false && imgview->openimages >0)
			{	msg->FindInt32("addon_id",0,&tempint32);
				filterwin_open=true;
				filterwin=new FilterWindow(tempint32,true);
			}
			else
			{	BAlert *alert=new BAlert("BePhotoMagic",SpTranslate("You need an image first."),SpTranslate("OK"));
				alert->Go();
			}
			break;			

		case POST_BITMAP:
		{	BBitmap *textbmp;
			textbmp=Message2Bitmap(msg);
			if(textbmp!=NULL)
				SetUserBrush(textbmp);
			break;
		}

		case MOUSECAM_HELP:
		{	if(app_prefs.mousewin_open==true)
				mousewin->PostMessage(msg);
			break;
		}

		default:
			BWindow::MessageReceived(msg);
			break;
	}	
}
