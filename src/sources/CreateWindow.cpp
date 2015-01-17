#include "CreateWindow.h"

CreateWindow::CreateWindow(BRect r)
				: BWindow(r, SpTranslate("Create New Document"), B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK)
{
createview = new CreateView(Bounds());
AddChild(createview);

old_unit=0;
old_unittype=0;

PostMessage(new BMessage(COORD_CHANGED)); 


//activate the first paper format
BMessage *x = new BMessage(ACTIVATE_FORMAT);
x->AddInt32("number",0);
PostMessage(x);
Show();
}


CreateWindow::~CreateWindow()
{
}


void CreateWindow::MessageReceived(BMessage *msg)
{

char str[255];
float val,valb;
int32 number,new_unit,unittype;

	switch (msg->what)
	{
		case DO_IT:
			// Force an update in case we hit OK without tabbing out of a
			// image size box
			UpdatePicSize();
			BMessage *cmsg;
			cmsg=new BMessage(CREATE_IMAGE);
			cmsg->AddInt16("width",createview->pix_x);
			cmsg->AddInt16("height",createview->pix_y);
			be_app->PostMessage(cmsg);
			Lock();
			Close();
			break;
		
		case CANCEL:
			Quit();
			break;

		case COORD_CHANGED:
			UpdatePicSize();
			break;

		case UNITS_CHANGED:
			val  = atof(createview->x->Text());
			valb = atof(createview->y->Text());
		
			new_unit  = createview->units->Menu()->IndexOf(createview->units->Menu()->FindMarked());
			unittype  = createview->unittype->Menu()->IndexOf(createview->unittype->Menu()->FindMarked());
		
			//unit,int32 dest_unit, float resolution, float res_units)
			val = ConvertUnits(val, old_unit, new_unit,atof(createview->res->Text()),unittype);
			valb = ConvertUnits(valb, old_unit, new_unit,atof(createview->res->Text()),unittype);
		
			if (new_unit==UNIT_PIXELS)
			{
				sprintf(str,"%ld",int32(val));
				createview->x->SetText(str);
				sprintf(str,"%ld",int32(valb));
				createview->y->SetText(str);
			}
			else
			{
				sprintf(str,"%.2f",val);   createview->x->SetText(str);
				sprintf(str,"%.2f",valb);  createview->y->SetText(str);
			}
		
		
			UpdatePicSize();
			old_unit=createview->units->Menu()->IndexOf(createview->units->Menu()->FindMarked());
			break;

		case RES_CHANGED:
			UpdatePicSize();
			break;
		
		case UNIT_TYPE_CHANGED:
		
			val = atof(createview->res->Text());
		
			//only if it has really changed
			if (old_unittype!=createview->unittype->Menu()->IndexOf(createview->unittype->Menu()->FindMarked()))
			{
				if (createview->unittype->Menu()->IndexOf(createview->unittype->Menu()->FindMarked())==DPI) 
				{ 	val *= 2.54;
				}
				else 
				{
					val /= 2.54; 
				}
			}
			
			sprintf(str,"%.2f",val);
			createview->res->SetText(str);
		
			old_unittype=createview->unittype->Menu()->IndexOf(createview->unittype->Menu()->FindMarked());
		
			UpdatePicSize();
			break;

		case ACTIVATE_FORMAT:
			msg->FindInt32("number",&number);

			createview->unittype->Menu()->ItemAt(createview->the_formats[number]->unit_type)->SetMarked(true);
			createview->unittype->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre

			createview->units->Menu()->ItemAt(createview->the_formats[number]->units)->SetMarked(true);
			createview->units->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre
		
			sprintf(str,"%.2f",createview->the_formats[number]->res);	createview->res->SetText(str);

			if (createview->the_formats[number]->units==UNIT_PIXELS)
			{
				//Les pixels peuvent pas avoir de virgule!
				sprintf(str,"%ld",int32(createview->the_formats[number]->x));
				createview->x->SetText(str);
				sprintf(str,"%ld",int32(createview->the_formats[number]->y));
				createview->y->SetText(str);
			}
			else
			{
				sprintf(str,"%.2f",createview->the_formats[number]->x);
				createview->x->SetText(str);
				sprintf(str,"%.2f",createview->the_formats[number]->y);
				createview->y->SetText(str);
			}
		
			// very important to convert units
			old_unit = createview->the_formats[number]->units;
			old_unittype = createview->the_formats[number]->unit_type;
	
		
			UpdatePicSize();
			break;
  	 }

//post message to parent window
//util.mainWin->PostMessage(new BMessage(Create_CHANGED)); 

}//end MessageReceived


