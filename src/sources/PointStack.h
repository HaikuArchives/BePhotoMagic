/*
	PointStack class: quick-and-dirty class to use for Flood fill routines &
	stuff.
	Released under GNU Public License
	Copyright 2001 BePhotoMagic Dev Team
	Written by DarkWyrm
*/
#ifndef _POINTSTACK_H_

#define _POINTSTACK_H_

#include <Point.h>
#include <List.h>

// A hack until I learn how to do dynamic BPoint allocation...
#define MAX_POINTS	1048576

class PointStack
{
public:
	PointStack(void);
	~PointStack(void);
	
	void Push(BPoint pt);
	void Pop(BPoint *pt);
	void Clear(void);
	uint32 Count(void);
//private:
//	BPoint _stack[MAX_POINTS];
	BList *_stack;
	uint32 _ptcount;
};

#endif