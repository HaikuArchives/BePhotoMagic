// libyprefs - preferences library. special written for Herring
// this library is public domain - you can copy, modify, everything
// what you want to do.
// libyprefs is written by Krzysztof Dziuba, email: yeti@viper.pl
// www: www.viper.pl/~yeti

#ifndef _Y_PREFS_H

#define _Y_PREFS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define max_param 500	// define maximum of available parameters
#define max_length 256	// define max length of parameter and value

class yprefs
{
	public:
		yprefs();
		~yprefs();
		int LoadPrefs(const char *file_name);	// load preferences file
		int SavePrefs(const char *file_name);	// save preferences file
		const char *GetValue(const char *param);	// get value for param
		int PutParam(const char *param, bool value);
		int PutParam(const char *param, float value);
		int PutParam(const char *param, int value);
		int PutParam(const char *param, const char *value);	// put new parameter
															// and value
		int ModifyParam(const char *param, const char *value);	// modify parameter`s value
		int DeleteParam(const char *param);	// delete parameter
		int ptoi(const char *string);
		float ptof(const char *string);
		long ptol(const char *string);

	private:
		struct _param_table {
			int items;	// number of available items
			char *parameter[max_param]; // parameters table
			char *value[max_param];	// values table
		} *param_table;
		
};

#endif