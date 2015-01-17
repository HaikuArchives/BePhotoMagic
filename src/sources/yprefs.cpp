// libyprefs - preferences library. special written for Herring
// this library is public domain - you can copy, modify, everything
// what you want to do.
// libyprefs is written by Krzysztof Dziuba, e-mail: yeti@viper.pl
// www: www.viper.pl/~yeti

#include "yprefs.h"


// constructor of yprefs, initialize praram_table
yprefs::yprefs()
{
	param_table = new _param_table;
	param_table->items = 0;
}

// destructor of yprefs, delete param_table
yprefs::~yprefs()
{
	delete param_table;
}

// load preferences file to param_table, returns -1 if not loaded, 0 if 
// loaded. file_name MUST contain full path to preferences file
int yprefs::LoadPrefs(const char *file_name)
{
	FILE *file;
	char *temp = new char[2*max_length+4];
	char *temp1 = new char[max_length];
	
	if ((file = fopen(file_name, "r")) == NULL) return -1; else {
		while(fgets(temp, 256, file) != NULL) {
			int x = 0;
			int y = 0;
			while((temp[x] != '=') && (temp[x] != '\0')) {
				temp1[y] = temp[x];
				temp1[y+1] = '\0';
				x++;
				y++;
			}
			y = 0;
			param_table->parameter[param_table->items] = new char[256];
			sprintf(param_table->parameter[param_table->items], "%s", temp1);
			x += 2;
			temp1 = new char[256];
			while((temp[x] != '"') && (temp[x] != '\0')) {
				temp1[y] = temp[x];
				temp1[y+1] = '\0';
				x++;
				y++;
			}
			param_table->value[param_table->items] = new char[256];
			sprintf(param_table->value[param_table->items], "%s", temp1);
			param_table->items++;
			temp = new char[2*max_length+4];
			temp1 = new char[max_length];
		}
		fclose(file);
		return 0;
	}
}

// save param_table to file, returns -1 if not saved, 0 if saves.
// file_name MUST contain full path to preferences file
int yprefs::SavePrefs(const char *file_name)
{
	FILE *file;
	char *temp = new char[2*max_length+4];
	int x = 0;
	
	if ((file = fopen(file_name, "w")) == NULL) return -1; else {
		for(x = 0; x < param_table->items; x++) {
			sprintf(temp, "%s=\"%s\"\n", param_table->parameter[x], param_table->value[x]);
			fputs(temp, file);
			temp = new char[2*max_length+4];
		}
		fclose(file);
		return 0;
	}
}

// get value for param, returns NULL if param doesn`t exist
const char *yprefs::GetValue(const char *param)
{
/*	int x = 0;
	while((strcmp(param, param_table->parameter[x]) != 0) && (x < param_table->items)) x++;
	if (strcmp(param, param_table->parameter[x]) == 0) return param_table->value[x]; 
		else return NULL;
*/	for(int x=0;x<param_table->items;x++)
	{
		if (strcmp(param, param_table->parameter[x]) == 0)
			return param_table->value[x]; 
	}
	return NULL;
}

float yprefs::ptof(const char *string)
{
	if(GetValue(string)!=NULL)
		return atof(GetValue(string));
	else
		return 0.0;
}

long yprefs::ptol(const char *string)
{
	if(GetValue(string)!=NULL)
		return atol(GetValue(string));
	else
		return 0L;
}

int yprefs::ptoi(const char *string)
{
	if(GetValue(string)!=NULL)
		return atoi(GetValue(string));
	else
		return 0;
}


int yprefs::PutParam(const char *param, bool value)
{
	if(value==true)
		return PutParam(param,"1");
	else
		return PutParam(param,"0");
}

int yprefs::PutParam(const char *param, float value)
{	char string[255];
	sprintf(string,"%f",value);
	return PutParam(param,string);
}

int yprefs::PutParam(const char *param, int value)
{	char string[255];
	sprintf(string,"%d",value);
	return PutParam(param,string);
}

// put (creating new) param and value to param_table, returns -1 if
// there isn`t room for new param, else returns 0
int yprefs::PutParam(const char *param, const char *value)
{
	if (param_table->items < max_param) {
		param_table->parameter[param_table->items] = new char[max_length];
		sprintf(param_table->parameter[param_table->items], "%s", param);
		param_table->value[param_table->items] = new char[max_length];
		sprintf(param_table->value[param_table->items], "%s", value);
		param_table->items++;
		return 0;
	} else return -1;
}

// modify existing prameter, returns -1 if param doesn`t exist, else returns 0
int yprefs::ModifyParam(const char *param, const char *value)
{
	int x = 0;
	while((strcmp(param, param_table->parameter[x]) != 0) && (x < param_table->items)) x++;
	if (strcmp(param, param_table->parameter[x]) == 0) {
		param_table->value[x] = new char[max_length];
		sprintf(param_table->value[x], "%s", value);
		return 0;
	} else return -1;
}

// delete existing parameter, returns -1 if param doesn`t exist, else returns 0
int yprefs::DeleteParam(const char *param)
{
	int x = 0;
	while((strcmp(param, param_table->parameter[x]) != 0) && (x < param_table->items)) x++;
	if (strcmp(param, param_table->parameter[x]) == 0) {
		int y;
		for(y = x; y < param_table->items - 1; y++) {
			param_table->parameter[y] = new char[max_length];
			param_table->value[y] = new char[max_length];			
			sprintf(param_table->parameter[y], "%s", param_table->parameter[y+1]);		
			sprintf(param_table->value[y], "%s", param_table->value[y+1]);
		}
		delete param_table->parameter[param_table->items - 1];
		delete param_table->value[param_table->items - 1];
		param_table->items--;
		return 0;
	} else return -1;
}