#ifndef CREATE_WINDOW_H
#define CREATE_WINDOW_H

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Message.h>
#include <Button.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SpLocaleApp.h"

#define CREATE_IMAGE	'cwci'
#define CANCEL_CREATE_IMAGE	'cwcc'
#define DO_IT			'doit'
#define CANCEL			'cwcl'
#define COORD_CHANGED	'cwco'
#define UNITS_CHANGED	'cwuc'
#define RES_CHANGED		'cwrc'
#define UNIT_TYPE_CHANGED 'cwtc'
#define ACTIVATE_FORMAT 'cwaf'

#define UNIT_CM 0
#define UNIT_MM 1
#define UNIT_INCH 2
#define UNIT_PIXELS 3
#define DPI 0
#define DPCM 1
#define MM_PER_INCH 25.4

class PaperFormat 
{
public:
	PaperFormat(char the_name[64],int32 the_res_type,float the_res,
				int32 the_units,float the_x,float the_y);
	char name[64];
	int32 unit_type;
	float res;
	int32 units;
	float x;
	float y;
};

class CreateView : public BView 
{
public:
	CreateView(BRect frame); 

	BTextControl *x,*y,*res,*name_field;
	BStringView *taille;
	BStringView *pixel_x,*pixel_y;
	BMenuField *units,*format_field;
	BMenuField *unittype;

	PaperFormat *the_formats[255];
	int32 pix_x,pix_y;
	int32 format_amount;
};

class CreateWindow : public BWindow 
{
public:
	CreateWindow(BRect r);
	~CreateWindow();
	virtual	bool QuitRequested();
	
	CreateView *createview;

	void MessageReceived(BMessage *msg);

	float ConvertUnits(float val, int32 src_unit,int32 dest_unit, float resolution, float res_units);
	void UpdatePicSize();

	int32 old_unit;
	int32 old_unittype;
};


#endif
