#include "BPMView.h"

extern BWindow *bpmwindow;
pattern viewbase={{240,240,240,240,15,15,15,15}};

PicView::PicView(BRect r, long flags) :
	BView(r, "picview", flags, B_WILL_DRAW | B_SUBPIXEL_PRECISE | B_FRAME_EVENTS
						| B_FULL_UPDATE_ON_RESIZE | B_ASYNCHRONOUS_CONTROLS)
{
	openimages=0;
	active_image=0;
	mousedown=false;
	zoomin=false;
	tool_blendmode=0;
	tool_alpha=255;
	currenttool=TOOL_BRUSH;
	previoustool=TOOL_BRUSH;
	stamp_xoffset=10;
	stamp_yoffset=10;
	stamp_rect.Set(0,0,0,0);
	gradientmode=GRADIENT_RECTANGULAR;
}

PicView::~PicView()
{
}

void PicView::AttachedToWindow(void)
{
//	backbitmap=BTranslationUtils::GetBitmap('bits',"back_text.png");
//	SetViewBitmap(backbitmap,backbitmap->Bounds(),backbitmap->Bounds());
//	delete backbitmap;
}

void PicView::Draw(BRect update_rect)
{
	if(openimages>0)
	{	
		update_rect.left=floor(update_rect.left/p_active_image->zoom);
		update_rect.right=floor(update_rect.right/p_active_image->zoom);
		update_rect.top=floor(update_rect.top/p_active_image->zoom);
		update_rect.bottom=floor(update_rect.bottom/p_active_image->zoom);

		SetHighColor(255,255,255);
		SetLowColor(216,216,216);
		FillRect(update_rect,viewbase);

		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(p_active_image->display_bitmap,update_rect,update_rect);
		if(p_active_image->maskmode==true)
			DrawBitmap(p_active_image->selection,update_rect,update_rect);
		SetDrawingMode(B_OP_COPY);
	}
}

void PicView::Draw(void)
{	
	Draw(Bounds());
}

void PicView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{	
		case B_KEY_DOWN:
//			printf("PicView received shortcut msg\n");
			be_app->PostMessage(msg);
			break;

		case FILTERED_BITMAP:
		{	// We should have a bitmap stashed in here, so let's
			// get it out and apply it to the current layer.
			BBitmap *temp=Message2Bitmap(msg);

			// Eventually this will apply the bitmap in the selection context
			// For now, just do a quick copy
			if(temp!=NULL)
			{	p_active_image->SaveUndo(UNDO_TOOL,p_active_image->p_active_layer->bitmap->Bounds());
				CopyRect(temp->Bounds(),temp,
					p_active_image->p_active_layer->bitmap->Bounds(),
					p_active_image->p_active_layer->bitmap,4);
				delete temp;
				p_active_image->UpdateDisplayImage(Bounds());
				Draw(); //Draw(Bounds());
			}
			break;
		}
		// When LayerWindow can support drag-and-drop layer moves, these will
		// become deprecated
		case LAYER_MOVE_UP:
			p_active_image->SwapLayers(p_active_image->active_layer,p_active_image->active_layer+1);
			Draw();
			break;
		case LAYER_MOVE_DOWN:
			p_active_image->SwapLayers(p_active_image->active_layer,p_active_image->active_layer-1);
			Draw();
			break;
		case LAYER_MOVE_TOP:
			p_active_image->SwapLayers(p_active_image->active_layer,p_active_image->number_layers-1);
			Draw();
			break;
		case LAYER_MOVE_BOTTOM:
			p_active_image->SwapLayers(p_active_image->active_layer,0);
			Draw();
			break;

		default:
			BView::MessageReceived(msg);
		break;
		
	}
}

