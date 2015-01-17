#include "SavePanel.h"

BPMSavePanel::BPMSavePanel(BMessage *msg):
	 BFilePanel(B_SAVE_PANEL,NULL,NULL,B_FILE_NODE,false,msg,NULL,false,true)
{
float height=74;
Window()->SetTitle(SpTranslate("Save As"));

Window()->Lock();
BView *background = Window()->ChildAt(0),*dirmenufield;

BRect limit = background->Bounds();
limit.bottom=height;
		
BMenu			*format_menu;
format_menu = BuildFormatsMenu();

format_menu->ItemAt(0)->SetMarked(true);
format_menu->SetLabelFromMarked(true);

BRect rect;
dirmenufield=Window()->FindView("DirMenuField");

if(dirmenufield!=NULL)
{
	dirmenufield->ResizeBy(-1*dirmenufield->Bounds().Width()/2,0);
	rect=dirmenufield->ConvertToParent(dirmenufield->Bounds());
	rect.OffsetBy(rect.Width()+5,0);
	rect.right=background->Bounds().right-10;
}
else
{
	//use view coordinates to set base for drop-down menu
	BRect taille=background->Bounds();
	float x_center=taille.right-100;
	
	rect.Set(x_center,   taille.top+20, x_center+126,    taille.top+20+12  );
	rect.OffsetBy(-20,2);
}

format = new BMenuField(rect,"",NULL,format_menu,B_FOLLOW_TOP | B_FOLLOW_RIGHT ,B_WILL_DRAW);
background->AddChild(format);
Window()->Unlock();

//SetRefFilter(new ImgRefFilter(shared));

}

BMenu *BPMSavePanel::BuildFormatsMenu(void)
{
	// Builds a menu with all the possible file types
	// KNOWN BUG: If more than one translator can produce a given file type there
	// will be a duplicate entry

	BMessage *message;
	bool first=true;
	int32 typecode;
	BMenu *menu = new BMenu( SpTranslate("Image Format") );
	BMenuItem *item;
	
	//	Find the translator to use for us
	translator_id * all_translators = NULL;
	int32 count = 0;
	status_t err = BTranslatorRoster::Default()->GetAllTranslators(&all_translators, &count);
	if (err >= B_OK)
	{
		err = B_ERROR;
		//	look through all available translators
		for (int ix=0; ix<count; ix++)
		{
			const translation_format *in_formats;
			int32 fmt_count;
			if( B_OK <= BTranslatorRoster::Default()->GetInputFormats(all_translators[ix], &in_formats, &fmt_count) )
			{
				//	look for translators that accept BBitmaps as input
				for (int iy=0; iy<fmt_count; iy++)
				{
					if (in_formats[iy].type == B_TRANSLATOR_BITMAP)
					{
						const translation_format *out_formats;
						if( B_OK <= BTranslatorRoster::Default()->GetOutputFormats(all_translators[ix],	&out_formats, &fmt_count))
						{
							//	look through the output formats
							for (int iy=0; iy<fmt_count; iy++)
							{
								//	and take the first output format that isn't BBitmap
								if( out_formats[iy].type != B_TRANSLATOR_BITMAP)
								{
									message = new BMessage(TRANSLATOR_SELECTED);
									message->AddInt32( "type_code", out_formats[iy].type );
									message->AddString( "MIME", out_formats[iy].MIME );
									item = new BMenuItem( out_formats[iy].name, message);
									item->SetTarget(be_app);
			
									if( first == true )
									{	item->SetMarked( true );
										typecode = out_formats[iy].type;
										first = false;
										message = new BMessage(TRANSLATOR_SELECTED);
										message->AddInt32( "type_code", out_formats[iy].type );
										message->AddString( "MIME", out_formats[iy].MIME );
										be_app->PostMessage(message);
									}
									
									menu->AddItem( item );
								}
							}
						}
					}
				}
			}
		}
	}
	delete[] all_translators;

	return menu;
}
