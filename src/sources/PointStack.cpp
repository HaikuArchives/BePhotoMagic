/*
	PointStack class: quick-and-dirty class to use for Flood fill routines &
	stuff.
	Released under GNU Public License
	Copyright 2001 BePhotoMagic Dev Team
	Written by DarkWyrm
*/
#include "PointStack.h"

PointStack::PointStack(void)
{
	_ptcount=0;
	_stack=new BList(255);
}

PointStack::~PointStack(void)
{
/*	int32 stacksize=_stack->CountItems();
	if(stacksize!=0)
	{	int32 i;
		for(i=0;i<stacksize;i++)
		{	delete (BPoint *)_stack->ItemAt(i);
		}
		_stack->MakeEmpty();
	}
*/
Clear();
}

void PointStack::Push(BPoint pt)
{
//	_stack[_ptcount++]=pt;
	_stack->AddItem(new BPoint(pt));
}

void PointStack::Pop(BPoint *pt)
{
/*	if(_ptcount < MAX_POINTS)
	{	pt->x=_stack[--_ptcount].x;
		pt->y=_stack[_ptcount].y;
	}
	else
	{	pt->x=-1;
		pt->y=-1;
	}
*/
	if(_stack->CountItems()>0)
	{	BPoint *poppeditem=(BPoint *)_stack->LastItem();
		_stack->RemoveItem(_stack->CountItems()-1);
		pt->x=poppeditem->x;
		pt->y=poppeditem->y;
		delete poppeditem;
	}
}

void PointStack::Clear(void)
{
//	_ptcount=0;
	int32 stacksize=_stack->CountItems();
	if(stacksize!=0)
	{	int32 i;
		for(i=0;i<stacksize;i++)
		{	delete (BPoint *)_stack->ItemAt(i);
		}
		_stack->MakeEmpty();
	}
}

uint32 PointStack::Count(void)
{	//	return _ptcount;
	return _stack->CountItems();
}