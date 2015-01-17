#include "Layer.h"

Layer::Layer(void)
{	
	sprintf(name," ");

	visible=false;
	active=false;
	opacity=0;
	blendmode=0;

	width=0;
	height=0;

	id=0;
	cliprect.Set(0,0,0,0);
}

Layer::Layer(const char *namestr, int16 laywidth, int16 layheight,int32 idnum)
{	
	if (namestr != NULL)
		sprintf(name, namestr);
	else 
		sprintf(name," ");

	visible=true;
	active=false;
	opacity=255;
	blendmode=0;

	width=laywidth;
	height=layheight;

	cliprect.Set(0,0,width-1,height-1);
	bitmap=new BBitmap( cliprect,B_RGBA32,true);
	id=idnum;
}

Layer::~Layer(void)
{	delete bitmap;
}
