#include "share.h"

BubbleHelper bb_help; //an extern

share::share()
{
//SETTINGS
act_img=NULL;
active_image=-1;
image_amount=-1; //at creation, the first gets a 0
   
stamp_offset = BPoint (64,64);
stamp_offset_changed = OFF;
paint_mode=NORMAL;
paint_transparency=0; //values from 0-1, 1 == 100%

sprintf(nom_de_brush_perso,"no name for this brush...");


// These values must be initialized before the creation of the image even if they
// are replaced at the time of this one - the display needs some before... 

pattern st = { 0xc7, 0x8f, 0x1f, 0x3e, 0x7c, 0xf8, 0xf1, 0xe3 };
stripes = st;

needs_update = false;
first_zone = true;
   
   
stroke=OFF;

current_brush=16;
current_perso_brush=0;
current_paper=0;
max_brush=2;
max_paper=2;


active_tool=PAINTBRUSH;


rgb_color a = {255,255,255,0};
rgb_color b = {0,0,0,0};

fore_color = a;
back_color = b;

// These selections should be part of the preferences later -- DarkWyrm
low_select_color.red = 0;
low_select_color.green = 0;
low_select_color.blue = 0;
low_select_color.alpha = 0;

high_select_color.red= 255;
high_select_color.green= 255;
high_select_color.blue= 255;
high_select_color.alpha= 0;
	
//cursor_ptr = &simple_cross[0];

active_translator=0;
hardness =0;
brush_is_perso=false;

// zero out pointers for images just for safety
int32 i;
for(i=0;i<IMAGE_NUMBER_MAX;i++)
	les_images[i]=NULL;

}


void share::EmptyPaper()
{
//create a 24 x 24 paper
BBitmap *temp = new BBitmap(BRect(0,0,24-1,24-1),B_RGB32);
UseAsPaper(temp);

}			


void share::CreateNewImage(const char *le_nom, int16 w, int16 h)
{
/*
int8 stop_it = OFF;	
int16 was_active = active_image; //for restoring if needed
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0;
*/
	ThePrefs.no_pictures_left=ON; //prevent display during loading

	image_amount++;
	active_image=image_amount;
	les_images[active_image]= new BPMImage(le_nom,w,h); 
	act_img = les_images[active_image];
	act_img->CreateNewLayer(NULL);
	act_lay = act_img->the_layers[act_img->layer_amount];
	
	ImageAdded();
	
}	

void share::LoadNewImage(const char *fichier)
{
ThePrefs.no_pictures_left=ON;	 //reactivated in Initpic
/*
int8 stop_it = OFF;	
int16 was_active = active_image; //for restoration if needed
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0;
*/
	image_amount++;  active_image=image_amount;
	les_images[active_image]= new BPMImage("",32,32); 
	act_img = les_images[active_image];
	if (act_img->LoadNewLayer(fichier) != B_ERROR)
	{
		act_lay = act_img->the_layers[act_img->layer_amount];
		ImageAdded();
	}
	else DeleteImage(image_amount);

}	

void share::NewImageFromBmp(const char *nm, BBitmap *pic)
{

/*
int8 stop_it = OFF;	
int16 was_active = active_image;
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0;
*/

ThePrefs.no_pictures_left=ON; //prevent display during loading

	image_amount++;
	active_image=image_amount;
	les_images[active_image]= new BPMImage(nm,32,32); 
	act_img = les_images[active_image];
	act_img->NewLayerFromBmp(pic);
	act_lay = act_img->the_layers[act_img->layer_amount];
	
	ImageAdded();
	
	
	
}	

