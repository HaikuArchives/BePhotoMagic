#include "AppCursor.h"

int8 AppCursor::GetCursor(void)
{	return currentcursor;
}

bool AppCursor::SetCursor(int8 index)
{	if(index > cursormax)
		return false;
	// true parameter needed to make app server change immediately
	be_app->SetCursor(cursordata[index],true);
	return true;
}

AppCursor::AppCursor(void)
{
static uint8 cursor_pick[] = {16,1,2,1,
0,0,64,0,160,0,88,0,36,0,34,0,17,0,8,128,
4,88,2,60,1,120,0,240,1,248,1,220,0,140,0,0,
0,0,64,0,224,0,120,0,60,0,62,0,31,0,15,128,
7,216,3,252,1,248,0,240,1,248,1,220,0,140,0,0
};

static uint8 cursor_cross_min[] = {16,1,7,7,
0,0,0,0,0,0,0,0,0,0,1,0,1,0,6,192,
1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,0,1,0,6,192,
1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static uint8 cursor_empty_pointer[] = {16,1,7,7,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static uint8 cursor_zoom_out[] = {16,1,5,6,
15,0,48,192,64,32,64,32,128,16,159,144,159,144,128,16,
64,32,64,48,48,248,15,60,0,30,0,15,0,7,0,2,
15,0,63,192,127,224,127,224,255,240,255,240,255,240,255,240,
127,224,127,240,63,248,15,60,0,30,0,15,0,7,0,2
};

static uint8 cursor_zoom_in[] = {16,1,5,6,
15,0,48,192,64,32,70,32,134,16,159,144,159,144,134,16,
70,32,64,48,48,248,15,60,0,30,0,15,0,7,0,2,
15,0,63,192,127,224,127,224,255,240,255,240,255,240,255,240,
127,224,127,240,63,248,15,60,0,30,0,15,0,7,0,2
};

static uint8 cursor_cross[] = {16,1,5,5,
14,0,4,0,4,0,4,0,128,32,241,224,128,32,4,0,
4,0,4,0,14,0,0,0,0,0,0,0,0,0,0,0,
14,0,4,0,4,0,4,0,128,32,245,224,128,32,4,0,
4,0,4,0,14,0,0,0,0,0,0,0,0,0,0,0
};

cursordata[0]=new BCursor(cursor_cross);
cursordata[1]=new BCursor(cursor_cross_min);
cursordata[2]=new BCursor(cursor_pick);
cursordata[3]=new BCursor(cursor_zoom_in);
cursordata[4]=new BCursor(cursor_zoom_out);
cursordata[5]=new BCursor(cursor_empty_pointer);

cursormax=CURSORMAX;
};

AppCursor::~AppCursor(void)
{	for(int8 i=0; i<= cursormax; i++)
		delete cursordata[i];
}

ViewCursor::ViewCursor(BView *vcview) : AppCursor()
{	view=vcview;
}

BView * ViewCursor::View(void)
{	return view;
}

bool ViewCursor::SetViewCursor(int8 index)
{	if(index > cursormax)
		return false;
	
	view->SetViewCursor(cursordata[index],true);
	return true;
}