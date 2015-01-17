#include "BPMImage.h"

extern BWindow *bpmwindow;

#ifndef MIN
	#define MIN(a,b) (a<b)?a:b
#endif
#ifndef MAX
	#define MAX(a,b) (a>b)?a:b
#endif

BPMImage::BPMImage(void)
{	
	sprintf(name," ");
	untitled=0;
	number_layers=0;
	active_layer=0;
	zoom=1.0;
	tolerance=0;
	
	width=0;
	height=0;

	brush_width=16;
	brush_height=16;
	brush_softness=0;
	brushtype=BRUSH_TYPE_ELLIPSE;
	brushblend=TNORMAL;
	blendmode=TNORMAL;
	linestep=1;
	brushalpha=255;
	linealpha=255;
	userbrush=false;
	ispainting=false;
	save_each_paint=true;
	typecode=0;

	forecolor.red=255;
	forecolor.green=255;
	forecolor.blue=255;
	forecolor.alpha=255;
	backcolor.red=0;
	backcolor.green=0;
	backcolor.blue=0;
	backcolor.alpha=255;
	path.SetTo("");
	undolist=new BList(255);
	maskmode=false;
	erasemode=false;
	tolerance=0;
}

BPMImage::BPMImage(const char *namestr, int16 imgwidth, int16 imgheight)
{
	if (namestr != NULL)
		sprintf(name, namestr);
	else 
		sprintf(name," ");
	
	untitled=0;
	number_layers=0;
	active_layer=0;
	zoom=1.0;
	
	width=(imgwidth<1)?1:imgwidth;
	height=(imgheight<1)?1:imgheight;

	BRect bitmaprect(0,0,width-1,height-1);
	display_bitmap=new BBitmap(bitmaprect,B_RGBA32,true);
	selection=new BBitmap(bitmaprect,B_RGBA32,true);
//	memset(selection->Bits(),128,selection->BitsLength());
	SelectAll();
	work_bitmap=new BBitmap(bitmaprect,B_RGBA32);
	memset(work_bitmap->Bits(),0,work_bitmap->BitsLength());	// set to completely transparent
	currentbrush=new BBitmap(BRect(0,0,31,31),B_RGBA32);
	CreateNewLayer(SpTranslate("Background"),width,height);

	bottomlayer=new BBitmap(BRect(0,0,1,1),B_RGB32);
	toplayer=new BBitmap(BRect(0,0,1,1),B_RGB32);

	brush_width=16;
	brush_height=16;
	brush_softness=0;
	brushtype=BRUSH_TYPE_ELLIPSE;
	brushblend=TNORMAL;
	blendmode=TNORMAL;
	stampblend=TNORMAL;
	linestep=1;
	brushalpha=255;
	linealpha=255;
	stampalpha=255;
	userbrush=false;
	ispainting=false;
	save_each_paint=true;
	typecode=0;

	forecolor.red=255;
	forecolor.green=255;
	forecolor.blue=255;
	forecolor.alpha=255;
	backcolor.red=0;
	backcolor.green=0;
	backcolor.blue=0;
	backcolor.alpha=255;
	MakeBrush(brush_width,brush_height,brush_softness,brushtype);
	path.SetTo("");
	undolist=new BList(255);
	maskmode=false;
	erasemode=false;
	tolerance=0;
}


BPMImage::~BPMImage()
{
	if(width==0 || height==0)
		return;
	if(display_bitmap != NULL)
		delete display_bitmap;
		
	if(work_bitmap != NULL)
		delete work_bitmap;
		
	if(selection != NULL)
		delete selection;
		
	if(undolist->CountItems()!=0)
	{	int32 i;
		UndoData *cdata;
		
		for(i=0;i<undolist->CountItems();i++)
		{	
			cdata=(UndoData *)undolist->ItemAt(i);
			if(cdata!=NULL)
				delete cdata;
		}
		undolist->MakeEmpty();
	}
	if(currentbrush != NULL)
		delete currentbrush;
	
	for(uint8 i=0; i<number_layers; i++)
		delete layers[i];
	
	if(toplayer!=NULL)
		delete toplayer;
	if(bottomlayer!=NULL)
		delete bottomlayer;
}

