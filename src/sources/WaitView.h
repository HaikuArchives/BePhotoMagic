#ifndef _WAITVIEW_H_

#define _WAITVIEW_H_

#include <View.h>
#include <String.h>

#include "SpLocaleApp.h"

class WaitView : public BView
{
public:
	WaitView(BRect size);
	WaitView(BRect size, const char *message);
	virtual void Draw(BRect update);
	BString text;
	BPoint textpt;
};

#endif