void PicView::MouseDown(BPoint where)
{	BPoint current;
	uint32 buttons;

	SetMouseEventMask(0,B_LOCK_WINDOW_FOCUS);
	mousedown=true;
			
	switch(currenttool)
	{	
		case TOOL_BRUSH:
		{	
			if(IsHidden()==true)
				break;
			delete p_active_image->work_bitmap;
			if(p_active_image->maskmode==false)
				p_active_image->work_bitmap = new BBitmap(p_active_image->p_active_layer->bitmap);
			else
				p_active_image->work_bitmap = new BBitmap(p_active_image->selection);
			GetMouse(&current,&buttons,false);

			updaterID = spawn_thread(ThreadUpdater, "BPMViewUpdater", B_NORMAL_PRIORITY, this);
			if (updaterID >= 0)
				resume_thread(updaterID);
			p_active_image->save_each_paint=true;

			if(buttons == B_PRIMARY_MOUSE_BUTTON)
				p_active_image->PaintBrush(where);
			else
			{	if(buttons == B_SECONDARY_MOUSE_BUTTON)
					p_active_image->PaintBrush(where,false);
			}
			p_active_image->ispainting=true;
			break;
		}
/*		case TOOL_STAMP:
		{	
			if(IsHidden()==true)
				break;
			delete p_active_image->work_bitmap;
//			if(p_active_image->maskmode==false)
//				p_active_image->work_bitmap = new BBitmap(p_active_image->p_active_layer->bitmap);
//			else
//				p_active_image->work_bitmap = new BBitmap(p_active_image->selection);
			GetMouse(&current,&buttons,false);

			updaterID = spawn_thread(ThreadUpdater, "BPMViewUpdater", B_NORMAL_PRIORITY, this);
			if (updaterID >= 0)
				resume_thread(updaterID);
			p_active_image->save_each_paint=true;

			if(buttons == B_PRIMARY_MOUSE_BUTTON)
				p_active_image->Stamp(where,where+BPoint(stamp_xoffset,stamp_yoffset));
//			else
//			{	if(buttons == B_SECONDARY_MOUSE_BUTTON)
//					p_active_image->PaintBrush(where,false);
//			}
			p_active_image->ispainting=true;
			break;
		}
*/		case TOOL_ZOOM:
			GetMouse(&current,&buttons);
			if(buttons == B_PRIMARY_MOUSE_BUTTON)
				zoomin=true;
			else
				zoomin=false;
			break;

/*		case TOOL_MOVE:
			old_mouse_pos=where;
			break;
*/			
		// All these guys do previews, so save the initial point
		// for the preview
		case TOOL_ERASER:
		case TOOL_GRADIENT:
		case TOOL_LINE:
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_FRECT:
		case TOOL_FELLIPSE:
		case TOOL_SPLINE:
		{	
			if(openimages>0)
			{	old_mouse_pos.x=floor(where.x/p_active_image->zoom);
				old_mouse_pos.y=floor(where.y/p_active_image->zoom);
				new_mouse_pos=old_mouse_pos;

				GetMouse(&current,&buttons);
				if(buttons==B_PRIMARY_MOUSE_BUTTON)
					foremode=true;
				else
					foremode=false;
				if(p_active_image->brush_width > p_active_image->brush_height)
					SetPenSize(p_active_image->brush_width-1);
				else
					SetPenSize(p_active_image->brush_height-1);
				SetHighColor(p_active_image->forecolor);
				SetLowColor(p_active_image->backcolor);
			}
			break;
		}	
		case TOOL_FILL:
		{	
			if(openimages>0)
			{
				GetMouse(&current,&buttons);
				if(buttons==B_PRIMARY_MOUSE_BUTTON)
					foremode=true;
				else
					foremode=false;
			}
			break;
		}	
		case TOOL_EYEDROPPER:
		{
			if(openimages<1)
				break;

			GetMouse(&current,&buttons);
			where.x=floor(where.x/p_active_image->zoom);
			where.y=floor(where.y/p_active_image->zoom);
			
			if(buttons == B_PRIMARY_MOUSE_BUTTON)
				p_active_image->forecolor=p_active_image->GetPixel(where);	
			else
			{
				if(buttons == B_SECONDARY_MOUSE_BUTTON)
					p_active_image->backcolor=p_active_image->GetPixel(where);	
			}
			be_app->PostMessage(UPDATE_COLORS);
			break;
		}
		case TOOL_WAND:
			break;
		default:
			break;
	}

}

