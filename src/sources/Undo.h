#ifndef UNDO_MASK

// Undo Types
#define UNDO_MASK 1		// flag signifying undo done on the mask

#define UNDO_PAINTBRUSH 2	// only for the paintbrush tool
#define UNDO_BRUSH 4	// Change of brush
#define UNDO_TRANSFORM 8	// Rotate, resize, etc
#define UNDO_LAYER 16		// Add layer, Delete layer, Make Composite
							// Merge Down
#define UNDO_MULTILAYER 32	// Flatten, Merge Visible
#define UNDO_TOOL 64		// Drawing on the image

#endif

#ifndef _UNDO_H_

#define _UNDO_H_

#include <Bitmap.h>
#include <List.h>

#include "Layer.h"
#include "Utils.h"
#include "ErrorCodes.h"

#define MAX_UNDO 255

class UndoData
{
public:
	UndoData(void);
	~UndoData(void);
	
	BBitmap *bitmap;
	uint8 type;
	BRect rect;
	uint8 layerindex;

	// Brush data
	uint16 brush_width,brush_height;
	uint8 brushtype,brush_softness;
	bool userbrush;

	BList layers;		// so we can save deleted layers
};
/*
class UndoTable
{
public:
	UndoTable(void);
	~UndoTable(void);

	UndoData *undodata[MAX_UNDO];
	uint16 	number_undo,			// Number of undos in current table
			undo_pointer;			// Current position in the stack
};	
*/
#endif