void share::DeleteImage(int32 which_one)
{
ThePrefs.no_pictures_left=ON; //prevent display while deleting
bool reactivate =false;

			util.mainWin->Lock();
			if (win_menu->ItemAt(which_one)!=NULL)	win_menu->RemoveItem(which_one);

		BPMImage *old_active =les_images[which_one]; //for deleting later
								
		if (which_one!=image_amount) //if not the last one
			{
			reactivate=true;
			//shift images
			int16 i=which_one;
			
			do {
				    if (les_images[i+1]!=NULL)	les_images[i] = les_images[i+1];   
				    i++;
			    } while (i!= image_amount);
				
			if (win_menu->ItemAt(which_one)!=NULL) win_menu->ItemAt(which_one)->SetMarked(true);
			}
		else 
				which_one=image_amount-1; 	//is the last image
			
			image_amount--;
			
			if (image_amount <0) {ThePrefs.no_pictures_left=ON; util.mainWin->PostMessage(DISABLE_ALL);}
			else win_menu->ItemAt(which_one)->SetMarked(true);
			
			delete old_active;		

			if (reactivate==true) ThePrefs.no_pictures_left=OFF; //reactivate display

			util.mainWin->PostMessage(IMAGE_CHOSEN); //redraw newly active image
			util.mainWin->Unlock();


}
  				

void share::ImageAdded()
{
 	act_img->undo_bitmap = new BBitmap(act_lay->img->Bounds(),B_RGB32,true,false); //accept views
	
	act_img->mask_bitmap 	   = new BBitmap(act_lay->img->Bounds(),B_GRAY8);
	act_img->mask_undo_bitmap  = new BBitmap(act_lay->img->Bounds(),B_GRAY8,true,false); //accept views 
	act_img->display_bitmap 	   = new BBitmap(act_lay->img->Bounds(),B_RGB32); //in rgb for display
		
	initPic();

	act_img->DeleteMask(OFF);
	act_img->UpdateDisplayImg(act_img->undo_bitmap->Bounds());
	
	win_menu->AddItem(new BMenuItem(act_img->name,   new BMessage(IMAGE_CHOSEN)));
	win_menu->ItemAt(active_image)->SetMarked(true);
	
	 
}	


void share:: CopyBitmaptoWork() 
{
	uint8 *dest  = (uint8 *) act_img->undo_bitmap->Bits();
	uint8 *src = (uint8 *)   act_lay->img->Bits();
	uint32 taille = act_img->undo_bitmap->BitsLength();
  			
	memcpy(dest,src,taille);
}

void share::initPic() //done with creation, loading, or changing of active image
{

	act_img = les_images[active_image];
	act_lay = act_img->the_layers[act_img->active_layer];

	
	// Couldn't make heads or tails of the meaning of this one...
	//si on fait plus un là il y une merde de décalage mais à mon avis
	//le problème est ailleurs vu que là il devrait pas y avoir autrement de +1
	act_img->pix_per_line = int16 (act_lay->img->Bounds().Width()+1);
	act_img->pix_per_row  = int16 (act_lay->img->Bounds().Height()+1);


	initLayer();

	//need to redo this - determines which filters are supported for the active layer
	SetManipMenus();
        
	ThePrefs.no_pictures_left=OFF; 
	util.mainWin->PostMessage(ENABLE_ALL);
	
	util.mainWin->PostMessage(new BMessage(PIC_CREATED));
	util.mainWin->PostMessage(new BMessage(ZOOM_CHANGED));
	 

	// Simpler to just close & reopen
	if (ThePrefs.layer_selector_open==true)
	{
		util.layerWin->Lock();
		util.layerWin->Close();
		util.mainWin->PostMessage(SHOW_LAYER_WIN);
	}
	

	/*
		BMessage x(INIT_PERSO);
		x.AddInt32("active",0);
		util.layerWin->PostMessage(&x);
	*/	

	
}