void PicView::MouseMoved(BPoint where, uint32 transit, const BMessage *dragdropmsg)
{
	BPoint current;
	uint32 buttons;
	switch(currenttool)
	{	
		case TOOL_BRUSH:
		{	GetMouse(&current,&buttons);
			if(mousedown)
			{	
//				GetMouse(&current,&buttons);
				if(buttons == B_PRIMARY_MOUSE_BUTTON)
					p_active_image->PaintBrush(where);	
				else
				{	if(buttons == B_SECONDARY_MOUSE_BUTTON)
						p_active_image->PaintBrush(where,false);
				}
			}
			break;
		}
		case TOOL_STAMP:
		{
/*			GetMouse(&current,&buttons);	
			if(mousedown)
			{	
				if(buttons == B_PRIMARY_MOUSE_BUTTON)
					p_active_image->Stamp(where,where+BPoint(stamp_xoffset,stamp_yoffset));
//				else
//				{	if(buttons == B_SECONDARY_MOUSE_BUTTON)
//						p_active_image->PaintBrush(where,false);
//				}
			}
*/
			Draw(stamp_rect);
			GetMouse(&current,&buttons,false);
			current.x+=stamp_xoffset;
			current.y+=stamp_yoffset;
			if(Bounds().Contains(current))
			{
				SetDrawingMode(B_OP_INVERT);
				SetHighColor(60,60,255);
				FillEllipse(current,10,10);
				stamp_rect.Set(current.x-15,current.y-15,
						current.x+15,current.y+15);
			}
			break;
		}
		case TOOL_EYEDROPPER:
		{
			if(openimages<1)
				break;
				
			GetMouse(&current,&buttons);
			where.x=floor(where.x/p_active_image->zoom);
			where.y=floor(where.y/p_active_image->zoom);
			
			if(buttons == B_PRIMARY_MOUSE_BUTTON)
			{	p_active_image->forecolor=p_active_image->GetPixel(where);	
				be_app->PostMessage(UPDATE_COLORS);
			}
			else
			{
				if(buttons == B_SECONDARY_MOUSE_BUTTON)
				{	p_active_image->backcolor=p_active_image->GetPixel(where);	
					be_app->PostMessage(UPDATE_COLORS);
				}
			}
			break;
		}
		case TOOL_GRADIENT:
		case TOOL_LINE:
		case TOOL_SPLINE:
			if(mousedown)
			{	
				BPoint lt,rb;
				if(old_mouse_pos.x<new_mouse_pos.x)
				{	lt.x=old_mouse_pos.x;
					rb.x=new_mouse_pos.x;
				}
				else
				{	rb.x=old_mouse_pos.x;
					lt.x=new_mouse_pos.x;
				}
				if(old_mouse_pos.y<new_mouse_pos.y)
				{	lt.y=old_mouse_pos.y;
					rb.y=new_mouse_pos.y;
				}
				else
				{	rb.y=old_mouse_pos.y;
					lt.y=new_mouse_pos.y;
				}
				old_preview_frame=BRect(lt,rb);
				old_preview_frame.InsetBy(0-(PenSize()*2),0-(PenSize()*2));
				Draw(old_preview_frame);
				
				new_mouse_pos.x=floor(where.x/p_active_image->zoom);
				new_mouse_pos.y=floor(where.y/p_active_image->zoom);
				SetHighColor(p_active_image->forecolor);
				SetLowColor(p_active_image->backcolor);
				StrokeLine(old_mouse_pos,new_mouse_pos,B_MIXED_COLORS);
			}
			break;

		case TOOL_ERASER:
		case TOOL_RECT:
		case TOOL_FRECT:
			if(mousedown)
			{	
				BPoint lt,rb;
				if(old_mouse_pos.x<new_mouse_pos.x)
				{	lt.x=old_mouse_pos.x;
					rb.x=new_mouse_pos.x;
				}
				else
				{	rb.x=old_mouse_pos.x;
					lt.x=new_mouse_pos.x;
				}
				if(old_mouse_pos.y<new_mouse_pos.y)
				{	lt.y=old_mouse_pos.y;
					rb.y=new_mouse_pos.y;
				}
				else
				{	rb.y=old_mouse_pos.y;
					lt.y=new_mouse_pos.y;
				}
				old_preview_frame=BRect(lt,rb);
				old_preview_frame.InsetBy(0-(PenSize()*2),0-(PenSize()*2));
				Draw(old_preview_frame);
				
				new_mouse_pos.x=floor(where.x/p_active_image->zoom);
				new_mouse_pos.y=floor(where.y/p_active_image->zoom);
				SetHighColor(p_active_image->forecolor);
				SetLowColor(p_active_image->backcolor);
				StrokeRect(BRect(lt,rb),B_MIXED_COLORS);
			}
			break;

		case TOOL_ELLIPSE:
		case TOOL_FELLIPSE:
			if(mousedown)
			{	
				BPoint lt,rb;
				if(old_mouse_pos.x<new_mouse_pos.x)
				{	lt.x=old_mouse_pos.x;
					rb.x=new_mouse_pos.x;
				}
				else
				{	rb.x=old_mouse_pos.x;
					lt.x=new_mouse_pos.x;
				}
				if(old_mouse_pos.y<new_mouse_pos.y)
				{	lt.y=old_mouse_pos.y;
					rb.y=new_mouse_pos.y;
				}
				else
				{	rb.y=old_mouse_pos.y;
					lt.y=new_mouse_pos.y;
				}
				old_preview_frame=BRect(lt,rb);
				old_preview_frame.InsetBy(0-(PenSize()*2),0-(PenSize()*2));
				Draw(old_preview_frame);
				
				new_mouse_pos.x=floor(where.x/p_active_image->zoom);
				new_mouse_pos.y=floor(where.y/p_active_image->zoom);
				SetHighColor(p_active_image->forecolor);
				SetLowColor(p_active_image->backcolor);
				StrokeEllipse(BRect(lt,rb),B_MIXED_COLORS);
			}
			break;

//		case TOOL_FILL:
//		case TOOL_TEXT:
		case TOOL_ZOOM:
		case TOOL_WAND:
			break;
/*		case TOOL_MOVE:
		{
			GetMouse(&current,&buttons);
			if(buttons == B_PRIMARY_MOUSE_BUTTON)
			{	p_active_image->p_active_layer->cliprect.OffsetBy(
							where.x-old_mouse_pos.x,
							where.y-old_mouse_pos.y	);
				p_active_image->UpdateDisplayImage(Bounds());
			}
			break;
		}
*/		default:
			break;
	}

}

