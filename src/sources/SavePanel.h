#ifndef _SAVE_PANEL_H
#define _SAVE_PANEL_H

#include <Application.h>
#include <TranslatorRoster.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <FilePanel.h>
#include <Button.h>

#include "BPMView.h"
#include "SpLocaleApp.h"

#define BPM_SAVE_REF	'bsrf'
#define TRANSLATOR_SELECTED 'tlsl'

class BPMSavePanel : public BFilePanel
{
public:
	BPMSavePanel(BMessage *msg);
	BMenu *BuildFormatsMenu(void);
	//virtual void SelectionChanged();
	BMenuField		*format;
	entry_ref old_dir_ref,the_active_ref;
};
/*
class ImgRefFilter : public BRefFilter
{
public:
ImgRefFilter(void);
virtual bool Filter(const entry_ref *ref, 
         BNode *node, 
         struct stat *st, 
         const char *filetype);
};
*/
#endif
