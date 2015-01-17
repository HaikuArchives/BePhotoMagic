#include "BPMImage.h"

// Extension of BPMImage.cpp which just defines all the tool functions

void BPMImage::PaintBrush(BPoint pt,bool fore=true,bool zoom_comp=true)
{
	if(zoom_comp)
	{	pt.x = floor(pt.x / zoom);
		pt.y = floor(pt.y / zoom);
	}

	pt.ConstrainTo(display_bitmap->Bounds());

	int32 	x=int32(pt.x),
			y=int32(pt.y);
	rgb_color paintcolor=(fore==true)?forecolor:backcolor;

	int16 	pred=paintcolor.red,
			pgreen=paintcolor.green,
			pblue=paintcolor.blue,
			palpha=paintcolor.alpha,
			srcred,srcgreen,srcblue,
			rdelta,gdelta,bdelta,adelta,
			trans,tempval=0,diffr,diffg,diffb,
			grayval;

	// nothing to do if alpha is 0
	if(palpha==0)
		return;

	// data vars for brush and for bitmap
	uint8 *brush_data=(uint8 *)currentbrush->Bits(),
			*img_data=(uint8 *)p_active_layer->bitmap->Bits(),
			*work_data=(uint8 *)work_bitmap->Bits(),
			*brush_pos,*img_pos,*work_pos;
			
	uint32 	brush_rowsize=currentbrush->BytesPerRow(),
			img_rowsize=p_active_layer->bitmap->BytesPerRow(),
			work_rowsize=work_bitmap->BytesPerRow();
	uint32 rows,columns,bitheight,bitwidth,
			lclip,rclip,tclip,bclip;	// integer vars for clipping rectangle
	BRect brush_rect,brushbounds,bmpbounds,workbounds,undorect;
	float tfrac;

	brushbounds=currentbrush->Bounds();
	bmpbounds=display_bitmap->Bounds();
	workbounds=work_bitmap->Bounds();
	
	// Create the necessary gray value and set proper pointers if in mask mode
	if(maskmode==true)
	{	grayval=ROUND( float(pred*pgreen*pblue)/3.0 );

		// set to mask color
		pred=128;
		pgreen=128;
		pblue=128;
//-----------------------------------------------------------------------		
		palpha=grayval;		// is this correct??
//-----------------------------------------------------------------------		
		img_data=(uint8 *)selection->Bits();
		img_rowsize=selection->BytesPerRow();
		bmpbounds=selection->Bounds();
	}
	// dimensions of bitmap for brush - NOT actual brush dimensions
	bitwidth=currentbrush->Bounds().IntegerWidth();
	bitheight=currentbrush->Bounds().IntegerHeight();

	// jump by offset so that brush's center is where we clicked
	x -=int32(bitwidth/2);
	y -=int32(bitheight/2);
	
	// Get brush dimensions in image
	brush_rect.left=x;
	brush_rect.top=y;
	brush_rect.right=x+bitwidth;
	brush_rect.bottom=y+bitheight;
	undorect=brush_rect;
	
	// Assign brush bitmap clipping rectangle proper values
	tclip=lclip=0;
	rclip=(int32)brushbounds.right;
	bclip=(int32)brushbounds.bottom;

	if(brush_rect.left < 0)
	{	lclip=int32(brush_rect.left * -1);
		undorect.left=0;
	}
	if(brush_rect.top < 0)
	{	tclip=int32(brush_rect.top * -1);
		undorect.top=0;
	}
	if(brush_rect.right > bmpbounds.right)
	{	rclip -= int32(brush_rect.right-bmpbounds.right);
		undorect.right=bmpbounds.right;
	}
	if(brush_rect.bottom > bmpbounds.bottom)
	{	bclip -= int32(brush_rect.bottom-bmpbounds.bottom);
		undorect.bottom=bmpbounds.bottom;
	}

	if(save_each_paint==true)
		SaveUndo(UNDO_PAINTBRUSH,undorect);

	// Jump to area in question in image
	img_data += int32((y*img_rowsize) + (x *4));
	work_data += int32((y*img_rowsize) + (x *4));

	for(rows=tclip;rows<=bclip;rows++)
	{	
		img_pos=(uint8*)(img_data+(img_rowsize * rows)+(lclip*4));
		brush_pos=(uint8*)(brush_data+(brush_rowsize * rows)+(lclip*4));
		work_pos=(uint8*)(work_data+(work_rowsize * rows)+(lclip*4));
		
		for(columns=lclip;columns<=rclip;columns++)
		{
			trans=int16( (255-brush_pos[1])*(float(palpha)/255.0));
			tfrac=float(trans)/255;

			// This has been added to prevent painting on paint in
			// nonnormal blending modes
			if(blendmode==TNORMAL)
			{
				srcred=img_pos[2];
				srcgreen=img_pos[1];
				srcblue=img_pos[0];
			}
			else
			{
				srcred=work_pos[2];
				srcgreen=work_pos[1];
				srcblue=work_pos[0];
			}
			
			switch(blendmode)
			{
				case TMULTIPLY:
				{
					diffb=int16(float(pblue*srcblue)/255.0);
					diffg=int16(float(pgreen*srcgreen)/255.0);
					diffr=int16(float(pred*srcred)/255.0);
					break;
				}
				case TDIVIDE:
				{	if(pred==0) pred=1;
					if(pgreen==0) pgreen=1;
					if(pblue==0) pblue=1;
					diffb=int16( float(srcblue*255)/float(pblue) );
					diffg=int16( float(srcgreen*255)/float(pgreen) );
					diffr=int16( float(srcred*255)/float(pred) );
					if(diffb>255) diffb=255;
					if(diffg>255) diffg=255;
					if(diffr>255) diffr=255;
					break;
				}
				case TDIFFERENCE:
				{
					diffb=pblue-srcblue;
					diffg=pgreen-srcgreen;
					diffr=pred-srcred;
					break;
				}
				case TEXCLUSION:
				{	
					diffb=srcblue ^ pblue;
					diffg=srcgreen ^ pgreen;
					diffr=srcred ^ pred;
					break;
				}
				case TLIGHTEN:
				{	
					diffb=MAX(srcblue,pblue);
					diffg=MAX(srcgreen,pgreen);
					diffr=MAX(srcred,pred);
					break;
				}
				case TDARKEN:
				{	
					diffb=MIN(srcblue,pblue);
					diffg=MIN(srcgreen,pgreen);
					diffr=MIN(srcred,pred);
					break;
				}
				case TOR:
				{	
					diffb=pblue|srcblue;
					diffg=pgreen|srcgreen;
					diffr=pred|srcred;
					break;
				}
				case TAVERAGE:
				{	
					diffb=(pblue+srcblue)>>1;
					diffg=(pgreen+srcgreen)>>1;
					diffr=(pred+srcred)>>1;
					break;
				}
				case TADDITION:
				{	
					diffb=srcblue+pblue;
					diffg=srcgreen+pgreen;
					diffr=srcred+pred;
					if(diffb>255) diffb=255;
					if(diffg>255) diffg=255;
					if(diffr>255) diffr=255;
					break;
				}
				case TSUBTRACT:
				{	
					diffb=srcblue-pblue;
					diffg=srcgreen-pgreen;
					diffr=srcred-pred;
					if(diffb<0) diffb=0;
					if(diffg<0) diffg=0;
					if(diffr<0) diffr=0;
					break;
				}
				default:
				{
					diffb=pblue;
					diffg=pgreen;
					diffr=pred;
				}
			}

			// Normal painting mode
			if(erasemode==false)
			{
				if(diffb<0)	diffb *= -1;
				bdelta=diffb-srcblue;
				tempval=srcblue + int16(float(bdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					img_pos[0] = tempval;
	
				if(diffg<0)	diffg *= -1;
				gdelta=diffg-srcgreen;
				tempval=srcgreen + int16(float(gdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					img_pos[1] = tempval;
	
				if(diffr<0)	diffr *= -1;
				rdelta=diffr-srcred;
				tempval=srcred + int16(float(rdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					img_pos[2] = tempval;
	
				adelta=img_pos[3]-palpha;
				tempval=img_pos[3] + int16(float(adelta) * tfrac);

				if(maskmode==false)
				{	if(tempval<0)	tempval *= -1;
					if(tempval>255)
						tempval&=255;
				}
				else
				{	tempval=work_pos[3] - int16(float(work_pos[3]) * tfrac);
					if(tempval<0)	tempval=0;
					img_pos[3]=tempval;
				}
				img_pos[3]=tempval;
			}
			else
			{	// erasing image
				if(maskmode==false)
				{	if(tfrac > 0.0)
					{	tempval=work_pos[3] - int16(float(work_pos[3]) * tfrac);
						if(tempval<0)	tempval=0;
						img_pos[3]=tempval;
					}
				}
				else	// erasing mask == increasing selected area
				{
					adelta=img_pos[3]-palpha;
					tempval=img_pos[3] + int16(float(adelta) * tfrac);
	
					if(tempval<0)	tempval=0;
					if(tempval>255)
						tempval=255;
					img_pos[3]=tempval;
				}
			}

			img_pos+=4;
			brush_pos+=4;
			work_pos+=4;
		}	// end for columns
	}	// end for rows
}

void BPMImage::PaintLine(BPoint start,BPoint end,bool fore=true)
{
	start.ConstrainTo(display_bitmap->Bounds());
	end.ConstrainTo(display_bitmap->Bounds());
	
	int32 	startx,starty,endx,endy,
			dx,dy,dist,iterations;
	float stepfrac,stepx,stepy;
	BPoint currentpt;
	bool saved_undo_state=save_each_paint;

	rgb_color paintcolor=(fore==true)?forecolor:backcolor;

	// We're going to use an interative method, being we're going to use a pixel-based
	// step size. Midpoint algorithm goes right out the window
	if(start.x<end.x)
	{	startx=(int32)start.x;
		starty=(int32)start.y;
		endx=(int32)end.x;
		endy=(int32)end.y;
	}
	else
	{	startx=(int32)end.x;
		starty=(int32)end.y;
		endx=(int32)start.x;
		endy=(int32)start.y;
	}

	dx=endx-startx;
	dy=endy-starty;

	dist=(int32)sqrt((dx*dx)+(dy*dy));

	// this is how far we go toward the second point
	stepfrac=float(linestep)/float(dist);

	// save the rectangle between the endpoints
	BRect undorect,brushrect,brushrect2;
	brushrect=GetBrushedRect(start,false);
	brushrect2=GetBrushedRect(end,false);

	undorect.left=MIN(brushrect.left,brushrect2.left);
	undorect.right=MAX(brushrect.right,brushrect2.right);
	undorect.top=MIN(brushrect.top,brushrect2.top);
	undorect.bottom=MAX(brushrect.bottom,brushrect2.bottom);

	if(save_each_paint==true)
		SaveUndo(UNDO_TOOL,undorect);
	
	// Draw the end points
	save_each_paint=false;
	PaintBrush(start,fore,false);
	PaintBrush(end,fore,false);

	// If step bigger than distance between points, we exit
	if(stepfrac>1.0)
		return;	

	iterations=int32( float(dist)/float(linestep));

	stepx=float(dx)*stepfrac;
	stepy=float(dy)*stepfrac;

	currentpt.Set(float(startx)+stepx,float(starty)+stepy);

	for(int32 i=0;i<iterations;i++)
	{	PaintBrush(currentpt,fore,false);
		currentpt.x+=stepx;
		currentpt.y+=stepy;
	}
	save_each_paint=saved_undo_state;
}

void BPMImage::PaintRect(BRect frame,bool fore=true)
{
	bool saved_undo_state=save_each_paint;
	BRect undorect,brushrect,brushrect2;
	brushrect=GetBrushedRect(frame.LeftTop(),false);
	brushrect2=GetBrushedRect(frame.RightBottom(),false);

	undorect.left=MIN(brushrect.left,brushrect2.left);
	undorect.right=MAX(brushrect.right,brushrect2.right);
	undorect.top=MIN(brushrect.top,brushrect2.top);
	undorect.bottom=MAX(brushrect.bottom,brushrect2.bottom);

	if(save_each_paint==true)
		SaveUndo(UNDO_TOOL,undorect);

	save_each_paint=false;
	PaintLine(frame.LeftTop(),frame.RightTop(),fore); 
	PaintLine(frame.LeftBottom(),frame.RightBottom(),fore); 
	PaintLine(frame.LeftTop(),frame.LeftBottom(),fore); 
	PaintLine(frame.RightTop(),frame.RightBottom(),fore); 
	save_each_paint=saved_undo_state;
}

void BPMImage::PaintFRect(BRect frame,bool fore=true)
{
	bool saved_undo_state=save_each_paint;
	BRect undorect,brushrect,brushrect2;
	brushrect=GetBrushedRect(frame.LeftTop(),false);
	brushrect2=GetBrushedRect(frame.RightBottom(),false);

	undorect.left=MIN(brushrect.left,brushrect2.left);
	undorect.right=MAX(brushrect.right,brushrect2.right);
	undorect.top=MIN(brushrect.top,brushrect2.top);
	undorect.bottom=MAX(brushrect.bottom,brushrect2.bottom);

	if(save_each_paint==true)
		SaveUndo(UNDO_TOOL,undorect);

	BBitmap *dbmp=p_active_layer->bitmap;
	BView draw(dbmp->Bounds(),"dview",B_FOLLOW_NONE,0);
	dbmp->Lock();
	dbmp->AddChild(&draw);
	draw.SetPenSize(1);
	if(fore==true)
		draw.SetHighColor(forecolor);
	else
		draw.SetHighColor(backcolor);
	draw.FillRect(frame);
	draw.RemoveSelf();
	dbmp->Unlock();

	save_each_paint=false;
//	PaintLine(frame.LeftTop(),frame.RightTop(),fore); 
//	PaintLine(frame.LeftBottom(),frame.RightBottom(),fore); 
//	PaintLine(frame.LeftTop(),frame.LeftBottom(),fore); 
//	PaintLine(frame.RightTop(),frame.RightBottom(),fore); 
	PaintRect(frame,fore);
	save_each_paint=saved_undo_state;
}

void BPMImage::PaintFEllipse(BRect frame,bool fore=true)
{
	bool saved_undo_state=save_each_paint;
	BRect undorect,brushrect,brushrect2;
	brushrect=GetBrushedRect(frame.LeftTop(),false);
	brushrect2=GetBrushedRect(frame.RightBottom(),false);

	undorect.left=MIN(brushrect.left,brushrect2.left);
	undorect.right=MAX(brushrect.right,brushrect2.right);
	undorect.top=MIN(brushrect.top,brushrect2.top);
	undorect.bottom=MAX(brushrect.bottom,brushrect2.bottom);

	if(save_each_paint==true)
		SaveUndo(UNDO_TOOL,undorect);

	// Draw the filled ellipse onto the layer
	BBitmap *dbmp=p_active_layer->bitmap;
	BView draw(dbmp->Bounds(),"dview",B_FOLLOW_NONE,0);
	dbmp->Lock();
	dbmp->AddChild(&draw);
	draw.SetPenSize(1);
	if(fore==true)
		draw.SetHighColor(forecolor);
	else
		draw.SetHighColor(backcolor);
	draw.FillEllipse(frame);
	draw.RemoveSelf();
	dbmp->Unlock();

	// Draw the ellipse which we saw in the preview
	save_each_paint=false;
	PaintEllipse(frame,fore);
	save_each_paint=saved_undo_state;
}

void BPMImage::PaintEllipse(BRect frame,bool fore=true)
{	// tweak interface to app for laziness purporses
	bool saved_undo_state=save_each_paint;
	int Rx = (int)floor(frame.IntegerWidth()/2);
	int Ry = (int)floor(frame.IntegerHeight()/2);
	int xCenter=(int)frame.left + Rx;
	int yCenter=(int)frame.top + Ry;
	int Rx2 = Rx*Rx;
  	int Ry2 = Ry*Ry;
  	int twoRx2 = 2*Rx2;
  	int twoRy2 = 2*Ry2;
  	int p;
  	int x = 0;
  	int y = Ry;
  	int px = 0;
  	int py = twoRx2 * y;

	BRect undorect,brushrect,brushrect2;
	brushrect=GetBrushedRect(frame.LeftTop(),false);
	brushrect2=GetBrushedRect(frame.RightBottom(),false);

	undorect.left=MIN(brushrect.left,brushrect2.left);
	undorect.right=MAX(brushrect.right,brushrect2.right);
	undorect.top=MIN(brushrect.top,brushrect2.top);
	undorect.bottom=MAX(brushrect.bottom,brushrect2.bottom);


	if(save_each_paint==true)
		SaveUndo(UNDO_TOOL,undorect);

	save_each_paint=false;
  	// Plot the first set of points
  	//ellipsePlotPoints (xCenter, yCenter, x, y);
	PaintBrush(BPoint(xCenter + x, yCenter + y),fore); 
 	PaintBrush(BPoint(xCenter - x, yCenter + y),fore); 
	PaintBrush(BPoint(xCenter + x, yCenter - y),fore); 
  	PaintBrush(BPoint(xCenter - x, yCenter - y),fore); 

  	// Region 1
  	p = ROUND (Ry2 - (Rx2 * Ry) + (0.25 * Rx2));
  	while (px < py) 
  	{
    	x++;
    	px += twoRy2;
    	if (p < 0)
      		p += Ry2 + px;
    	else 
    	{
      		y--;
      		py -= twoRx2;
      		p += Ry2 + px - py;
    	}
    	//ellipsePlotPoints (xCenter, yCenter, x, y);
		PaintBrush(BPoint(xCenter + x, yCenter + y),fore); 
 		PaintBrush(BPoint(xCenter - x, yCenter + y),fore); 
		PaintBrush(BPoint(xCenter + x, yCenter - y),fore); 
  		PaintBrush(BPoint(xCenter - x, yCenter - y),fore); 
  	}

  	// Region 2
  	p = ROUND (Ry2*(x+0.5)*(x+0.5) + Rx2*(y-1)*(y-1) - Rx2*Ry2);
  	while (y > 0) 
  	{
    	y--;
    	py -= twoRx2;
    	if (p > 0) 
      		p += Rx2 - py;
    	else 
    	{
      		x++;
      		px += twoRy2;
      		p += Rx2 - py + px;
    	}
    	//ellipsePlotPoints (xCenter, yCenter, x, y);
		PaintBrush(BPoint(xCenter + x, yCenter + y),fore); 
	 	PaintBrush(BPoint(xCenter - x, yCenter + y),fore); 
		PaintBrush(BPoint(xCenter + x, yCenter - y),fore); 
	  	PaintBrush(BPoint(xCenter - x, yCenter - y),fore); 
  	}
	save_each_paint=saved_undo_state;
}

void BPMImage::FloodFill(BPoint pt,rgb_color color)
{

	// zoom compensation done during BPMView::MouseUp()
	BPoint point(0,0);
	pointstack.Clear();
	
	SaveUndo(UNDO_TOOL,p_active_layer->bitmap->Bounds());

	// Fill color is set in MouseDown(). Set the color to be filled
	fillcolor=color;
	filledcolor=GetPixel(pt);

	// Just to make sure we're not trying to fill the fill color
	if ( 	( (fillcolor.red-filledcolor.red) !=0) &&
			( (fillcolor.green-filledcolor.green) !=0) &&
			( (fillcolor.blue-filledcolor.blue) !=0) )
	{
		// Call this to jump start the algorithm
		FillRow(pt);

		while(pointstack.Count())
		{	pointstack.Pop(&point);
			if(point.x == -1 && point.y == -1)
				break;
			if(p_active_layer->bitmap->Bounds().Contains(point))
				FillRow(point);
		}
	}
}

void BPMImage::FillRow(BPoint pt)
{
	upbound=true;
	downbound=true;
	
	// Fill the pixel
	FillPixel(pt);
	int32 index=1,
		x=(int32)pt.x,
		y=(int32)pt.y;
	
	// fill to the right of the pixel
	while( CheckPixel( BPoint(x+index,y)) )
		FillPixel( BPoint(x + index++, y));
	
	index=1;	// reset so we can fill to the left
	
	upbound=true;
	downbound=true;

	// fill to the left of the pixel
	while( CheckPixel( BPoint(x-index,y)) )
		FillPixel( BPoint(x - index++, y));

}

void inline BPMImage::FillPixel(BPoint pt)
{
	int32 	x=(int32) pt.x,
			y=(int32) pt.y;
	SetPixel(pt, fillcolor);

	// Check pixels above and below filled pixel
	if( CheckPixel(BPoint(x,y+1)) )
	{	if(upbound==true)
			pointstack.Push( BPoint(x,y+1) );
		upbound=false;
	}
	else
	{	upbound=true;
	}

	if( CheckPixel(BPoint(x,y-1)) )
	{
		if(downbound==true)
			pointstack.Push( BPoint(x,y-1) );
		downbound=false;
	}
	else
	{	downbound=true;
	}
}

bool BPMImage::CheckPixel(BPoint pt)
{
	// Check for in bounds
	if( (pt.x < 0) || (pt.y < 0) || (pt.x > p_active_layer->width)
				|| (pt.y > p_active_layer->height ) )
		return false;

	int16 dr,dg,db;
	// Get the color to be filled	
	rgb_color to_be_filled=GetPixel(pt);
	
	// See if we even need to fill this pixel
	dr=fillcolor.red-to_be_filled.red;
	dg=fillcolor.green-to_be_filled.green;
	db=fillcolor.blue-to_be_filled.blue;

	// Short circuit if same color
	if( (dr==0) && (dg==0) && (db==0))
		return false;

	// Is it selected?
//	if(shared->PickMaskColorAt(pt)==0)
//		return false;
	
	// Calculate differences
	dr=(int16)filledcolor.red-(int16)to_be_filled.red;
	dg=(int16)filledcolor.green-(int16)to_be_filled.green;
	db=(int16)filledcolor.blue-(int16)to_be_filled.blue;

	// Get absolute values for differences
	if(dr < 0)
		dr *= -1;
	if(dg < 0)
		dg *= -1;
	if(db < 0)
		db *= -1;
	
	// Check tolerance
	if( (dr>tolerance) || (dg>tolerance) || (db>tolerance))
		return false;

	// Everything checks out ok if we got this far...
	return true;
}

rgb_color BPMImage::GetPixel(BPoint pt)
{
//	pt.x = floor(pt.x / zoom);
//	pt.y = floor(pt.y / zoom);
	rgb_color color;

	uint32 pos_bits=uint32 ( (p_active_layer->bitmap->BytesPerRow()*pt.y) + (pt.x*4) );
	uint8	*s_bits=(uint8*) p_active_layer->bitmap->Bits() + pos_bits;
	
	color.blue=*s_bits;
	s_bits++;
	color.green=*s_bits;
	s_bits++;
	color.red=*s_bits;
	s_bits++;
	color.alpha=*s_bits;
	return color;
}

void BPMImage::SetPixel(BPoint where, rgb_color color)
{	
//	where.x = floor(where.x / zoom);
//	where.y = floor(where.y / zoom);

	// Make some quick-access pointers
	BBitmap *work_bmp;
	work_bmp=p_active_layer->bitmap;

	uint8 *bmpdata=(uint8 *)work_bmp->Bits();

	// Check bounds
	where.ConstrainTo(work_bmp->Bounds());
	
	// Calculate offset & jump
	uint32 bitoffset;
	bitoffset= uint32 ((where.x*4)+(work_bmp->BytesPerRow()*where.y)); //only counts there - it is similar on all lines
	bmpdata += bitoffset;
	
	// Assign color data
	*bmpdata=color.blue;
		bmpdata++;
	*bmpdata=color.green;
		bmpdata++;
	*bmpdata=color.red;
		bmpdata++;
	*bmpdata=color.alpha;  
}

void BPMImage::Stamp(BPoint from, BPoint to)
{
	from.ConstrainTo(display_bitmap->Bounds());
	to.ConstrainTo(display_bitmap->Bounds());

	int32 	tx,ty,fx,fy;	// used in rect calculations

	int16 	fromred,fromgreen,fromblue,tored,togreen,toblue,
			rdelta,gdelta,bdelta,adelta,
			trans,tempval=0,diffr,diffg,diffb;

	if(stampalpha==0)	// Nothing to do
		return;
		
	// data vars for brush and for bitmap
	uint8 *brush_data=(uint8 *)currentbrush->Bits(),
			*to_data=(uint8 *)p_active_layer->bitmap->Bits(),
			*work_data=(uint8 *)work_bitmap->Bits(),
			*brush_pos,*to_pos,*work_pos;

	uint32 	brush_rowsize=currentbrush->BytesPerRow(),
			img_rowsize=p_active_layer->bitmap->BytesPerRow(),
			work_rowsize=work_bitmap->BytesPerRow();
	uint32 rows,columns,bitheight,bitwidth,
			tlclip,trclip,ttclip,tbclip,	// integer vars for to clipping rectangle
			flclip,frclip,ftclip,fbclip;	// integer vars for from clipping rectangle
	BRect brushbounds,bmpbounds,workbounds,undorect,fromrect,torect;
	float tfrac;

	brushbounds=currentbrush->Bounds();
	bmpbounds=display_bitmap->Bounds();
	workbounds=work_bitmap->Bounds();
	
	// dimensions of bitmap for brush - NOT actual brush dimensions
	bitwidth=currentbrush->Bounds().IntegerWidth();
	bitheight=currentbrush->Bounds().IntegerHeight();

	// jump by offset so that destination's center is where we clicked
	tx=int32(to.x - (bitwidth>>1));
	ty=int32(to.y- (bitheight>>1));
	fx=int32(from.x - (bitwidth>>1));
	fy=int32(from.y- (bitheight>>1));
	
	// Get to/from frames in image
	torect.left=tx;
	torect.top=ty;
	torect.right=tx+bitwidth;
	torect.bottom=ty+bitheight;
	undorect=torect;

	fromrect.left=fx;
	fromrect.top=fy;
	fromrect.right=fx+bitwidth;
	fromrect.bottom=fy+bitheight;
	
	// Clip source rect to image
	ftclip=flclip=0;
	frclip=(int32)brushbounds.right;
	fbclip=(int32)brushbounds.bottom;

	if(fromrect.left < 0)
	{	flclip=int32(fromrect.left * -1);
		undorect.left=0;
	}
	if(fromrect.top < 0)
	{	ftclip=int32(fromrect.top * -1);
		undorect.top=0;
	}
	if(fromrect.right > bmpbounds.right)
	{	frclip -= int32(fromrect.right-bmpbounds.right);
		undorect.right=bmpbounds.right;
	}
	if(fromrect.bottom > bmpbounds.bottom)
	{	fbclip -= int32(fromrect.bottom-bmpbounds.bottom);
		undorect.bottom=bmpbounds.bottom;
	}

	// Clip dest rect to image
	ttclip=tlclip=0;
	trclip=(int32)brushbounds.right;
	tbclip=(int32)brushbounds.bottom;

	if(torect.left < 0)
	{	tlclip=int32(torect.left * -1);
		undorect.left=0;
	}
	if(torect.top < 0)
	{	ttclip=int32(torect.top * -1);
		undorect.top=0;
	}
	if(torect.right > bmpbounds.right)
	{	trclip -= int32(torect.right-bmpbounds.right);
		undorect.right=bmpbounds.right;
	}
	if(torect.bottom > bmpbounds.bottom)
	{	tbclip -= int32(torect.bottom-bmpbounds.bottom);
		undorect.bottom=bmpbounds.bottom;
	}

	// Jump to area in question in image
	to_data += int32((ty*img_rowsize) + (tx *4));
	work_data += int32((fy*img_rowsize) + (fx *4));

	for(rows=ftclip;rows<=fbclip;rows++)
	{	
		to_pos=(uint8*)(to_data+(img_rowsize * rows)+(flclip*4));
		brush_pos=(uint8*)(brush_data+(brush_rowsize * rows)+(flclip*4));
		work_pos=(uint8*)(work_data+(work_rowsize * rows)+(flclip*4));
		
		for(columns=flclip;columns<=frclip;columns++)
		{
			trans=int16( (255-brush_pos[1])*(float(stampalpha)/255.0));
			tfrac=float(trans)/255;

			// Stamp tool *always* uses the work bitmap to prevent read/write conflicts
			fromred=work_pos[2];
			fromgreen=work_pos[1];
			fromblue=work_pos[0];
			tored=to_pos[2];
			togreen=to_pos[1];
			toblue=to_pos[0];
			
			switch(blendmode)
			{
				case TMULTIPLY:
				{
					diffb=int16(float(fromblue*toblue)/255.0);
					diffg=int16(float(fromgreen*togreen)/255.0);
					diffr=int16(float(fromred*tored)/255.0);
					break;
				}
				case TDIVIDE:
				{	if(fromred==0) fromred=1;
					if(fromgreen==0) fromgreen=1;
					if(fromblue==0) fromblue=1;
					diffb=int16( float(toblue*255)/float(fromblue) );
					diffg=int16( float(togreen*255)/float(fromgreen) );
					diffr=int16( float(tored*255)/float(fromred) );
					if(diffb>255) diffb=255;
					if(diffg>255) diffg=255;
					if(diffr>255) diffr=255;
					break;
				}
				case TDIFFERENCE:
				{
					diffb=fromblue-toblue;
					diffg=fromgreen-togreen;
					diffr=fromred-tored;
					break;
				}
				case TEXCLUSION:
				{	
					diffb=toblue ^ fromblue;
					diffg=togreen ^ fromgreen;
					diffr=tored ^ fromred;
					break;
				}
				case TLIGHTEN:
				{	
					diffb=MAX(toblue,fromblue);
					diffg=MAX(togreen,fromgreen);
					diffr=MAX(tored,fromred);
					break;
				}
				case TDARKEN:
				{	
					diffb=MIN(toblue,fromblue);
					diffg=MIN(togreen,fromgreen);
					diffr=MIN(tored,fromred);
					break;
				}
				case TOR:
				{	
					diffb=fromblue|toblue;
					diffg=fromgreen|togreen;
					diffr=fromred|tored;
					break;
				}
				case TAVERAGE:
				{	
					diffb=(fromblue+toblue)>>1;
					diffg=(fromgreen+togreen)>>1;
					diffr=(fromred+tored)>>1;
					break;
				}
				case TADDITION:
				{	
					diffb=toblue+fromblue;
					diffg=togreen+fromgreen;
					diffr=tored+fromred;
					if(diffb>255) diffb=255;
					if(diffg>255) diffg=255;
					if(diffr>255) diffr=255;
					break;
				}
				case TSUBTRACT:
				{	
					diffb=toblue-fromblue;
					diffg=togreen-fromgreen;
					diffr=tored-fromred;
					if(diffb<0) diffb=0;
					if(diffg<0) diffg=0;
					if(diffr<0) diffr=0;
					break;
				}
				default:
				{
					diffb=fromblue;
					diffg=fromgreen;
					diffr=fromred;
				}
			}

			// Normal painting mode
			if(erasemode==false)
			{
				if(diffb<0)	diffb *= -1;
				bdelta=diffb-toblue;
				tempval=toblue + int16(float(bdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					to_pos[0] = tempval;
	
				if(diffg<0)	diffg *= -1;
				gdelta=diffg-togreen;
				tempval=togreen + int16(float(gdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					to_pos[1] = tempval;
	
				if(diffr<0)	diffr *= -1;
				rdelta=diffr-tored;
				tempval=tored + int16(float(rdelta) * tfrac);
				if(tempval>255)
					tempval&=255;
				if(trans>0)
					to_pos[2] = tempval;
	
				adelta=to_pos[3]-stampalpha;
				tempval=to_pos[3] + int16(float(adelta) * tfrac);

				if(tempval<0)	tempval *= -1;
				if(tempval>255)
					tempval&=255;
				to_pos[3]=tempval;
			}
			else
			{	// erasing image
				if(maskmode==false)
				{	if(tfrac > 0.0)
					{	tempval=work_pos[3] - int16(float(work_pos[3]) * tfrac);
						if(tempval<0)	tempval=0;
						to_pos[3]=tempval;
					}
				}
				else	// erasing mask == increasing selected area
				{
				}
			}

			to_pos+=4;
			brush_pos+=4;
			work_pos+=4;
		}	// end for columns
	}	// end for rows
}