void PicView::MouseUp(BPoint where)
{	

	if(mousedown==false)	// this prevents stray mouseup messages
		return;
		
	mousedown=false;
	if(IsHidden()!=true)
	{	
		p_active_image->ispainting=false;
		switch(currenttool)
		{	
			case TOOL_BRUSH:
				p_active_image->UpdateDisplayImage(extents);
//				Draw(extents);
				break;
			case TOOL_ZOOM:
				SetZoom(zoomin);
				break;
	
			case TOOL_LINE:
				where.x=floor(where.x/p_active_image->zoom);
				where.y=floor(where.y/p_active_image->zoom);
				p_active_image->PaintLine(old_mouse_pos,where,foremode);
				break;
	
			case TOOL_GRADIENT:
			{
				where.x=floor(where.x/p_active_image->zoom);
				where.y=floor(where.y/p_active_image->zoom);

				switch(gradientmode)
				{
					case GRADIENT_LINEAR:
						break;
					case GRADIENT_RADIAL:
						RadialGradient(old_mouse_pos,where);
						break;
					case GRADIENT_RECTANGULAR:
						RectGradient(old_mouse_pos,where);
						break;
				}
				break;
			}
			case TOOL_FILL:
			{	
				if(openimages>0)
				{
					where.x=floor(where.x/p_active_image->zoom);
					where.y=floor(where.y/p_active_image->zoom);
					be_app->PostMessage(SHOW_WAIT);
					if(foremode==true)
						p_active_image->FloodFill(where,p_active_image->forecolor);
					else
						p_active_image->FloodFill(where,p_active_image->backcolor);
					be_app->PostMessage(CLOSE_WAIT);
				}
				break;
			}	
			case TOOL_RECT:
				UninvertRect(&old_mouse_pos,&where);
				p_active_image->PaintRect(BRect(old_mouse_pos,where),foremode);
				break;
			case TOOL_FRECT:
				UninvertRect(&old_mouse_pos,&where);
				p_active_image->PaintFRect(BRect(old_mouse_pos,where),foremode);
				break;
			case TOOL_ELLIPSE:
				UninvertRect(&old_mouse_pos,&where);
				p_active_image->PaintEllipse(BRect(old_mouse_pos,where),foremode);
				break;
			case TOOL_FELLIPSE:
				UninvertRect(&old_mouse_pos,&where);
				p_active_image->PaintFEllipse(BRect(old_mouse_pos,where),foremode);
				break;
/*			case TOOL_ERASER:
			case TOOL_SPLINE:
				where.x=floor(where.x/p_active_image->zoom);
				where.y=floor(where.y/p_active_image->zoom);
				ApplyBeGraphics(old_mouse_pos,where,foremode);
				break;
*/				
			default:
				break;
		}
		p_active_image->UpdateDisplayImage(p_active_image->display_bitmap->Bounds());
		Draw();
	}
}

