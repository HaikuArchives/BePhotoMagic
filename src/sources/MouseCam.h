#ifndef _MOUSECAM_H_
#define _MOUSECAM_H_

#include <Box.h>
#include <FilePanel.h>
#include <MenuBar.h>
#include <View.h>
#include <Window.h>
#include "SpLocaleApp.h"

#define TOGGLE_MOUSECAM_WINDOW 'tmcw'
#define MOUSECAM_CLOSED 'tmcc'

#define MOUSECAM_HELP 'help'
#define MOUSECAM_UPDATE_INDO 'info'
#define MOUSECAM_SHOW_INFO 'show'
#define MOUSECAM_TOGGLE_GRID 'grid'
#define MOUSECAM_SHRINK 'shnk'
#define MOUSECAM_GROW 'grow'
#define MOUSECAM_MAKE_SQUARE 'sqar'
#define MOUSECAM_SHRINK_PIXEL 'pshk'
#define MOUSECAM_GROW_PIXEL 'pgrw'

#define MOUSECAM_NEW_COLOR 'colr'
#define MOUSECAM_TOGGLE_RULER 'rulr'
#define MOUSECAM_COPY_IMAGE 'copy'
#define MOUSECAM_TRACK_COLOR 'trak'
#define MOUSECAM_FREEZE 'frez'
#define MOUSECAM_DUMP 'dump'
#define MOUSECAM_ADD_CROSSHAIR 'acrs'
#define MOUSECAM_REMOVE_CROSSHAIR 'rcrs'
#define MOUSECAM_SAVE 'save'

const rgb_color kViewGray = { 216, 216, 216, 255};
const rgb_color kGridGray = {130, 130, 130, 255 };
const rgb_color kWhite = { 255, 255, 255, 255};
const rgb_color kBlack = { 0, 0, 0, 255};
const rgb_color kDarkGray = { 96, 96, 96, 255};
const rgb_color kRedColor = { 255, 10, 50, 255 };
const rgb_color kGreenColor = { 10, 255, 50, 255 };
const rgb_color kBlueColor = { 10, 50, 255, 255 };

const int32 msg_save = 'save';

//******************************************************************************

class TMagnify;
class TOSMagnify : public BView {
public:
						TOSMagnify(BRect, TMagnify* parent, color_space space);
						~TOSMagnify();
						
		void			InitObject();
		void			FrameResized(float width, float height);
		void			SetSpace(color_space space);
		
		void			Resize(int32 width, int32 height);
						
		bool			CreateImage(BPoint, bool force=false);
		bool			CopyScreenRect(BRect);

		void			DrawGrid(int32 width, int32 height,
							BRect dest, int32 pixelSize);
		void			DrawSelection();
		
		rgb_color		ColorAtSelection();
		
		BBitmap*		Bitmap() { return fBitmap; }
		
private:
		color_map*		fColorMap;
		color_space		fColorSpace;
		char*			fOldBits;
		long			fBytesPerPixel;
		
		TMagnify*		fParent;
		BBitmap*		fBitmap;
		BBitmap*		fPixel;
		BView*			fPixelView;
};

//******************************************************************************

class TWindow;
class TMagnify : public BView {
public:
						TMagnify(BRect, TWindow*);
						~TMagnify();
		void			AttachedToWindow();
		void			InitBuffers(int32 hPixelCount, int32 vPixelCount,
							int32 pixelSize, bool showGrid);
		
		void			Draw(BRect);
		
		void			KeyDown(const char *bytes, int32 numBytes);
		void			FrameResized(float, float);
		void			MouseDown(BPoint where);
		void			ScreenChanged(BRect bounds, color_space cs);

		void			SetSelection(bool state);
		void			MoveSelection(int32 x, int32 y);
		void			MoveSelectionTo(int32 x, int32 y);
		void			ShowSelection();
		
		short 			Selection();
		bool			SelectionIsShowing();
		void			SelectionLoc(float* x, float* y);
		void			SetSelectionLoc(float, float);
		rgb_color		SelectionColor();
		
		void			CrossHair1Loc(float* x, float* y);
		void			CrossHair2Loc(float* x, float* y);
		BPoint			CrossHair1Loc();		
		BPoint			CrossHair2Loc();
		
		void			NudgeMouse(float x, float y);
		
