#ifndef _LAYER_H_

#define _LAYER_H_

#include <Bitmap.h>
#include <Rect.h>
#include <stdio.h>

class Layer
{
public:
	Layer(const char *namestr, int16 laywidth, int16 layheight,int32 idnum);
	Layer(void);
	~Layer(void);
	
	BBitmap *bitmap;
	
	char name[255];
	uint32 width,height;
	uint8 opacity;
	uint8 blendmode;
	int32 id;
	BRect cliprect;	// Layers are not all the same size, a la GIMP
	bool visible;
	bool active;
};

#endif