bool PicView::CreateNewImage(const char *namestr, uint16 imgwidth, uint16 imgheight)
{
	BPMImage *temp;
	
	// Should we even bother?
	if(openimages >254)
		return false;
		
	temp=new BPMImage(namestr,imgwidth,imgheight);
	
	// Do we have enough memory to do so?
	if(temp == NULL)
		return false;

	// We can afford to do this because openimages = last index + 1
	images[openimages]=temp;
	active_image=openimages;
	p_active_image=temp;
	openimages++;
	ResizeTo(imgwidth,imgheight);
	return true;
}

bool PicView::CreateNewImageFromBitmap(const char *namestr,BBitmap *bmp)
{
	BPMImage *temp;

	// Should we even bother?
	if(openimages >254)
		return false;
		
	// Check bitmap pointer
	if(bmp == NULL)
		return false;

	// Create new image and copy bitmap to bottom layer
	// +1 needed to add to Width() for some odd reason...

	temp=new BPMImage(namestr,bmp->Bounds().Width()+1,bmp->Bounds().Height());
	if(temp == NULL)
		return false;

	if(temp->layers[0]!=NULL)
	{	
		memcpy ((uint8*)temp->layers[0]->bitmap->Bits(),
					(uint8*)bmp->Bits(), bmp->BytesPerRow() * bmp->Bounds().IntegerHeight());
		memcpy ((uint8*)temp->display_bitmap->Bits(),
					(uint8*)bmp->Bits(), bmp->BytesPerRow() * bmp->Bounds().IntegerHeight());
	}
	
	// We can afford to do this because openimages = last index + 1
	images[openimages]=temp;
	active_image=openimages;
	p_active_image=temp;
	openimages++;
	ResizeTo(bmp->Bounds().Width(),bmp->Bounds().Height());
	return true;
}

bool PicView::CloseActiveImage(void)
{
	uint8 index=active_image;	// to save where we are

	if(openimages==0)
		return false;

	delete images[index];
	
	// Garbage collection if not the top image in the stack
	if(index != openimages-1)
	{	for(int8 i=index; i<openimages-1; i++)
			images[i]=images[i+1];
	}
	openimages--;
	
	// Make the nearest image active, if any
	if(openimages > 0)
	{	if(index==0)
			active_image=0;
		else
			active_image--;
	}
	p_active_image=images[active_image];
	return true;
}

void PicView::UpdateLayerWindow(void)
{	
	BMessage *layermsg;
	
	if(openimages>0)
		p_active_image->NotifyLayerWindow();
	else
	{	
		layermsg=new BMessage(ALTER_LAYERWINDOW_DATA);
		layermsg->AddInt8("number_layers",0);
		be_app->PostMessage(layermsg);
	}

}

