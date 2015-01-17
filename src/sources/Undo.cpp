#include "Undo.h"
#include <stdio.h>

UndoData::UndoData(void)
{	layers.MakeEmpty();
}

UndoData::~UndoData(void)
{
	if(bitmap != NULL)
		delete bitmap;

	Layer *current;
	if(layers.CountItems()!=0)
	{	int32 i;
		for(i=0;i<layers.CountItems();i++)
		{	
			current=(Layer *)layers.ItemAt(i);
			if(current!=NULL)
				delete current;
		}
		layers.MakeEmpty();
	}

}