bpm_status BPMImage::CreateNewLayer(const char *namestr, int16 laywidth, int16 layheight)
{	
	char tempstr[255];
	Layer *templayer;
	int16 i;
		
	if(number_layers>254)
		return BPM_MAX_LAYERS;
		
	if (namestr != NULL)
		sprintf(tempstr, namestr);
	else 
		sprintf(tempstr," ");
	
	width=(laywidth<1)?1:laywidth;
	height=(layheight<1)?1:layheight;

	// Insert the layer into the stack right above current layer if not last element
	templayer=new Layer(tempstr,width,height,untitled++);

	if(templayer == NULL)
		return BPM_BAD_ALLOC;

	if( (number_layers > 1) && (active_layer != number_layers-1) )
	{	// Move everything up one to make room for new layer
		for(i=number_layers; i>active_layer+1; i--)
			layers[i]=layers[i-1];
	}

	if(number_layers>0)
	{	layers[active_layer+1]=templayer;
		number_layers++;
		active_layer++;
		p_active_layer=layers[active_layer];
		memset((void*)p_active_layer->bitmap->Bits(),0,p_active_layer->bitmap->BitsLength()); // make transparent
	}
	else
	{	layers[0]=templayer;		
		number_layers++;
		p_active_layer=layers[active_layer];
	}	
	UpdateDisplayImage(display_bitmap->Bounds());
	NotifyLayerWindow();
	return BPM_OK;
}

bpm_status BPMImage::NewLayerFromBitmap(const char *namestr, BBitmap *bmp)
{	// Given a new bitmap, makes a layer which is an exact copy of the layer
	// Any bitmaps passed to it must be deleted by the caller

	bpm_status lstat;

	if(bmp==NULL)
		return BPM_BAD_PARAMETER;
		
	lstat=CreateNewLayer(namestr, bmp->Bounds().IntegerWidth(), bmp->Bounds().IntegerHeight());

	if(lstat!=BPM_OK)	// if Create died, return the error code
		return lstat;
		
	// We created a new layer, so make a copy of the bitmap and kill the blank layer

	// After a Create(), new layer is active
	delete p_active_layer->bitmap;
	p_active_layer->bitmap=new BBitmap(bmp);
	
	UpdateDisplayImage(display_bitmap->Bounds());
	NotifyLayerWindow();
	return BPM_OK;

}

bool BPMImage::DeleteLayer(void)
{	// Delete active layer
	if(number_layers==1)
		return false;

	uint8 index=active_layer;	// to save where we are
	delete layers[index];
	
	// Garbage collection if not the top image in the stack
	if(index != number_layers-1)
	{	for(int8 i=index; i<number_layers-1; i++)
			layers[i]=layers[i+1];
	}
	number_layers--;
	
	// Make the nearest image active, if any
	if(index==0)
		active_layer=0;
	else
		active_layer--;
	p_active_layer=layers[active_layer];

	NotifyLayerWindow();
	UpdateDisplayImage(display_bitmap->Bounds());
	return true;
}

void BPMImage::SwapLayers(uint16 first, uint16 second)
{	Layer *temp;

	if( (first > number_layers-1) || (second > number_layers-1) || (second == first) )
		return;
	temp=layers[first];
	layers[first]=layers[second];
	layers[second]=temp;
	UpdateDisplayImage(display_bitmap->Bounds());
}

void BPMImage::FlattenImage(void)
{
	if(number_layers==1)
		return;

	// Nuke the layer stack, save the bottom one
	for(uint16 i=1;i<number_layers;i++)
		delete layers[i];
	
	active_layer=0;
	number_layers=1;
	p_active_layer=layers[0];
	
	// Replace the layer's bitmap with our readymade composite - the display image
	delete layers[0]->bitmap;
	layers[0]->bitmap=new BBitmap(display_bitmap);
	
	NotifyLayerWindow();
	UpdateDisplayImage(display_bitmap->Bounds());
}

void BPMImage::MakeComposite(void)
{	// make a new layer out of the flattened image
	char layername[255];
	sprintf(layername,"Layer %d",untitled++);
	NewLayerFromBitmap(layername, display_bitmap);
}

void BPMImage::DuplicateLayer(void)
{	// Make a duplicate of the active layer
	char layername[255];
	sprintf(layername,"Layer %d",untitled++);
	NewLayerFromBitmap(layername, p_active_layer->bitmap);
}