void CreateWindow::UpdatePicSize()
{

//on doit obtenir des pixels à partir de xx,yy;
float xx = atof(createview->x->Text());
float yy = atof(createview->y->Text());
		

float divider =0;
if (createview->unittype->Menu()->IndexOf(createview->unittype->Menu()->FindMarked())==DPI)
	//dots per inch (dpi)
	divider=2.54;
else
	//dots per centimeterd
	divider = 1;
		
float inch_divider;
if (divider ==1)
	inch_divider=2.54;
else 
	inch_divider=1;
		
float res = atof(createview->res->Text());
		
switch(createview->units->Menu()->IndexOf(createview->units->Menu()->FindMarked()))
{	
	case UNIT_CM:
		xx *= (res/divider);
		yy *= (res/divider);
		break;
			
	case UNIT_MM:
		xx *= (res/(divider*10));
		yy *= (res/(divider*10));
		break;
					
	case UNIT_INCH:
		xx *= (res*inch_divider);
		yy *= (res*inch_divider);
		break;
					
	case UNIT_PIXELS:
		//do nothing
		break;
			
}
		
char s[255];	

sprintf(s,"(%ld pixels)",int32(xx));
createview->pixel_x->SetText(s);
sprintf(s,"(%ld pixels)",int32(yy));
createview->pixel_y->SetText(s);

//values used during creation
createview->pix_x = int32(xx);
createview->pix_y = int32(yy);

float sz= (xx *yy *4) ;
char str[255];
char st_base[255];

sprintf(st_base, SpTranslate("Size"));
strcat(st_base,": ");
if (sz > 32768)
{	sz /= 1024;
	sprintf(str,"%G Kb",sz);
	strcat(st_base,str);
}
else
{ 	sprintf(str,"(%G bytes)",sz);
	strcat(st_base,str);
}
createview->taille->SetText(st_base);

}

float CreateWindow::ConvertUnits(float val, int32 src_unit,int32 dest_unit, float resolution, float res_units)
{
	float divider =0;
	if (res_units==DPI)
		divider=2.54; 		//dots per inch (dpi)
	else
		divider = 1;		//dots per centimeterd
	
	float inch_divider;
	if (divider ==1)
		inch_divider=2.54;
	else
		inch_divider=1;
	
	switch(src_unit)
	{
		case UNIT_CM:
			switch(dest_unit)
			{
				case UNIT_CM:			 				break;
				case UNIT_MM:   	val *= 10;	 		break;
				case UNIT_INCH: 	val /= 2.54;		break;
				case UNIT_PIXELS: 	val  *= (resolution/divider); break;
			}
			break;
					
		case UNIT_MM:
			switch(dest_unit)
			{
				case UNIT_CM:		val /= 10;			break;
				case UNIT_MM:   						break;
				case UNIT_INCH: 	val /= (2.54*10);	break;
				case UNIT_PIXELS: 	val  *= (resolution/divider)/10;	break;
			}
			break;
					
		case UNIT_INCH:
			switch(dest_unit)
			{
				case UNIT_CM:		val *= 2.54;			break; 
				case UNIT_MM:   	val *= (2.54*10);		break;
				case UNIT_INCH: 	break;
				case UNIT_PIXELS: 	val  *= (resolution*inch_divider);	break;
			}
			break;
					
		case UNIT_PIXELS:
			switch(dest_unit)
			{
				case UNIT_CM: 		val  /= (resolution/divider);		break;
										
				case UNIT_MM:   	val  /= (resolution/divider)/10;	break; 
									
				case UNIT_INCH: 	val  /= (resolution*inch_divider);	break; 
								
				case UNIT_PIXELS: 	break;
			}
			break;
	}
	return val;
}	

PaperFormat::PaperFormat(char the_name[64], int32 the_unittype, float the_res, int32 the_units, float the_x,float the_y)
{
sprintf(name,the_name);
unit_type = the_unittype;
res = the_res;
units = the_units;
x = the_x;
y = the_y;
}

bool CreateWindow::QuitRequested()
{
	be_app->PostMessage(CANCEL_CREATE_IMAGE);
	return(true);
}

