#include "BPMMain.h"

int main()
{	

	BPMApplication	myApplication;
	
	myApplication.Run();

	return(0);
}

BPMApplication::BPMApplication()
		  		  : BApplication("application/x-vnd.beunited-bpmagic")
{

	ThePrefs.ResetPrefs(); 
	//the reset is necessary in case we can't load the preferences
	ThePrefs.Load(); 
	
	Language.InitLang();

	//PostMessage(MENU_HELP_ABOUT);
	BRect r= util.find_win_pos_on_main_screen(560,298+12);
	ab_win = new AboutWindow(r,shared);
	ab_win->Show();
	
	shared = new share();

	BRect rect;
	// set up a rectangle and instantiate a new window
	
	rect = util.find_win_pos_on_main_screen(256,48);
	progWindow = new ProgressWindow(rect,"Loading...");
	util.progress_win = progWindow;
	progWindow->Show();
	
	rect.Set(8, 24, 8+START_X_SIZE-1, 24+START_Y_SIZE-1);
	mainWindow = new BPMWindow(rect,shared);
 	mainWindow->PostMessage(new BMessage(TOOL_CHANGED)); 
	mainWindow->PostMessage(new BMessage(TRANSP_CHANGED)); 	
	mainWindow->Zoom();
	mainWindow->Show();
	
	mainWindow->PostMessage(HIDE_PROGRESS_WIN);

	util.mainWin=mainWindow;
	
	if (Image_Init(NULL) != B_OK)
	{
		BAlert *alert = new BAlert(NULL,Language.get("NO_IMAGE_MANIP_LIB"), 
                     Language.get("YES"),NULL,NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL);
	}

//	mainWindow->ZoomChanged(); 

	mainWindow->PostMessage(UPDATE_TITLE);
	
	BMessage *mx = new BMessage(SET_CURSOR);
	mx->AddInt32("id",6); //6 = cursor_simple_cross
	mainWindow->PostMessage(mx);
	mainWindow->Activate();
	
	ab_win->Lock();
	ab_win->Close();
}

void BPMApplication::ArgvReceived(int32 argc, char **argv)
{
int32 i = 1;
argv++; 			//skip first command-line arg (name of executable)
char nm[NAME_SIZE];

// ATTENTION: as long as there isn't a created picture one cannot init the others 
// it is a bug, but for the moment, create one.
//	  shared->CreateNewImage(BRect(0,0,64,64),NULL,NULL,OFF);	
	  	 
do
{
	sprintf(nm,*argv);
//	printf("\n\n**** ");  printf(*argv);
	shared->LoadNewImage(nm);	
	argv++;
    i++;
}while (i < argc);

mainWindow->Lock();
mainWindow->ZoomChanged();
mainWindow->Unlock();

// Shutdown the image manipulation library
Image_Shutdown();
}