void BPMImage::MergeDown(void)
{	// Merge active layer into one below it
	if(active_layer==0)
		return;
	ApplyBitmap(p_active_layer->bitmap,p_active_layer->bitmap->Bounds(),
		layers[active_layer-1]->bitmap,layers[active_layer-1]->bitmap->Bounds(),
		p_active_layer->blendmode);
	DeleteLayer();
	NotifyLayerWindow();
	UpdateDisplayImage(display_bitmap->Bounds());
}

void BPMImage::MakeBrush(int16 width, int16 height,int8 softness, int8 type)
{	
	if (width < 1) 
		width=1;
	if (height<1)
		height=1;
	
	BRect rect; 
	rect.Set(0,0,width-1,height-1);
	
	BBitmap *tmp_brush = new BBitmap(rect,B_RGB32,true);

	tmp_brush->Lock(); //important!
	BView virtualView( tmp_brush->Bounds(), NULL, B_FOLLOW_NONE, 0 );
	tmp_brush->AddChild( &virtualView );

   	//draw the brush
   	float tmp_width = floor(width/2); //get radius
   	float tmp_height = floor(height/2);
   	int16 val = 255;
   	float centerx = tmp_width;
   	float centery = tmp_height;
   	float step;
   	if(softness==0)
   		step=255;
   	else
   		step=255/softness;
  
  	BPicture *my_pict; 
  	virtualView.BeginPicture(new BPicture);

	if(type==BRUSH_TYPE_RECT)
	{	// Set up the rectangle
		BRect *work_rect=new BRect(tmp_brush->Bounds());
	   	do 
		{
			// starts out white and goes to black
  			virtualView.SetHighColor(val,val,val);
   			virtualView.FillRect(*work_rect,B_SOLID_HIGH);
	   		val -= (int16) step;
   			if (val < 0) 
   				val=0;

			// kept around just for the loop
	   		if(tmp_width>-1)
		   		tmp_width--;
   			if(tmp_height>-1)
  				tmp_height--;

			work_rect->InsetBy(1,1);   			
		} while ( (tmp_width != -1) && (tmp_height != -1) );

		delete work_rect;
	}
	else
	{
	   	do 
		{
			// starts out white and goes to black
  			virtualView.SetHighColor(val,val,val);
   			virtualView.FillEllipse(BPoint(centerx,centery),tmp_width,tmp_height,B_SOLID_HIGH);
	   		val -= (int16) step;
   			if (val < 0) 
   				val=0;
	   		if(tmp_width>-1)
		   		tmp_width--;
   			if(tmp_height>-1)
  				tmp_height--;
   			
		} while ( (tmp_width != -1) && (tmp_height != -1) );
	}

   	my_pict = virtualView.EndPicture();
   	virtualView.DrawPicture(my_pict);
	tmp_brush->RemoveChild( &virtualView );
	tmp_brush->Unlock();
	
	brush_width=width;
	brush_height=height;
	brush_softness=softness;
	brushtype=type;

	if(currentbrush != NULL)
		delete currentbrush;
	currentbrush=tmp_brush;
}

void BPMImage::NotifyLayerWindow(void)
{
	BMessage *layermsg;
	layermsg=new BMessage(ALTER_LAYERWINDOW_DATA);
	
	layermsg->AddInt8("number_layers",number_layers);
	for(uint8 i=0;i<number_layers;i++)
	{	if(layers[i]->visible==false)
			layermsg->AddInt8("hidden_layer",i);
	}
	layermsg->AddInt8("active_layer",active_layer);
	layermsg->AddInt8("blendmode",p_active_layer->blendmode);
	be_app->PostMessage(layermsg);
}

