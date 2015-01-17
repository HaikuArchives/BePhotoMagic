#ifndef BPM_IMAGE_H

#define BPM_IMAGE_H

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Picture.h>
#include <Bitmap.h>
#include <String.h>
#include <List.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Layer.h"
#include "LayerWindowMsgs.h"
#include "ErrorCodes.h"
#include "SpLocaleApp.h"
#include "TransModes.h"
#include "PointStack.h"
#include "Undo.h"
#include "Utils.h"

// "Borrowed" from BrushView.h
#ifndef BRUSH_TYPE_ELLIPSE
	#define BRUSH_TYPE_ELLIPSE	0
	#define BRUSH_TYPE_RECT		1
#endif

#ifndef UPDATE_BRUSH
	#define UPDATE_BRUSH 'upbr'
#endif


#define MERGE_DOWN_AND 'mdan'
#define MERGE_DOWN_OR 'mdor'
#define MERGE_DOWN_NOT 'mdnt'
#define MERGE_DOWN_XOR 'mdxr'

#define SELECT_ALL 'slal'
#define SELECT_NONE 'slnn'

#ifndef UPDATE_COLORS
	#define UPDATE_COLORS 'upcl'
#endif

#define MAX_LAYERS	255

// Defined also in BrushView.h
#ifndef SET_BRUSH_CONTROLS
	#define SET_BRUSH_CONTROLS 'sbwc'
#endif

class BPMImage
{
public:
	BPMImage(const char *namestr, int16 imgwidth, int16 imgheight);
	BPMImage(void);
//	BPMImage(const char *namestr, BBitmap *bmp);
	virtual	~BPMImage(void);

	// Data Members
	char name[255];	
	uint32 width,height;
	Layer *layers[MAX_LAYERS];

	rgb_color forecolor;
	rgb_color backcolor;

	BBitmap *display_bitmap,	// Used in updating the display view
			*currentbrush,
			*work_bitmap,	// Used as an intermediary during paintbrush use
			*selection;
	BBitmap *bottomlayer,*currentlayer, *toplayer;	// for faster updates

	uint16 brush_width,brush_height;
	uint8 brushtype,brush_softness;
	uint8 brushblend, brushalpha, lineblend, linealpha, linestep;
	uint8 blendmode;	// image's "global" blendmode for functions
	uint8 stampblend,stampalpha;
	
	bool userbrush;
	bool ispainting;
	bool save_each_paint;	// tells Paintbrush to save where it painted
	
	void MakeBrush(int16 width, int16 height,int8 softness, int8 type);
	void NotifyLayerWindow(void);
	void UpdateDisplayImage(BRect update_rect);
	void PrintToStream(void);

	// Tool functions
	void PaintBrush(BPoint pt, bool fore=true,bool zoom_comp=true);
	void PaintLine(BPoint start,BPoint end,bool fore=true);
	void PaintRect(BRect frame,bool fore=true);
	void PaintFRect(BRect frame,bool fore=true);
	void PaintEllipse(BRect frame,bool fore=true);
	void PaintFEllipse(BRect frame,bool fore=true);
	void Stamp(BPoint from,BPoint to);

	void FloodFill(BPoint pt,rgb_color color);
	void FillRow(BPoint pt);
	void inline FillPixel(BPoint pt);
	rgb_color GetPixel(BPoint pt);
	void SetPixel(BPoint where, rgb_color color);
	bool CheckPixel(BPoint pt);
	rgb_color fillcolor,filledcolor;
	PointStack pointstack;
	uint8 tolerance;
	bool upbound,downbound;

	// Layer Functions
	bpm_status CreateNewLayer(const char *namestr, int16 laywidth, int16 layheight);
	bpm_status NewLayerFromBitmap(const char *namestr, BBitmap *bmp);
	bool DeleteLayer(void);
	void DuplicateLayer(void);
	void SwapLayers(uint16 first, uint16 second);
	void FlattenImage(void);
	void MakeComposite(void);
	void MergeDown(void);
	void MergeAbove(BRect update_rect);
	void MergeBelow(BRect update_rect);

	// Undo-related
	bpm_status SaveUndo(uint8 type,BRect saverect);
	bpm_status Undo(void);
	void ClearUndo(void);
	BRect GetBrushedRect(BPoint pt,bool zoom_comp=true);

	// Mask-related
	void SelectAll(void);
	void SelectNone(void);
	
	uint16 active_layer;
	Layer *p_active_layer;
	uint16 number_layers;
	float zoom;
	uint8 untitled;
	int32 typecode;
	BString path;
	BString MIME;	// MIME signature
	BList *undolist;
	bool maskmode,erasemode;
};	

#endif