void share::initLayer() //called after creating, loading, & changing of a layer
{
	ThePrefs.no_pictures_left=ON; //prevent display while loading
	
    the_bitmap_bits    = (uint8*) act_lay->img->Bits();
	undo_bitmap_bits   = (uint8*) act_img->undo_bitmap->Bits();
	mask_bits 		   = (uint8*) act_img->mask_bitmap->Bits();
	mask_work_bits     = (uint8*) act_img->mask_undo_bitmap->Bits();


	CopyBitmaptoWork();
	act_img->UpdateDisplayImg(act_img->the_layers[0]->img->Bounds());
	
	ThePrefs.no_pictures_left=OFF; //reactivate display


	//update layers if necessary
	if (ThePrefs.layer_selector_open==true)
	{
		BMessage msg(DRAW_LAYERS);
		msg.AddInt32("active",act_lay->id);
		util.layerWin->PostMessage(&msg);		
	}		

/*
	BMessage tmp(UPDATE_ME);
	tmp.AddRect("zone",BRect(0,0,act_img->pix_per_line-1,act_img->pix_per_line-1));
 	util.mainWin->PostMessage(&tmp);
 	*/
 	
}
    
void share::loadBrush(int16 numbrush)
{ 

	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/brushes/");
	
	//get number of brushes in directory
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	max_brush = the_dir.CountEntries();
//	printf("\nnb brushes: %d",max_brush);

	if (numbrush <= 0) 	numbrush = 0;
	if (numbrush >= max_brush) numbrush = max_brush;
	current_perso_brush = numbrush;
	
	BEntry the_entry;	

	uint16 i = 0;
	do { the_dir.GetNextEntry(&the_entry,false);   i++;  } while (i!=current_perso_brush+1);
	the_entry.GetName(tmp);
	strcat (name,tmp);

	
	BBitmap *temp_32;
    if ( (temp_32 = util.load_bmp(name)) != NULL) 
    { UseAsBrush(temp_32);
 	  sprintf(nom_de_brush_perso,tmp);
    }  //load image & convert to 8-bit
    
   	brush_is_perso=true;

}

    
void share::loadPaper(int16 numpaper)
{ 
	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/papers/");
	
	//get # of papers in directory
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	max_paper = the_dir.CountEntries();

	if (numpaper <= 0) 	numpaper = 0;
	if (numpaper >= max_paper) numpaper = max_paper;
	current_paper = numpaper;
	
	BEntry the_entry;	

	uint16 i = 0;
	do { the_dir.GetNextEntry(&the_entry,false);   i++;  } while (i!=current_paper+1);
	the_entry.GetName(tmp);
	strcat (name,tmp);
	
	BBitmap *temp_32;
    if ( (temp_32 = util.load_bmp(name)) !=NULL) 
    { UseAsPaper(temp_32);
 	  sprintf(nom_paper,tmp);
    }
	    
}
  
void share::generateBrush(int16 size) //generate a brush of a particular size
{ 
	brush_is_perso=false;

	if (size < 1) size=1;
	current_brush = size;
	
	BRect rect; 
	rect.Set(0,0,size-1,size-1);
	
	BBitmap *tmp_brush = new BBitmap(rect,B_RGB32,true);

	tmp_brush->Lock(); //important!

	BView virtualView( tmp_brush->Bounds(), NULL, B_FOLLOW_NONE, 0 );
	tmp_brush->AddChild( &virtualView );


   //draw the brush
   float tmp_size = floor(size/2); //cut ray in half
   int16 val = 255;
   float center = tmp_size;
   float step = 255/size;
   hardness=100;
   step *= 1 * (4*float(hardness)/100);
  
  BPicture *my_pict; 
  virtualView.BeginPicture(new BPicture); 

   do {
  		 virtualView.SetHighColor(val,val,val);
   		 virtualView.FillEllipse(BPoint(center,center),tmp_size,tmp_size,B_SOLID_HIGH);
   		 val -= (int16) step;   if (val < 0) val=0;
   		 tmp_size--;
   	   } while (tmp_size!=-1);
 	
   my_pict = virtualView.EndPicture();
 
   virtualView.DrawPicture(my_pict);

   tmp_brush->RemoveChild( &virtualView );
   tmp_brush->Unlock();
	
  sprintf(nom_de_brush_perso,"pas de nom de brush");
  UseAsBrush(tmp_brush);
	
 // SURTOUT PAS delete tmp_brush;
 //UseAsBrush uses the tmp_brush bitmap
}


