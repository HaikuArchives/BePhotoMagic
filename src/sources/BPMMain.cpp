#include "BPMMain.h"

BWindow *bpmwin;

#ifndef MOUSECAM_CLOSED
	#define MOUSECAM_CLOSED 'tmcc'
#endif

int main()
{
 	BPMApplication	myApplication;
	myApplication.Run();
	return(0);
}

BPMApplication::BPMApplication()
		  		  : BApplication("application/x-vnd.bpm-BePhotoMagic")
{
	aboutwin=new AboutWindow(true);
	aboutwin->Show();

	InitTrigTables();

	mainwin = new BPMWindow(BRect(100,100,300,300));
	bpmwin = (BWindow *)mainwin;
	BScreen screen;
	mainwin->SetSizeLimits(320,screen.Frame().Width()-1,270,screen.Frame().Height()-1);
	mainwin->Show();
	mainwin->Activate();

	filterwin_open=false;
	textwin_open=false;
	waitwin_open=false;

	aboutwin->Lock();
	aboutwin->Close();
}

BPMApplication::~BPMApplication(void)
{
}

void BPMApplication::ArgvReceived(int32 argc, char **argv)
{
	int32 i = 1;
	argv++; 			//skip first command-line arg (name of executable)
	char namestr[255];

	do
	{
		sprintf(namestr,*argv);
		// Call the LoadImage() function here
		argv++;
	    i++;
	}while (i < argc);
}

void BPMApplication::RefsReceived(BMessage *le_message)
{
	uint32 type;
	int32 count;
	entry_ref ref;
	BPath the_path;
	char txt[255];
	BBitmap *tempbmp;

	le_message->GetInfo("refs", &type, &count);
	if(count>30) count=30;

	ShowWait();
	for ( long i = --count; i >= 0; i-- )
	{
		le_message->FindRef("refs", i, &ref);
		BEntry entry(&ref);

		entry.GetPath(&the_path);
		sprintf(txt,the_path.Path());

		if(IsBPMFile(txt))
			mainwin->OpenBPM(txt);
		else
		{
			tempbmp=LoadBitmap(txt);

			if(tempbmp==NULL)
			{	sprintf(txt,SpTranslate("Unable to open "));
				strcat(txt,the_path.Path());
				BAlert *alert = new BAlert("BePhotoMagic",txt,SpTranslate("OK") );
				alert->Go();
			}
			else
			{
				mainwin->OpenImage(txt,tempbmp);
				delete tempbmp;
			}
		}
	}
	CloseWait();
}

void BPMApplication::MessageReceived(BMessage *msg)
{
BPath temppath;

	switch (msg->what)
	{
		case FILTERED_BITMAP:
			filterwin_open=false;
			mainwin->PostMessage(msg);
			break;

		case SHOW_WAIT:
			ShowWait();
			break;

		case CLOSE_WAIT:
			CloseWait();
			break;

		case TRANSLATOR_SELECTED:
		{	int32 code;
			BString tempmime;
			if(msg->FindInt32("type_code",0,&code)!= B_OK)
				break;
			if(msg->FindString("MIME",&tempmime)!=B_OK)
				tempmime.SetTo("");

			if(mainwin->imgview->openimages>0)
			{	mainwin->Lock();
				mainwin->imgview->p_active_image->typecode=code;
				mainwin->imgview->p_active_image->MIME=tempmime;
				mainwin->Unlock();
			}
			else
			{	default_type=code;
				default_mime=tempmime;
			}

			break;
		}
		case B_KEY_DOWN:
			Shortcuts(msg);
			break;

		case BPM_SAVE_REF:
			if(mainwin->save_BPM)
				mainwin->SaveActive(msg);
			else
				TranslatorExport(msg);
			break;

		// We are playing hot potato with these messages
		// Shouldn't really matter. Few ppl need to change tools
		// 10x a second. ;^)
		case TOOL_BRUSH:
		case TOOL_FILL:
		case TOOL_GRADIENT:
		case TOOL_LINE:
		case TOOL_SPLINE:
		case TOOL_RECT:
		case TOOL_ELLIPSE:
		case TOOL_ZOOM:
		case TOOL_EYEDROPPER:
		case TOOL_STAMP:
		case TOOL_WAND:
		case TOOL_FRECT:
		case TOOL_FELLIPSE:
		case TOOL_MASK:
		case TOOL_MOVE:
			if(optionswin_open)
				optionswin->PostMessage(msg);
		case SCREEN_FULL_NONE:
		case SCREEN_FULL_HALF:
		case SCREEN_FULL_ALL:
		case CREATE_IMAGE:
		case TOOL_ERASER:

		case TOOL_TEXT:
		case TOGGLE_TOOL_WINDOW:
		case TOGGLE_BRUSH_WINDOW:
		case TOGGLE_OPTIONS_WINDOW:
   	 	case TOGGLE_BACKCOLOR:
   	 	case TOGGLE_FORECOLOR:
		case GET_APP_DIR:
		case CHILDWIN_MOVED:
		case TOOLWIN_CLOSED:
		case FOREWIN_CLOSED:
		case BACKWIN_CLOSED:
		case BRUSHWIN_CLOSED:
		case LAYERWIN_CLOSED:
		case OPTIONSWIN_CLOSED:
		case TEXTWIN_CLOSED:
		case MOUSECAM_CLOSED:
		case UPDATE_COLORS:
		case FORECOLOR_CHANGED:
		case BACKCOLOR_CHANGED:
		case SELECT_USER_BRUSH:
		case UPDATE_BRUSH:
		case SET_BRUSH:
		case SHOW_FILTER_WINDOW:
		case CANCEL_CREATE_IMAGE:
		case POST_BITMAP:
		case LAYER_NEW:
		case LAYER_DUPLICATE:
		case LAYER_DELETE:
		case LAYER_COMPOSITE:
		case LAYER_FLATTEN:
		case LAYER_MOVE_UP:
		case LAYER_MOVE_DOWN:
		case LAYER_MOVE_TOP:
		case LAYER_MOVE_BOTTOM:
			mainwin->createwin_open=false;
			mainwin->PostMessage(msg);
			break;
		default:
			BApplication::MessageReceived(msg);
	}

}
