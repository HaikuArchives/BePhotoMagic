#ifndef _BPM_PREFS_
#define _BPM_PREFS_

#include <Rect.h>
#include <Alert.h>
#include <Screen.h>
#include <Directory.h>
#include <Entry.h>
#include "yprefs.h"

class BPMPrefs
{
public:
	BPMPrefs(void);
	bool Save(const char *path);
	bool Load(const char *path);
	bool toolwin_open, layerwin_open, brushwin_open;
	bool optionswin_open, backwin_open, forewin_open;
	bool mousewin_open;
	BRect mainwin_frame, toolwin_frame, layerwin_frame, brushwin_frame;
	BRect optionswin_frame;
	BPoint backpoint,forepoint,mousepoint;

	BPMPrefs& operator=(const BPMPrefs &from);
};

#endif