rgb_color share::PickColorAt(BPoint pix)
{

//similar for all lines
uint32 pos_bits = (uint32) ((pix.x+(act_img->pix_per_line*pix.y))*4);
uint8	*p_bits = undo_bitmap_bits +pos_bits; //jump to pixel in question
rgb_color col;
col.blue=*p_bits;  	p_bits++;
col.green=*p_bits;  	p_bits++;
col.red=*p_bits;  
//col.alpha=*bits;  
return col;
}

uint8 share::PickMaskColorAt(BPoint pix)
{

	uint32 pos_bits;
	pos_bits = uint32((pix.x+((act_img->pix_per_line)*pix.y))); 
	uint8 *p_bits = mask_work_bits ;
	p_bits += pos_bits;

	uint8 val = *p_bits;
	return val;

}


void share::UseAsBrush(BBitmap *temp_32)
{

    the_brush_24 = new BBitmap(temp_32); 
    
    uint8 *tmp_ptr,*dest_ptr;

	BRect rect = (temp_32->Bounds());
	the_brush = new BBitmap(rect,B_GRAY8);
	

    tmp_ptr = (uint8*) temp_32->Bits();
	dest_ptr= (uint8*) the_brush->Bits();

	uint16 moyenne=0;
	uint32 pos=0;
	uint32 taille=the_brush->BitsLength();
	
	// RGBA32 color space assumed
		while (pos!=taille)
		{
			moyenne  = 0;
			moyenne += *tmp_ptr; tmp_ptr++; //blue
			moyenne += *tmp_ptr; tmp_ptr++;	//green
			moyenne += *tmp_ptr; tmp_ptr++;	//red
			tmp_ptr++;						//alpha
			moyenne/=3; //average of the RGB triple
			*dest_ptr = (uint8) moyenne;
			dest_ptr++;
			pos++;
		}
	
    the_brush_bits = (uint8 *)the_brush->Bits();
	brush_x = uint16 (the_brush->Bounds().Width()  + 1);
	brush_y = uint16 (the_brush->Bounds().Height() + 1);
 	
 	brush_rayon_x = (brush_x/2)-1;
 	brush_rayon_y = (brush_y/2)-1;
 	 
 	if (util.toolWin!=NULL) util.toolWin->PostMessage(new BMessage(BRUSH_CHANGED)); 
	
}

void share::UseAsPaper(BBitmap *temp_32)
{

    paper_24 = temp_32; 
    
	uint8 *tmp_ptr,*dest_ptr;

	BRect rect = (temp_32->Bounds()); 
	paper = new BBitmap(rect,B_GRAY8);

    tmp_ptr = (uint8*) temp_32->Bits();
	dest_ptr= (uint8*) paper->Bits();

	uint16 moyenne=0;
	uint32 pos=0;
	uint32 taille=paper->BitsLength();
	while (pos!=taille)
		{
		moyenne  = 0;
		moyenne += *tmp_ptr; tmp_ptr++; //blue
		moyenne += *tmp_ptr; tmp_ptr++;	//green
		moyenne += *tmp_ptr; tmp_ptr++;	//red
		tmp_ptr++;						//alpha
		moyenne/=3; //average of the RGB triple
		
	
		*dest_ptr = (uint8) moyenne;
		dest_ptr++;
		pos++;
		}

    paper_bits = (uint8 *)paper->Bits();
	paper_x = uint16 (paper->Bounds().Width()  + 1);
	paper_y = uint16 (paper->Bounds().Height() + 1);
 	 
 if (util.toolWin!=NULL) util.toolWin->PostMessage(new BMessage(PAPER_CHANGED)); 
	
}