void PicView::UpdateOptionWindow(void)
{	
	BMessage *optionmsg;
	optionmsg=new BMessage(ALTER_OPTIONWIN_DATA);
	uint32 data;
	data=currenttool;
	optionmsg->AddInt32("tool",currenttool);
	if(openimages>0)
	{	switch(currenttool)
		{	
			case TOOL_BRUSH:
			{	optionmsg->AddInt32("opacity",p_active_image->forecolor.alpha);
				optionmsg->AddInt32("mode",p_active_image->blendmode);
				break;
			}
			case TOOL_LINE:
			{	optionmsg->AddInt32("opacity",p_active_image->forecolor.alpha);
				optionmsg->AddInt32("mode",p_active_image->blendmode);
				break;
			}
			case TOOL_FILL:
			{	optionmsg->AddInt8("tolerance",p_active_image->tolerance);
				break;
			}
			case TOOL_GRADIENT:
			case TOOL_ZOOM:
			case TOOL_MOVE:
			case TOOL_ERASER:
			case TOOL_RECT:
			case TOOL_ELLIPSE:
			case TOOL_FRECT:
			case TOOL_FELLIPSE:
			case TOOL_SPLINE:
			case TOOL_TEXT:
			case TOOL_EYEDROPPER:
			case TOOL_STAMP:
			case TOOL_WAND:
				break;
			default:
				break;
		}
	}
	else
	{	optionmsg->AddInt32("opacity",255);
		optionmsg->AddInt32("mode",0);
	}
	be_app->PostMessage(optionmsg);
}

void PicView::SetZoom(bool mode)
{
	if(openimages<=0)
		return;

	// Set scale for view
	if(mode==true)	// if zooming in
	{	if(p_active_image->zoom > .5)
			p_active_image->zoom += .5;
		else
			p_active_image->zoom *= 2;
	}
	else
	{	if(p_active_image->zoom > .5)
			p_active_image->zoom -= .5;
		else
			p_active_image->zoom /= 2;
	}

	SetScale(p_active_image->zoom);
	ResizeTo(p_active_image->width * p_active_image->zoom,
				p_active_image->height * p_active_image->zoom);
	Window()->PostMessage(CENTER_VIEW);
	Draw();
}

void PicView::ClipboardCopy(void)
{
/*	BMessage *clip = NULL,*flattened;
	if (be_clipboard->Lock()) 
	{
		be_clipboard->Clear();
		if ((clip = be_clipboard->Data()))
		{
			flattened=Bitmap2Message(p_active_image->p_active_layer->bitmap);
			if(flattened!=NULL)
			{	flattened->what=B_OK;
				clip->AddMessage("image/bitmap",flattened);
				be_clipboard->Commit();
				delete flattened;
			}
		}
		be_clipboard->Unlock();
	}
*/
}

void PicView::ClipboardPasteAsNew(void)
{
/*	BMessage *clip = NULL,*flattened;
	BBitmap *bitmap;
	
	if (be_clipboard->Lock()) 
	{
		if ((clip = be_clipboard->Data()))
		{
			// Try BePhotoMagic/ArtPaint style storage
			if(clip->FindMessage("image/bitmap",flattened)!=B_NAME_NOT_FOUND)
			{
				printf("Found image on clipboard\n");
				const void *buffer;
				char *tempstr;
				int32 bufferLength;
				
				if(flattened->FindString("class",(const char **)&tempstr)==B_NAME_NOT_FOUND);
				{	printf("Couldn't get Class from flattened\n");
					return;
				}
					
				if (strcmp(tempstr,"BBitmap")==0)
				{
					bitmap = new BBitmap( flattened->FindRect("_frame"),
								(color_space) flattened->FindInt32("_cspace"),true );
					flattened->FindData("_data",B_RAW_TYPE,(const void **)&buffer, &bufferLength);
					memcpy(bitmap->Bits(), buffer, bufferLength);
				}
//				bitmap=Message2Bitmap(flattened);
				if(bitmap!=NULL)
				{
					CreateNewImageFromBitmap("",bitmap);
				}
				else
					printf("Couldn't create bitmap from clipboard\n");

			}
			else
				printf("No Image on clipboard\n");
		}
		be_clipboard->Unlock();
	}
*/
}

