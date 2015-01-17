#ifndef _OPEN_PANEL_H
#define _OPEN_PANEL_H

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <FilePanel.h>
#include <Entry.h>
#include <Directory.h>
#include <File.h>
#include <Path.h>

#include "DrawButton.h"
#include "BPMView.h"
#include "SpLocaleApp.h"
#include "Utils.h"

class BPMOpenPanel : public BFilePanel
{
public:
	BPMOpenPanel(void);
	~BPMOpenPanel(void);
	void SelectionChanged(void);
	BBitmap *pbitmap;
	DrawButton *preview;
};
#endif