void BPMApplication::RefsReceived(BMessage *le_message)
{		// Load all selected files
uint32 type; 
int32 count; 
entry_ref ref; 
       
le_message->GetInfo("refs", &type, &count); 
	   				
for ( long i = --count; i >= 0; i-- )
{	
	le_message->FindRef("refs", i, &ref); 
	
	BEntry entry(&ref);
	BPath the_path; 
	char txt[NAME_SIZE];
	entry.GetPath(&the_path);
	sprintf(txt,the_path.Path());
			
	//see if file is already loaded somewhere (loose translation)
	int16 ii=-2; //if you start with 0 and there are no images, loop never exits
	int16 exists= -100;
	do 
	{	//if image exists, tell the user
		if (shared->les_images[i] != NULL)
		{	if (strcmp(shared->les_images[i]->full_path,txt)==0) 
		 		exists =i; 
		}
	} while (ii++ != shared->image_amount);
			
	if (exists != -100)
	{
		BAlert *alert = new BAlert(NULL,Language.get("ALREADY_LOADED"), 
    			Language.get("YES"),  Language.get("NO"),NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		int32 button_index;

   		switch(button_index = alert->Go())
   		{
   			//buttons have value 0,1, or 2 - 3 buttons max for a BAlert
   			case 1:
   			shared->win_menu->ItemAt(exists)->SetMarked(true);
   			shared->active_image = exists;
			shared->initPic();
   			break;
					
			case 0:   //make a new copy
			util.show_progress_load = ON;
			//	util.mainWin->PostMessage(SHOW_PROGRESS_WIN);
			shared->LoadNewImage(txt);	 //revert mode =OFF
			break;
		}
	}
	else 	// load file
	{
		util.show_progress_load = ON;
		//util.mainWin->PostMessage(SHOW_PROGRESS_WIN);
		shared->LoadNewImage(txt);	 //revert mode =OFF
	}

	mainWindow->Lock();
	mainWindow->ZoomChanged();
	mainWindow->Unlock();
					
}					

}

void BPMApplication::MessageReceived(BMessage *msg)
{
BRect r;
	switch (msg->what)
	{
		case B_REFS_RECEIVED:
		beep();
		break;
		
		case B_SAVE_REQUESTED:
		SaveImg(msg); // Save file. (Get name from BMessage)
		break;
		
		case SAVE_WITH_NAME:
		SaveImg(NULL); //Save As...
		break;

		case B_KEY_DOWN:
		mainWindow->PostMessage(msg);
		break;
		
		case MENU_HELP_ABOUT:
		r= util.find_win_pos_on_main_screen(560,298+12);
		ab_win = new AboutWindow(r,shared);
		ab_win->Show();
		break;
			
	}
	
}


void BPMApplication::SaveImg(BMessage *msg)
{

char txt[NAME_SIZE];

if (msg != NULL) //if msg==NULL means prompt for a name
{
				
	const char *Filename; // filename -  CONST keyword is important
	entry_ref Directory; // save directory

	msg->FindRef( "directory", &Directory );
	msg->FindString( "name",  &Filename );
		
	BEntry entry(&Directory);
	BPath the_path; 
	entry.GetPath(&the_path);
		
	sprintf(txt,the_path.Path());
	strcat(txt,"/");
	strcat(txt,Filename);
		
	//if one selected a filename (needed for new files)
	sprintf(shared->act_img->full_path,txt);
	mainWindow->PostMessage(new BMessage(UPDATE_TITLE)); 
}
		
else //sinon c'est qu'on update le fichier existant
{
	sprintf(shared->act_img->full_path,txt);
}	

	
//get ref starting from path
BEntry full_path;
full_path.SetTo(txt);
entry_ref ref;
full_path.GetRef(&ref);
		
BPath pth(txt);

sprintf(shared->act_img->full_path,txt);
sprintf(shared->act_img->name,pth.Leaf());


//ON SAVE	
//	util.StoreTranslatorBitmap(shared->act_lay->img, txt,shared->active_translator) ;


//ThePrefs.no_pictures_left=ON;

//can't access bitmap directly - lock window or crash
shared->act_img->display_bitmap->Lock();

if (util.StoreTranslatorBitmap(shared->act_img->display_bitmap, 
				txt, shared->active_translator) == B_OK) 
{

/*	if (ThePrefs.save_with_thumb==B_CONTROL_ON)
	{
		if (shared->thumbnail->Bitmap())    // will be NULL if the thumbnail couldn't be created 
		{ 
	 	  	BNode node(&ref); 
   			shared->thumbnail->WriteThumbnailAttribute(&node); 
   			shared->thumbnail->WriteIconAttribute(&node); 
   			shared->thumbnail->WriteMiniIconAttribute(&node); 
   			shared->thumbnail->WriteResolutionAttributes(&node); 
		} 
	}*/
}
	
shared->act_img->display_bitmap->Unlock();

mainWindow->PostMessage(UPDATE_TITLE);
mainWindow->PostMessage(UPDATE_WIN_MENU);
mainWindow->PostMessage(ZOOM_CHANGED);
}