int32 PicView::ThreadUpdater(void *data)
{	BPoint pt;
	uint32 buttons;
	int32 bitwidth,bitheight;
	BRect brush_rect;
	bool erasemode,maskmode;

	PicView *view=(PicView *)data;

	// dimensions of bitmap for brush - NOT actual brush dimensions
	bitwidth=view->p_active_image->currentbrush->Bounds().IntegerWidth();
	bitheight=view->p_active_image->currentbrush->Bounds().IntegerHeight();

	bitwidth >>=1;
	bitheight >>=1;
	
	view->LockLooper();
	view->GetMouse(&pt,&buttons,false);
	pt.ConstrainTo(view->Bounds());

	erasemode=view->p_active_image->erasemode;
	maskmode=view->p_active_image->maskmode;

	pt.x = floor(pt.x / view->p_active_image->zoom);
	pt.y = floor(pt.y / view->p_active_image->zoom);
	view->extents.Set(pt.x,pt.y,pt.x+1,pt.y+1);
	view->UnlockLooper();

	while(buttons !=0)
	{	view->LockLooper();

		view->GetMouse(&pt,&buttons);

		pt.ConstrainTo(view->Bounds());
		
		pt.x = floor(pt.x / view->p_active_image->zoom);
		pt.y = floor(pt.y / view->p_active_image->zoom);

		brush_rect.left=pt.x-bitwidth-1;
		brush_rect.top=pt.y-bitheight-1;
		brush_rect.right=pt.x+bitwidth+1;
		brush_rect.bottom=pt.y+bitheight+1;
		
		view->extents.left=MIN(view->extents.left,brush_rect.left);
		view->extents.top=MIN(view->extents.top,brush_rect.top);
		view->extents.right=MAX(view->extents.right,brush_rect.right);
		view->extents.bottom=MAX(view->extents.bottom,brush_rect.bottom);

		view->p_active_image->UpdateDisplayImage(brush_rect);
//		view->DrawBitmap(view->p_active_image->display_bitmap,brush_rect,brush_rect);

		if(maskmode==false)
		{	view->SetDrawingMode(B_OP_ALPHA);
			view->DrawBitmap(view->p_active_image->p_active_layer->bitmap,brush_rect,brush_rect);
			view->SetDrawingMode(B_OP_COPY);
		}
		else
		{	view->SetDrawingMode(B_OP_ALPHA);
			view->DrawBitmap(view->p_active_image->selection,brush_rect,brush_rect);
			view->SetDrawingMode(B_OP_COPY);
		}
		view->GetMouse(&pt,&buttons,false);
		view->UnlockLooper();
	}
//	view->p_active_image->SaveUndo(UNDO_PAINTBRUSH,extents);
	view->mousedown=false;
	exit_thread(B_OK);
	return B_OK;
}