bpm_status BPMImage::SaveUndo(uint8 type,BRect saverect)
{
	UndoData *newundo;
	switch(type & 254)	// get just the layer type - not the mask flag
	{
		case UNDO_PAINTBRUSH:	// for paintbrush tool only
		case UNDO_TOOL:
		{	
			if(undolist->CountItems()==MAX_UNDO)
			{
				delete (UndoData *)undolist->ItemAt(0);
			}
			newundo=new UndoData();
			undolist->AddItem(newundo);
			
			saverect.right++;
			if(!(p_active_layer->bitmap->Bounds().Contains(saverect)))
			{	saverect.LeftTop().ConstrainTo(p_active_layer->bitmap->Bounds());
				saverect.RightBottom().ConstrainTo(p_active_layer->bitmap->Bounds());
			}
			newundo->rect=saverect;

			// When mask mode is implemented, will need to check
			// for the mask flag here, in addition to the existing code
			newundo->layerindex=active_layer;
			saverect.OffsetTo(0,0);

			newundo->bitmap=new BBitmap(saverect,B_RGBA32);
			newundo->type=UNDO_TOOL;

			CopyRect(newundo->rect,p_active_layer->bitmap,newundo->bitmap->Bounds(),newundo->bitmap,4);
			break;
		}	
		case UNDO_BRUSH:	// brush changes
		{	
			if(undolist->CountItems()==MAX_UNDO)
			{
				delete (UndoData *)undolist->ItemAt(0);
			}
			newundo=new UndoData();
			undolist->AddItem(newundo);
			if(userbrush==false)
			{	newundo->brush_width=brush_width;
				newundo->brush_height=brush_height;
				newundo->brushtype=brushtype;
				newundo->brush_softness=brush_softness;
				newundo->userbrush=false;
			}
			else
			{	newundo->brush_width=0;
				newundo->brush_height=0;
				newundo->brushtype=0;
				newundo->brush_softness=0;
				newundo->userbrush=true;
			}
			newundo->bitmap=new BBitmap(currentbrush);
			newundo->type=UNDO_BRUSH;
			break;
		}	
		case UNDO_LAYER:	// single-layer operations
		{
			if(undolist->CountItems()==MAX_UNDO)
				delete (UndoData *)undolist->ItemAt(0);
			newundo=new UndoData();
			undolist->AddItem(newundo);

			// Layer save operations here

			newundo->type=UNDO_LAYER;
			break;
		}
		case UNDO_TRANSFORM:	// image was resized, rotated,etc.
			break;
		default:
			return BPM_BAD_PARAMETER;
			break;
	}
	return BPM_OK;
}

bpm_status BPMImage::Undo(void)
{
	if(undolist->CountItems()==0)
		return BPM_OK;

	UndoData *cdata=(UndoData *)undolist->LastItem();

	switch(cdata->type & 254)
	{
		case UNDO_PAINTBRUSH:
		case UNDO_TOOL:
		{	// Need to add a check for the UNDO_MASK flag when mask mode implemented

			if(cdata->bitmap==NULL)
				return BPM_BAD_ALLOC;
				
			CopyRect(cdata->bitmap->Bounds(),cdata->bitmap,
						cdata->rect,p_active_layer->bitmap,4);
			UpdateDisplayImage(cdata->rect);

			// This will need to be changed when/if Redo() is implemented
			delete cdata;
			undolist->RemoveItem(undolist->LastItem());
			break;
		}
		case UNDO_BRUSH:
		{
			delete currentbrush;
			currentbrush=new BBitmap(cdata->bitmap);
			if(cdata->userbrush==false)
			{	BMessage *bmsg=new BMessage(SET_BRUSH_CONTROLS);
				bmsg->AddInt8("shape",cdata->brushtype);
				bmsg->AddInt16("width",cdata->brush_width);
				bmsg->AddInt16("height",cdata->brush_height);
				bmsg->AddInt8("softness",cdata->brush_softness);

				userbrush=false;
				brushtype=cdata->brushtype;
				brush_width=cdata->brush_width;
				brush_height=cdata->brush_height;
				brush_softness=cdata->brush_softness;
				
				bpmwindow->PostMessage(bmsg);
			}
			else
			{	userbrush=true;
				delete currentbrush;
				currentbrush=new BBitmap(cdata->bitmap);
			}
			be_app->PostMessage(UPDATE_BRUSH);
			delete cdata;
			undolist->RemoveItem(undolist->LastItem());
			break;
		}
		case UNDO_LAYER:
		{
			// Basically does nothing for the moment...
			delete cdata;
			undolist->RemoveItem(undolist->LastItem());
			break;
		}
			
		case UNDO_TRANSFORM:
			// Do nothing for now...
			break;

		default:
			return BPM_INTERNAL_ERROR;	// bad undodata type
	}	
	return BPM_OK;
}

