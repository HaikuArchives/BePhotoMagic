#ifndef NAT_WINDOW_H
#define NAT_WINDOW_H

#include <Cursor.h>
#include <ScrollBar.h>

#include "message_labels.h"
#include "PrefWindow.h"
#include "CreateWindow.h"
#include "ProgressWin.h"
#include "PaperWindow.h"
#include "BrushWindow.h"
#include "LayerWindow.h"
#include "InfoWindow.h"
#include "RangeWindow.h"
//#include "CurveWindow.h"
#include "LimitLevelsWindow.h"
#include "OptionWindow.h"
#include "NavigationWindow.h"
#include "ToolWindow.h"
#include "TextToolWindow.h"

#include "SaveAsPanel.h"
#include "LoadSavePanels.h"

#include "BPMView.h"
#include "share.h"
#include "cursors.h"

#define FULL_SCREEN_NONE 0
#define FULL_SCREEN_HALF 1
#define FULL_SCREEN_TOTAL 2


class BPMWindow : public BWindow 
{
public:
	
share *shared;

BPMWindow(BRect frame,share *sh); 
~BPMWindow();
virtual	bool	QuitRequested();

void UpdateToolInfo();

BView		*back_view;
PicView		*inside_view; 
BScrollView	*scroll_view;
//OutputFormatWindow *format_win;

BrushWindow		*brWindow;
PaperWindow 	*ppWindow;
LayerWindow 	*lyWindow;
InfoWindow		*ifWindow;
OptionWindow	*opWindow;
NavigationWindow *nvWindow;
TextToolWindow	*ttWindow;
ToolWindow 		 *tlWindow;
RangeWindow 		 *rgWindow;
//LimitLevelsWindow    *llWindow;
//CurveWindow *curv_win;
CreateWindow *crt_win;
PrefWindow *prefWindow;

bool rgWindow_visible;

BBitmap *back_bmp;	

uint8 full_screen;

void FrameResized(float width, float height);
void DisableAll();
void EnableAll();


void MessageReceived(BMessage *msg);
void ShortCuts(BMessage *msg);

void OpenPanel();
void SavePanel();
 
BPMOpenPanel *panneau_load;
SaveAsPanel     *panneau_save;

BMenu		*gimpFilterMenu;

void UpdateTitle();
void ZoomChanged();		
void SetPointerForActiveTool();

bool menu_bar_shown;
BMenuItem *mask_item;
void SetFullScreen();
BRect windowed_frame;

bool all_win_hidden;


//----------------------- FOR SUPPORT GIMP FILTERS -------------------------------------
		//Pointeurs sur lesquels doivent pointer les sources et dest pour filter gimp ok
		BBitmap *fSrcBitmap; 
		BBitmap	*fDstBitmap;
		
		void	AddFilter (const char *menu, const char *item, BMessage *msg);
		void	UseBitmap (BBitmap *bm);
		void	SwitchBuffers (bool showMain);
		void	Update (void);
		BBitmap	*Src (void) { return fSrcBitmap; }
		BBitmap	*Dst (void) { return fDstBitmap; }
		
void RegisterGimpFilters (void) ;
int32 AsyncFilter (void *arg);

protected:
	void	BuildMenus (void);
	BMenu		*fFilterMenu;
};

#endif //BPM_WINDOW_H
