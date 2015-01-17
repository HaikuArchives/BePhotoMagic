#ifndef _BPM_CURSORS_H_

#define BPM_CURSORS_H_

#include <Application.h>
#include <Cursor.h>
#include <View.h>

// Set this to the index of the last cursor
#define CURSORMAX	5

#define CURSOR_CROSS 0
#define CURSOR_CROSS_MIN 1
#define CURSOR_PICK	2
#define CURSOR_ZOOM_IN 3
#define CURSOR_ZOOM_OUT 4
#define CURSOR_EMPTY 5

class AppCursor
{
public:
	AppCursor(void);
	~AppCursor(void);
	
	bool SetCursor(int8 index);
	int8 GetCursor(void);
protected:
	int8 currentcursor;
	BCursor *cursordata[25];
	int cursormax;
};

class ViewCursor : public AppCursor
{
public:
	ViewCursor(BView *vcview);

	bool SetViewCursor(int8 index);
	BView * View(void);
protected:
	BView *view;
};

#endif