CreateView::CreateView(BRect rec):
	BView(rec, "create view", B_FOLLOW_ALL, B_WILL_DRAW)
{
SetViewColor(216,216,216);

BRect rect;

rect.Set (4,4,Bounds().Width()-4,4+16);
taille = new BStringView(rect,NULL,"",B_FOLLOW_ALL,B_WILL_DRAW);
taille->SetAlignment(B_ALIGN_CENTER);
AddChild(taille);

char str[255]; 
//char str2[255];
sprintf(str,"Untitled");
//sprintf(str2," %d",shared->image_amount+1); 
//sprintf(str2," %d",shared->image_amount); 
//strcat(str,str2);

rect.OffsetBy(0,24);
name_field = new BTextControl(rect,NULL,NULL,str,new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);
AddChild(name_field);


int32 q =0;
the_formats[q] = new PaperFormat("320 x 240 pixels",DPI,72,UNIT_PIXELS,320,240);	q++;
the_formats[q] = new PaperFormat("640 x 480 pixels",DPI,72,UNIT_PIXELS,640,480);	q++;
the_formats[q] = new PaperFormat("800 x 600 pixels",DPI,72,UNIT_PIXELS,800,600);	q++;
the_formats[q] = new PaperFormat("1024 x 768 pixels",DPI,72,UNIT_PIXELS,1024,768);	q++;
the_formats[q] = new PaperFormat("A6 (105 x 148 mm)",DPI,300,UNIT_MM,105,148);		q++;
the_formats[q] = new PaperFormat("A5 (148 x 210 mm)",DPI,300,UNIT_MM,210,148);		q++;
the_formats[q] = new PaperFormat("A4 (210 x 297 mm)",DPI,300,UNIT_MM,210,297);		q++;
the_formats[q] = new PaperFormat("A3 (297 x 420 mm)",DPI,300,UNIT_MM,297,420);				
format_amount=q;


rect.OffsetBy(0,32);
rect.right=240;
BMenu *format_menu = new BMenu("");
BMessage *messie;
BMenuItem *ait;

messie = new BMessage(ACTIVATE_FORMAT);
messie->AddInt32("number",0);

format_menu->AddItem(ait = new BMenuItem(the_formats[0]->name,   messie) );

int32 g=1;

while (g != format_amount+1)
{
	messie = new BMessage(ACTIVATE_FORMAT); messie->AddInt32("number",g);
	format_menu->AddItem(new BMenuItem(the_formats[g]->name,   messie) );
	g++;
}
ait->SetMarked(true);

format_menu->SetLabelFromMarked(true);
format_field = new BMenuField(rect,"",SpTranslate("Formats"),format_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
format_field->SetDivider(80);
format_menu->SetLabelFromMarked(true);
AddChild(format_field);


rect.OffsetBy(0,50);
//rect.top = 64+4;
//rect.bottom= 64+4+16;
rect.left =4;
rect.right =4+80;
x = new BTextControl(rect,NULL,NULL,"14.8",new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

rect.OffsetBy(0,24);
y = new BTextControl(rect,NULL,NULL,"10.5",new BMessage(COORD_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

rect.OffsetBy(0,24);
res = new BTextControl(rect,NULL,NULL,"300",new BMessage(RES_CHANGED), B_FOLLOW_ALL | B_NAVIGABLE, B_WILL_DRAW);

AddChild(x);
AddChild(y);
AddChild(res);

BMenuItem *it;
rect.OffsetBy(4+80+16-1,-24*2);
rect.right+=60;
BMenu *unit_menu = new BMenu("");
unit_menu->AddItem(it = new BMenuItem(SpTranslate("Centimeters"),   new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem(SpTranslate("Millimeters"),    new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem(SpTranslate("Inches"), new BMessage(UNITS_CHANGED)));
unit_menu->AddItem(new BMenuItem(SpTranslate("Pixels"), new BMessage(UNITS_CHANGED))); //pixels cannot be translated in any language...

it->SetMarked(true);

unit_menu->SetLabelFromMarked(true);
units = new BMenuField(rect,"",NULL,unit_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
unit_menu->SetLabelFromMarked(true);
AddChild(units);


//DPI Dots/cm

rect.OffsetBy(0,24+24);
BMenuItem *xit;
BMenu *res_menu = new BMenu("");

char str_a[256];
char str_b[256];
sprintf(str_a, "pixels/"); 
strcat(str_a,SpTranslate("Inch"));
strcat(str_a," (dpi)"); 

sprintf(str_b, "pixels/");
strcat(str_b,SpTranslate("Centimeter"));
strcat(str_b," (dpcm)");

res_menu->AddItem(xit = new BMenuItem(str_a, new BMessage(UNIT_TYPE_CHANGED)));
res_menu->AddItem(new BMenuItem(str_b, new BMessage(UNIT_TYPE_CHANGED)));

xit->SetMarked(true);

res_menu->SetLabelFromMarked(true);
unittype = new BMenuField(rect,"",NULL,res_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
res_menu->SetLabelFromMarked(true);
AddChild(unittype);


rect.OffsetBy(140,-24-24);
pixel_x = new BStringView(rect,"xpix","xxx",B_FOLLOW_ALL,B_WILL_DRAW);
rect.OffsetBy(0,24);
pixel_y = new BStringView(rect,"ypix","yyy",B_FOLLOW_ALL,B_WILL_DRAW);

AddChild(pixel_x);
AddChild(pixel_y);

//---------- BUTTONS

rect.bottom= Bounds().bottom-24;
rect.top = Bounds().bottom-32;
rect.left =8;
rect.right =8+80;
BButton *cancel = new BButton(rect,"",SpTranslate("Cancel"),new BMessage(CANCEL),B_FOLLOW_ALL,B_WILL_DRAW | B_NAVIGABLE);
AddChild(cancel);

rect.right = Bounds().Width()-8;
rect.left = Bounds().Width()-8-80;
BButton *do_it = new BButton(rect,"",SpTranslate("OK"),new BMessage(DO_IT),B_FOLLOW_ALL,B_WILL_DRAW | B_NAVIGABLE);
AddChild(do_it);
do_it->MakeDefault(true);
}