void BPMImage::ClearUndo(void)
{
	if(undolist->CountItems()!=0)
	{	int32 i;
		for(i=0;i<undolist->CountItems();i++)
		{	delete (UndoData *)undolist->ItemAt(i);
		}
		undolist->MakeEmpty();
	}
}

BRect BPMImage::GetBrushedRect(BPoint pt,bool zoom_comp=true)
{	// Given a point, this will return the rectangle that would be painted by
	// a Paintbrush call, optionally with zoom compensation
	if(zoom_comp)
	{	pt.x = floor(pt.x / zoom);
		pt.y = floor(pt.y / zoom);
	}
	pt.ConstrainTo(display_bitmap->Bounds());

	int32 	x=int32(pt.x),
			y=int32(pt.y);

	// data vars for brush and for bitmap
	uint32 bitheight,bitwidth,lclip,rclip,tclip,bclip;
	BRect brush_rect,brushbounds,bmpbounds,workbounds,undorect;

	brushbounds=currentbrush->Bounds();
	bmpbounds=display_bitmap->Bounds();
	workbounds=work_bitmap->Bounds();
	
	// dimensions of bitmap for brush - NOT actual brush dimensions
	bitwidth=currentbrush->Bounds().IntegerWidth();
	bitheight=currentbrush->Bounds().IntegerHeight();

	// jump by offset so that brush's center is where we clicked
	x -=int32(bitwidth/2);
	y -=int32(bitheight/2);
	
	// Get brush dimensions in image
	brush_rect.left=x;
	brush_rect.top=y;
	brush_rect.right=x+bitwidth;
	brush_rect.bottom=y+bitheight;
	undorect=brush_rect;
	
	// Assign brush bitmap clipping rectangle proper values
	tclip=lclip=0;
	rclip=(int32)brushbounds.right;
	bclip=(int32)brushbounds.bottom;

	if(brush_rect.left < 0)
	{	lclip=int32(brush_rect.left * -1);
		undorect.left=0;
	}
	if(brush_rect.top < 0)
	{	tclip=int32(brush_rect.top * -1);
		undorect.top=0;
	}
	if(brush_rect.right > bmpbounds.right)
	{	rclip -= int32(brush_rect.right-bmpbounds.right);
		undorect.right=bmpbounds.right;
	}
	if(brush_rect.bottom > bmpbounds.bottom)
	{	bclip -= int32(brush_rect.bottom-bmpbounds.bottom);
		undorect.bottom=bmpbounds.bottom;
	}
	return undorect;
}

void BPMImage::SelectAll(void)
{
	BView dview(selection->Bounds(),"dview",B_FOLLOW_NONE,B_WILL_DRAW);
	selection->Lock();
	selection->AddChild(&dview);
	dview.SetHighColor(128,128,128,0);
	dview.FillRect(dview.Bounds());
	dview.RemoveSelf();
	selection->Unlock();
}

void BPMImage::SelectNone(void)
{
	BView dview(selection->Bounds(),"dview",B_FOLLOW_NONE,B_WILL_DRAW);
	selection->Lock();
	selection->AddChild(&dview);
	dview.SetHighColor(128,128,128,255);
	dview.FillRect(dview.Bounds());
	dview.RemoveSelf();
	selection->Unlock();
}

void BPMImage::PrintToStream(void)
{
	printf("---------------------------------------------\n");
	printf("BPMImage %s (%ld x %ld)\n",name,width,height);
	printf("Foreground: RBGA(%d %d %d %d)\n",forecolor.red,forecolor.green,forecolor.blue,forecolor.alpha);
	printf("Background: RBGA(%d %d %d %d)\n",backcolor.red,backcolor.green,backcolor.blue,backcolor.alpha);
	printf("Fill tolerance: %d\nActive layer: %d\n",tolerance,active_layer);
	printf("Number of layers: %d\nZoom: %f\n",number_layers,zoom);
	printf("Path: %s\nMIME string: %s",path.String(),MIME.String());
	printf("Mask mode: %d\nErase mode: %d\n",maskmode,erasemode);
	printf("---------------------------------------------\n");
}
