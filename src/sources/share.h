#ifndef SHARE_TRUC_H
#define SHARE_TRUC_H

#include "all_includes.h"
#include "BPMImage.h"

extern BubbleHelper bb_help;

#define IMAGE_NUMBER_MAX 2048
#define BRUSH_TYPE_ELLIPSE	0
#define BRUSH_TYPE_RECT		1
	
class share
{
   public:
   
   share();

	//for Thumbnails
    int32 current_select_thumb;	
	DThumbnail *thumbnail;


	uint8 *the_bitmap_bits,*undo_bitmap_bits;
	uint8 *mask_bits,*mask_work_bits;
    uint8 *the_brush_bits;


   pattern stripes;
  
   BPMImage *les_images[IMAGE_NUMBER_MAX];
   BPMImage *act_img;
   Layer *act_lay;
   
   int16 active_image;
   int16 image_amount;

   bool brush_is_perso;
   uint16 brush_x,brush_y,brush_rayon_x,brush_rayon_y;
   uint8 brushtype;
   uint16 paper_x, paper_y;   
   
   bool needs_update;
   bool first_zone;
   
   BRect what_to_update;
   void AddToUpdateZone(BRect zone);

   uint8 active_tool,previous_tool;
   uint8 paint_mode;
   uint8 paint_transparency;
   uint8 hardness ;
   float pressure;

	rgb_color fore_color;
	rgb_color back_color;
	rgb_color picker_color;
	BBitmap *the_brush,*the_brush_24,*paper,*paper_24;
	 
	void CreateNewImage(const char *name,int16 w, int16 h);
	void LoadNewImage(const char *nam_file);
	void NewImageFromBmp(const char *nm,BBitmap *pic);
	void DeleteImage(int32 which_one);

	void ImageAdded();
	//void CreateNewImage(const char *name, BRect size, BBitmap* image, char *nam_file, uint8 is_revert);
	void initPic();
	void initLayer();

	
	void loadPaper(int16 nb);
	void EmptyPaper();
	void loadBrush(int16 nb);
	void generateBrush(int16 nb);
	float mb_height;//hauteur barre menu
	void UseAsBrush(BBitmap *temp_32);
	void UseAsPaper(BBitmap *temp_32);
	
	BRect FindSelectedRect();
	void   CopyBitmaptoWork();

	int32 current_perso_brush,current_paper;	
	int32 current_brush;

	int32 max_brush,max_paper;

	uint8 *cursor_ptr; //pointer to data for cursor
	int32 active_cursor_id;

	BScrollView *scroll_view_ptr;
	BScrollBar *scroll_hori,*scroll_verti;
	BView		*back_view_ptr;
	
	//points for color picker
	BPoint ori_back_slide;	BPoint ori_front_slide;
	BPoint ori_back_square;	BPoint ori_front_square;

	rgb_color PickColorAt(BPoint x);
	uint8 PickMaskColorAt(BPoint x);

	translator_id active_translator;

	uint8 stamp_offset_changed;
	BPoint stamp_offset; //offset of copy location to current location
    BPoint stamp_old_offset; //previous offset
    BPoint stamp_old_pos;

	BPoint pos_old,pos_actuelle,ori_pos;
	BMenu *win_menu,*manip_menu,*convert_menu,*display_menu;
	BMenuBar	*menubar;
	char nom_de_brush_perso[255],nom_paper[255];
	uint8 stroke;
	uint32 ori_buttons;

	//for imagemaniplib
	void SetManipMenus();
	bool mBitmapInUse;
	image_addon_id mAddonId;
	void ImageManip(BBitmap *bit);
	void ImageConvert(BBitmap *bit);
//	void PrintAddonInfo();

	BStringView* tool_name_ptr; 
	char tool_name_backup[255];

	rgb_color low_select_color;
	rgb_color high_select_color;
	
	BMenuItem *close_item;
	BMenuItem *revert_item;
	BMenuItem *save_item;
	BMenuItem *save_as_item;

    uint8 col_blue,col_red,col_green;
    
	//pointers for the paste_brush function
	uint8 *bits,*bits_st,*wbits,
			*brush_bits,*paper_bits,*mask_ptr,
			*ori_ptr,*wori_ptr,*ori_ptr_st;
	uint8 nb_bytes;

	
	
	};

#endif  