DesktopView::DesktopView(BRect rect, PicView *child) :
	BView(rect,"desktop",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{	childview=child;
}

DesktopView::~DesktopView(void)
{
}

void DesktopView::AttachedToWindow(void)
{
	// Leaving in this code in case we decide to add wallpapers into BPM
/*	BBitmap *backbitmap;
	backbitmap=BTranslationUtils::GetBitmap('bits',"back_text.png");
	SetViewBitmap(backbitmap,backbitmap->Bounds(),backbitmap->Bounds());
	delete backbitmap;
*/
}

void DesktopView::MouseUp(BPoint where)
{
	childview->MouseUp(BPoint(0,0));
}

void DesktopView::MouseDown(BPoint where)
{
	childview->MouseDown(BPoint(0,0));
}

void PicView::RadialGradient(BPoint startpt,BPoint endpt)
{
	rgb_color start=p_active_image->forecolor;
	rgb_color end=p_active_image->backcolor;
	float rstep=0.0,gstep=0.0,bstep=0.0,astep=0.0;
	
	int32 	i,				// index var
			x1,y1,x2,y2,	// int points for the line
			dx,dy,
			left,right,top,bottom,
			length;
	rgb_color currentcolor=start;

	BBitmap *bitmap=p_active_image->p_active_layer->bitmap;

	BRect viewbounds=Bounds();
	left=int32(viewbounds.left);
	top=int32(viewbounds.top);
	right=int32(viewbounds.right);
	bottom=int32(viewbounds.bottom);

	// we're doing integer math here, going left to right
	if(startpt.x<endpt.x)
	{	x1=int32(startpt.x);
		x2=int32(endpt.x);
		y1=int32(startpt.y);
		y2=int32(endpt.y);
	}
	else
	{	x1=int32(endpt.x);
		x2=int32(startpt.x);
		y1=int32(endpt.y);
		y2=int32(startpt.y);
	}

	// Short ciruit in special cases
	if(x1==x2 && y1==y1)	// pointless case
		return;

	dx=x2-x1;
	dy=y2-y1;

	// Calculate radius of gradient	
	length=int32(sqrt((dx*dx)+(dy*dy)));
		
	// get step size from beginning color to end color
	rstep=float(end.red-start.red)/length;
	gstep=float(end.green-start.green)/length;
	bstep=float(end.blue-start.blue)/length;
	astep=float(end.alpha-start.alpha)/length;
	
	SetPenSize(10);
	SetDrawingMode(B_OP_ALPHA);

	BView draw(bitmap->Bounds(),"bmpdraw",B_FOLLOW_NONE,B_WILL_DRAW);
	bitmap->Lock();
	bitmap->AddChild(&draw);
	
	// loop to actually draw the gradient
	for(i=0;i<=length;i++)
	{
		draw.SetHighColor(currentcolor);
		draw.StrokeEllipse(startpt,i,i);
		currentcolor.red=uint8(start.red+(float(i)*rstep));
		currentcolor.green=uint8(start.green+(float(i)*gstep));
		currentcolor.blue=uint8(start.blue+(float(i)*bstep));
		currentcolor.alpha=uint8(start.alpha+(float(i)*astep));
	}
	draw.RemoveSelf();
	bitmap->Unlock();
	
}

void PicView::RectGradient(BPoint startpt,BPoint endpt)
{
	float rstep=0.0,gstep=0.0,bstep=0.0,astep=0.0;
	
	uint32 	i,				// index var
			length;
	rgb_color start=p_active_image->forecolor;
	rgb_color end=p_active_image->backcolor;
	rgb_color currentcolor=end;

	// Uninvert the rectangle if necessary
	BRect looprect(startpt,endpt);
	if(looprect.IsValid()==false)
	{	float temp;
		temp=MAX(looprect.left,looprect.right);
		looprect.left=MIN(looprect.left,looprect.right);
		looprect.right=temp;
		
		temp=MAX(looprect.top,looprect.bottom);
		looprect.top=MIN(looprect.top,looprect.bottom);
		looprect.bottom=temp;
	}

	BBitmap *bitmap=p_active_image->p_active_layer->bitmap;

	// get step size from beginning color to end color
	length=MIN(abs(looprect.IntegerWidth()),abs(looprect.IntegerHeight()));

	rstep=float(end.red-start.red)/(length>>1);
	gstep=float(end.green-start.green)/(length>>1);
	bstep=float(end.blue-start.blue)/(length>>1);
	astep=float(end.alpha-start.alpha)/(length>>1);
	
	SetPenSize(2);

	SetDrawingMode(B_OP_ALPHA);
	// loop to actually draw the gradient
	i=0;

	BView draw(bitmap->Bounds(),"bmpdraw",B_FOLLOW_NONE,B_WILL_DRAW);
	bitmap->Lock();
	bitmap->AddChild(&draw);
	
	while(looprect.IntegerWidth()>0 && looprect.IntegerWidth()>0)
	{	
		draw.SetHighColor(currentcolor);
		draw.StrokeRect(looprect);
		looprect.InsetBy(1,1);
		
		currentcolor.red=uint8(end.red-(float(i)*rstep));
		currentcolor.green=uint8(end.green-(float(i)*gstep));
		currentcolor.blue=uint8(end.blue-(float(i)*bstep));
		currentcolor.alpha=uint8(end.alpha-(float(i)*astep));
		i++;
	}
	draw.RemoveSelf();
	bitmap->Unlock();
}