BRect share::FindSelectedRect()
//find the zone or the mask is active
{

	BRect limit;
	uint8 val;
	//invert values
	limit.top    = act_img->pix_per_row-1;
	limit.left   = act_img->pix_per_line-1;
	limit.bottom = 0;
	limit.right  = 0;


	BPoint le_point;
	le_point.Set(0,0);
	
	while(le_point.y != act_img->pix_per_row)
		{
						
					while(le_point.x!= act_img->pix_per_line)
					{	
					    val = PickMaskColorAt(le_point);
					    
						 if (val > 0) 
							{	
						    	if (le_point.x < limit.left)  limit.left = le_point.x;
								if (le_point.y < limit.top)   limit.top  = le_point.y;
							
								if (le_point.x > limit.right)	limit.right  = le_point.x;
								if (le_point.y > limit.bottom)	limit.bottom = le_point.y;
							}
					    le_point.x++;
					}

		le_point.x=0;
		le_point.y++;
		}

// Nothing is selected, therefore use *everything* as the selection
if (
	   limit.top    == act_img->pix_per_row-1
	&& limit.left   == act_img->pix_per_line-1
	&& limit.bottom == 0
	&& limit.right  == 0   ) 
	
	{
	limit.top    = 0;
	limit.left   = 0;
	limit.bottom = act_img->pix_per_row-1;
	limit.right  = act_img->pix_per_line-1;
	}

return limit;
}


void share::ImageManip(BBitmap *bit)
{
	mBitmapInUse = true;
	
	// Create bitmap accessor object
	BBitmapAccessor src_accessor(bit);
	src_accessor.SetDispose(false);

	Image_Manipulate(mAddonId, &src_accessor, 0);
	mBitmapInUse = false;
}


void share::ImageConvert(BBitmap *bit)
{
	mBitmapInUse = true;
	// Create bitmap accessor object
	BBitmapAccessor src_accessor(bit);
	src_accessor.SetDispose(false);

	// Create empty bitmap accessor object for the destination
	BBitmapAccessor dest_accessor;

	// Do image conversion
	Image_Convert(mAddonId, &src_accessor, &dest_accessor, NULL);

	// Got valid new bitmap?
	if (dest_accessor.IsValid())
	{
		if (dest_accessor.Bitmap()->ColorSpace()==B_RGB32)
		{
		char str[NAME_SIZE];
		sprintf(str,"(");
		strcat(str,Language.get("FILTERED"));
		strcat(str,")");
		NewImageFromBmp(str,dest_accessor.Bitmap());
		// Don't dispose it
		dest_accessor.SetDispose(false);
		}
		else
		{
		
		BAlert *alert = new BAlert("",Language.get("DEST_NOT_B_RGB32"), Language.get("YES"),NULL,NULL,
						   B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL);
		}
	} 

	mBitmapInUse = false;
}


void share::SetManipMenus()
{
util.mainWin->Lock();	

	manip_menu->RemoveItems(0,manip_menu->CountItems());
	convert_menu->RemoveItems(0,convert_menu->CountItems());

	// Create bitmap accessor object to pass along, so that we
	// only get the add-ons that support this bitmap in the menu
	BBitmapAccessor accessor(act_lay->img);
	accessor.SetDispose(false);

	
	const char *addonName;
	const char *addonInfo;
	const char *addonCategory;
	int32 addonVersion;
	image_addon_id *outList;
	int32 outCount = 0;
	if (Image_GetManipulators(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				BMessage *msg = new BMessage(IMAGE_MANIP);
				msg->AddInt32("addon_id", outList[i]);
				manip_menu->AddItem(new BMenuItem(addonName, msg));
			}
		}
		delete[] outList;
	}
	if (outCount == 0)
		manip_menu->SetEnabled(false);

	// Create sub menu with image converters
	outCount = 0;
	if (Image_GetConverters(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				BMessage *msg = new BMessage(IMAGE_CONV);
				msg->AddInt32("addon_id", outList[i]);
				convert_menu->AddItem(new BMenuItem(addonName, msg));
			}
		}
		delete[] outList;
	}
	if (outCount == 0)
		convert_menu->SetEnabled(false);


util.mainWin->Unlock();	

		
}

void share::AddToUpdateZone(BRect r)
{
   needs_update = true;
   
   if (first_zone==true) 
    {
	   what_to_update = r;
   	   first_zone = false;
   	}
   else
    {
   		what_to_update = what_to_update | r ; // union of 2 rectangles
   	}	

}