		void			WindowActivated(bool);

static	long			MagnifyTask(void *);

		void			Update(bool force);
		bool			NeedToUpdate();
		void			SetUpdate(bool);
		
		void			CopyImage();
		
		long			ThreadID() { return fThread; }

		void			MakeActive(bool);
		bool			Active()	{ return fActive; }
		
		void			AddCrossHair();
		void			RemoveCrossHair();
		void			SetCrossHairsShowing(bool ch1=false, bool ch2=false);
		void			CrossHairsShowing(bool*, bool*);
				
		void			PixelCount(int32* width, int32* height);
		int32 			PixelSize();
		bool			ShowGrid();
				
		void			StartSave();
		void			SaveImage(entry_ref* ref, char* name, bool selectionOnly=false);
		void 			SaveBits(BFile* file, const BBitmap *bitmap, char* name) const;
		void			EndSave();
		
private:
		bool			fNeedToUpdate;
		long			fThread;				//	magnify thread id
		bool			fActive;				//	magnifying toggle
		

		BBitmap*		fImageBuf;				// os buffer
		TOSMagnify*		fImageView;				// os view
		BPoint			fLastLoc;

		short			fSelection;

		bool			fShowSelection;
		BPoint			fSelectionLoc;

		bool			fShowCrossHair1;
		BPoint			fCrossHair1;
		bool			fShowCrossHair2;
		BPoint			fCrossHair2;
		
		TWindow*		fParent;
		
		bool			fImageFrozenOnSave;
};

//******************************************************************************

class TWindow;
class TMenu : public BMenu {
public:
					TMenu(TWindow* mainWindow, const char *title=NULL,
						menu_layout layout = B_ITEMS_IN_COLUMN);
					~TMenu();
					
		void		AttachedToWindow();

private:
		TWindow*	fMainWindow;
};

class TInfoView : public BBox {
public:
					TInfoView(BRect frame);
					~TInfoView();
				
		void		AttachedToWindow();
		void		Draw(BRect updateRect);
		void		FrameResized(float width, float height);
		
		void		AddMenu();		
		void		SetMagView(TMagnify* magView);

private:
		float 		fFontHeight;		
		TMagnify*	fMagView;
		BMenuField* fPopUp;
		TMenu*		fMenu;
		
		int32 		fHPixelCount;
		int32 		fVPixelCount;
		int32		fPixelSize;
		
		rgb_color	fSelectionColor;
		
		BPoint		fCH1Loc;
		BPoint		fCH2Loc;

		char		fInfoStr[64];
		char		fRGBStr[64];
		char		fCH1Str[64];
		char		fCH2Str[64];
};

//******************************************************************************

class TWindow : public BWindow {
public:
					TWindow(int32 pixelCount=-1);
					~TWindow();
		
		void		MessageReceived(BMessage*);
		bool		QuitRequested();
		
		void		GetPrefs(int32 pixelCount=-1);
		void		SetPrefs();
		
		void		FrameResized(float w, float h);
		void 		FrameMoved(BPoint origin);
		void		ScreenChanged(BRect screen_size, color_space depth);
		
		void		Minimize(bool);
		void		Zoom(BPoint rec_position, float rec_width, float rec_height);
		
		void		CalcViewablePixels();
		void		GetPreferredSize(float* width, float* height);
		
		void		ResizeWindow(int32 rowCount, int32 columnCount);
		void		ResizeWindow(bool direction);

		void		SetGrid(bool);
		bool		ShowGrid();
		
		void		ShowInfo(bool);
		bool		InfoIsShowing();
		void		UpdateInfo();
		
		void		AddCrossHair();
		void		RemoveCrossHair();
		void		CrossHairsShowing(bool* ch1, bool* ch2);
		
		void		PixelCount(int32* h, int32 *v);
		
		void		SetPixelSize(int32);
		void		SetPixelSize(bool);
		int32		PixelSize();
		
		void		ShowHelp();
		
		bool		IsActive();
		
private:
		float		fInfoHeight;
		bool		fShowInfo;
		float 		fFontHeight;

		bool		fShowGrid;
		
		int32		fHPixelCount;
		int32		fVPixelCount;
		int32 		fPixelSize;
		
		TMagnify*	fFatBits;
		TInfoView*	fInfo;
		
		BFilePanel*	fSavePanel;
};

#endif