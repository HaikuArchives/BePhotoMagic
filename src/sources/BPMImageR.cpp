#include "BPMImage.h"

void BPMImage::UpdateDisplayImage(BRect update_rect)
{
	uint32 i;
	uint32 left,right,top,bottom;	// sides of update rectangle
	uint32 rows,columns,rowlength,rowoffset,copybytes,columnoffset;
	uint8 *layerdata,*layerpos, *displaydata, *displaypos;
//	int16 trans,diffb,diffg,diffr,rdelta,gdelta,bdelta,adelta,tempval;
	int16 diffb,diffg,diffr,rdelta,gdelta,bdelta,adelta,tempval;
	uint8 blendmode,red,green,blue;
	float tfrac,layerfrac;

	displaydata=(uint8 *)display_bitmap->Bits();

	// Initialize for area to be redrawn
	left=(uint32)update_rect.left;
	top=(uint32)update_rect.top;
	right=(uint32)update_rect.right;
	bottom=(uint32)update_rect.bottom;

	if(right>(width-1)) right=width-1;
	if(bottom>(height-1)) bottom=height-1;

	copybytes=(right-left+1)<<2;
	columnoffset=left<<2;

	// Copy bottom layer to display bitmap
	rowlength=layers[0]->bitmap->BytesPerRow();
	layerdata=(uint8 *)layers[0]->bitmap->Bits();
	
	for(rows=top;rows<=bottom;rows++)
	{	
		rowoffset=rows * rowlength;
		layerpos=layerdata + rowoffset;
		displaypos=displaydata + rowoffset;
		
		memcpy((void*)(displaypos+columnoffset),(void*)(layerpos+columnoffset),copybytes);
	}

	for(i=1;i<number_layers;i++)
	{
		if(layers[i]->visible==true)
		{
			rowlength=layers[i]->bitmap->BytesPerRow();
			layerdata=(uint8 *)layers[i]->bitmap->Bits();
			blendmode=layers[i]->blendmode;
			layerfrac=float(layers[i]->opacity)/255.0;

			// for now layers have a fixed size, a la Photoshop
			for(rows=top;rows<=bottom;rows++)
			{
				rowoffset=rows * rowlength;
				layerpos=layerdata + rowoffset;
				displaypos=displaydata + rowoffset;
				
				for(columns=columnoffset;columns<columnoffset+copybytes;columns+=4)
				{
					if(layerpos[3] != 0)
					{
//						trans=int16( (255-layerpos[1])*(float(layerpos[3])/255.0));
//						tfrac=float(trans)/255;
						tfrac=layerfrac * (float(layerpos[3])/255.0);

						red=layerpos[2];
						green=layerpos[1];
						blue=layerpos[0];

						switch(blendmode)
						{
							case TMULTIPLY:
							{
								diffb=int16(float(blue*displaypos[0])/255.0);
								diffg=int16(float(green*displaypos[1])/255.0);
								diffr=int16(float(red*displaypos[2])/255.0);
								break;
							}
							case TDIVIDE:
							{	if(red==0) red=1;
								if(green==0) green=1;
								if(blue==0) blue=1;
								diffr=int16( float(displaypos[2]*255)/float(red) );
								diffg=int16( float(displaypos[1]*255)/float(green) );
								diffb=int16( float(displaypos[0]*255)/float(blue) );
								if(diffr>255) diffr=255;
								if(diffg>255) diffg=255;
								if(diffb>255) diffb=255;
								break;
							}
							case TDIFFERENCE:
							{
								diffb=blue-displaypos[0];
								diffg=green-displaypos[1];
								diffr=red-displaypos[2];
								break;
							}
							case TEXCLUSION:
							{	
								diffb=displaypos[0] ^ blue;
								diffg=displaypos[1] ^ green;
								diffr=displaypos[2] ^ red;
								break;
							}
							case TLIGHTEN:
							{	
								diffb=MAX(displaypos[0],blue);
								diffg=MAX(displaypos[1],green);
								diffr=MAX(displaypos[2],red);
								break;
							}
							case TDARKEN:
							{	
								diffb=MIN(displaypos[0],blue);
								diffg=MIN(displaypos[1],green);
								diffr=MIN(displaypos[2],red);
								break;
							}
							case TOR:
							{	
								diffb=blue|displaypos[0];
								diffg=green|displaypos[1];
								diffr=red|displaypos[2];
								break;
							}
							case TAVERAGE:
							{	
								diffb=(blue+displaypos[0])>>1;
								diffg=(green+displaypos[1])>>1;
								diffr=(red+displaypos[2])>>1;
								break;
							}
							case TADDITION:
							{	
								diffb=displaypos[0]+blue;
								diffg=displaypos[1]+green;
								diffr=displaypos[2]+red;
								if(diffb>255) diffb=255;
								if(diffg>255) diffg=255;
								if(diffr>255) diffr=255;
								break;
							}
							case TSUBTRACT:
							{	
								diffb=displaypos[0]-blue;
								diffg=displaypos[1]-green;
								diffr=displaypos[2]-red;
								if(diffb<0) diffb=0;
								if(diffg<0) diffg=0;
								if(diffr<0) diffr=0;
								break;
							}
							default:
							{
								diffb=blue;
								diffg=green;
								diffr=red;
							}
						}
						if(diffb<0)	diffb *= -1;
						bdelta=diffb-displaypos[0];
						tempval=displaypos[0] + int16(float(bdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[0] = tempval;

						if(diffg<0)	diffg *= -1;
						gdelta=diffg-displaypos[1];
						tempval=displaypos[1] + int16(float(gdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[1] = tempval;

						if(diffr<0)	diffr *= -1;
						rdelta=diffr-displaypos[2];
						tempval=displaypos[2] + int16(float(rdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[2] = tempval;

/*						adelta=displaypos[3]-layerpos[3];
						tempval=displaypos[3] + int16(float(adelta) * tfrac);
						if(tempval<0)	tempval *= -1;
						if(tempval>255)
							tempval&=255;
						displaypos[3]=tempval;
*/
					}	// end if alpha != 0
					layerpos +=4;
					displaypos +=4;
				}	// end for columns
			}	// end for rows
		}	// end if visible
	}	// end for number_layers

}

/*
void BPMImage::UpdateDisplayImage(BRect update_rect)
{
	MergeBelow(update_rect);
	MergeAbove(update_rect);
	BView dview(display_bitmap->Bounds(),"dview",B_FOLLOW_NONE,B_WILL_DRAW);

	display_bitmap->Lock();
	display_bitmap->AddChild(&dview);
//	dview.SetHighColor(255,255,255);
	dview.FillRect(update_rect);
	dview.DrawBitmap(bottomlayer,update_rect,update_rect);
	dview.DrawBitmap(p_active_layer->bitmap,update_rect,update_rect);
	dview.DrawBitmap(toplayer,update_rect,update_rect);
	display_bitmap->RemoveChild(&dview);
	display_bitmap->Unlock();
}
*/




void BPMImage::MergeBelow(BRect update_rect)
{	// This function merges all layers into the *bottomlayer bitmap

	if(bottomlayer!=NULL)
		delete bottomlayer;
	bottomlayer=new BBitmap(layers[0]->bitmap);
	
	if(active_layer==0)
		return;

	uint32 i;
	uint32 left,right,top,bottom;
	uint32 rows,columns,rowlength,rowoffset,copybytes,columnoffset;
	uint8 *layerdata,*layerpos, *displaydata, *displaypos;
	int16 diffb,diffg,diffr,rdelta,gdelta,bdelta,adelta,tempval;
	uint8 blendmode,red,green,blue;
	float tfrac,layerfrac;

	displaydata=(uint8 *)bottomlayer->Bits();

	// Initialize for area to be redrawn
	left=(uint32)update_rect.left;
	top=(uint32)update_rect.top;
	right=(uint32)update_rect.right;
	bottom=(uint32)update_rect.bottom;

	if(right>(width-1)) right=width-1;
	if(bottom>(height-1)) bottom=height-1;

	copybytes=(right-left+1)<<2;
	columnoffset=left<<2;

	// Copy bottom layer to display bitmap
	rowlength=layers[0]->bitmap->BytesPerRow();
	layerdata=(uint8 *)layers[0]->bitmap->Bits();
	
	for(i=1;i<number_layers;i++)
	{
		if(layers[i]->visible==true)
		{
			rowlength=layers[i]->bitmap->BytesPerRow();
			layerdata=(uint8 *)layers[i]->bitmap->Bits();
			blendmode=layers[i]->blendmode;
			layerfrac=float(layers[i]->opacity)/255.0;

			// for now layers have a fixed size, a la Photoshop
			for(rows=top;rows<=bottom;rows++)
			{
				rowoffset=rows * rowlength;
				layerpos=layerdata + rowoffset;
				displaypos=displaydata + rowoffset;
				
				for(columns=columnoffset;columns<columnoffset+copybytes;columns+=4)
				{
					if(layerpos[3] != 0)
					{
						tfrac=layerfrac * (float(layerpos[3])/255.0);

						red=layerpos[2];
						green=layerpos[1];
						blue=layerpos[0];

						switch(blendmode)
						{
							case TMULTIPLY:
							{
								diffb=int16(float(blue*displaypos[0])/255.0);
								diffg=int16(float(green*displaypos[1])/255.0);
								diffr=int16(float(red*displaypos[2])/255.0);
								break;
							}
							case TDIVIDE:
							{	if(red==0) red=1;
								if(green==0) green=1;
								if(blue==0) blue=1;
								diffb=int16( float(displaypos[0]*255)/float(blue) );
								diffg=int16( float(displaypos[1]*255)/float(green) );
								diffr=int16( float(displaypos[2]*255)/float(red) );
								if(diffb>255) diffb=255;
								if(diffg>255) diffg=255;
								if(diffr>255) diffr=255;
								break;
							}
							case TDIFFERENCE:
							{
								diffb=blue-displaypos[0];
								diffg=green-displaypos[1];
								diffr=red-displaypos[2];
								break;
							}
							case TEXCLUSION:
							{	
								diffb=displaypos[0] ^ blue;
								diffg=displaypos[1] ^ green;
								diffr=displaypos[2] ^ red;
								break;
							}
							case TLIGHTEN:
							{	
								diffb=MAX(displaypos[0],blue);
								diffg=MAX(displaypos[1],green);
								diffr=MAX(displaypos[2],red);
								break;
							}
							case TDARKEN:
							{	
								diffb=MIN(displaypos[0],blue);
								diffg=MIN(displaypos[1],green);
								diffr=MIN(displaypos[2],red);
								break;
							}
							case TOR:
							{	
								diffb=blue|displaypos[0];
								diffg=green|displaypos[1];
								diffr=red|displaypos[2];
								break;
							}
							case TAVERAGE:
							{	
								diffb=(blue+displaypos[0])>>1;
								diffg=(green+displaypos[1])>>1;
								diffr=(red+displaypos[2])>>1;
								break;
							}
							case TADDITION:
							{	
								diffb=displaypos[0]+blue;
								diffg=displaypos[1]+green;
								diffr=displaypos[2]+red;
								if(diffb>255) diffb=255;
								if(diffg>255) diffg=255;
								if(diffr>255) diffr=255;
								break;
							}
							case TSUBTRACT:
							{	
								diffb=displaypos[0]-blue;
								diffg=displaypos[1]-green;
								diffr=displaypos[2]-red;
								if(diffb<0) diffb=0;
								if(diffg<0) diffg=0;
								if(diffr<0) diffr=0;
								break;
							}
							default:
							{
								diffb=blue;
								diffg=green;
								diffr=red;
							}
						}
						if(diffb<0)	diffb *= -1;
						bdelta=diffb-displaypos[0];
						tempval=displaypos[0] + int16(float(bdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[0] = tempval;

						if(diffg<0)	diffg *= -1;
						gdelta=diffg-displaypos[1];
						tempval=displaypos[1] + int16(float(gdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[1] = tempval;

						if(diffr<0)	diffr *= -1;
						rdelta=diffr-displaypos[2];
						tempval=displaypos[2] + int16(float(rdelta) * tfrac);
						if(tempval>255)
							tempval&=255;
						displaypos[2] = tempval;

						adelta=displaypos[3]-layerpos[3];
						tempval=displaypos[3] + int16(float(adelta) * tfrac);
						if(tempval<0)	tempval *= -1;
						if(tempval>255)
							tempval&=255;
						displaypos[3]=tempval;

					}	// end if alpha != 0
					layerpos +=4;
					displaypos +=4;
				}	// end for columns
			}	// end for rows
		}	// end if visible
	}	// end for number_layers

}





void BPMImage::MergeAbove(BRect update_rect)
{
	if(toplayer!=NULL)
		delete bottomlayer;
	toplayer=new BBitmap(layers[number_layers-1]->bitmap);
	
	if(active_layer==number_layers-1)
		return;

	uint32 i;
	uint32 left,right,top,bottom;	// sides of update rectangle
	uint32 rows,columns,rowlength,rowoffset,copybytes,columnoffset;
	uint8 *layerdata,*layerpos, *displaydata, *displaypos;
//	int16 trans,diffb,diffg,diffr,rdelta,gdelta,bdelta,adelta,tempval;
	int16 diffb,diffg,diffr,rdelta,gdelta,bdelta,adelta,tempval;
	uint8 blendmode,red,green,blue;
	float tfrac,layerfrac;

	displaydata=(uint8 *)toplayer->Bits();

	// Initialize for area to be redrawn
	left=(uint32)update_rect.left;
	top=(uint32)update_rect.top;
	right=(uint32)update_rect.right;
	bottom=(uint32)update_rect.bottom;

	if(right>(width-1)) right=width-1;
	if(bottom>(height-1)) bottom=height-1;

	copybytes=(right-left+1)<<2;
	columnoffset=left<<2;

	// Copy bottom layer to display bitmap
	rowlength=p_active_layer->bitmap->BytesPerRow();
	layerdata=(uint8 *)p_active_layer->bitmap->Bits();
	
	for(i=active_layer;i<number_layers;i++)
	{
		if(layers[i]->visible==true)
		{
			rowlength=layers[i]->bitmap->BytesPerRow();
			layerdata=(uint8 *)layers[i]->bitmap->Bits();
			blendmode=layers[i]->blendmode;
			layerfrac=float(layers[i]->opacity)/255.0;

			// for now layers have a fixed size, a la Photoshop
			for(rows=top;rows<=bottom;rows++)
			{
				rowoffset=rows * rowlength;
				layerpos=layerdata + rowoffset;
				displaypos=displaydata + rowoffset;
				
				for(columns=columnoffset;columns<columnoffset+copybytes;columns+=4)
				{
					if(layerpos[3] != 0)
					{
//						trans=int16( (255-layerpos[1])*(float(layerpos[3])/255.0));
//						tfrac=float(trans)/255;
						tfrac=layerfrac * (float(layerpos[3])/255.0);

						red=layerpos[2];
						green=layerpos[1];
						blue=layerpos[0];

						switch(blendmode)
						{
							case TMULTIPLY:
							{
								diffb=int16(float(blue*displaypos[0])/255.0);
								diffg=int16(float(green*displaypos[1])/255.0);
								diffr=int16(float(red*displaypos[2])/255.0);
								break;
							}
							case TDIVIDE:
							{	if(red==0) red=1;
								if(green==0) green=1;
								if(blue==0) blue=1;
								diffb=int16( float(displaypos[0]*255)/float(blue) );
								diffg=int16( float(displaypos[1]*255)/float(green) );
								diffr=int16( float(displaypos[2]*255)/float(red) );
								if(diffb>255) diffb=255;
								if(diffg>255) diffg=255;
								if(diffr>255) diffr=255;
								break;
							}
							case TDIFFERENCE:
							{
								diffb=blue-displaypos[0];
								diffg=green-displaypos[1];
								diffr=red-displaypos[2];
								break;
							}
							case TEXCLUSION:
							{	
								diffb=displaypos[0] ^ blue;
								diffg=displaypos[1] ^ green;
								diffr=displaypos[2] ^ red;
								break;
							}
							case TLIGHTEN:
							{	
								diffb=MAX(displaypos[0],blue);
								diffg=MAX(displaypos[1],green);
								diffr=MAX(displaypos[2],red);
								break;
							}
							case TDARKEN:
							{	
								diffb=MIN(displaypos[0],blue);
								diffg=MIN(displaypos[1],green);
								diffr=MIN(displaypos[2],red);
								break;
							}
							case TOR:
							{	
								diffb=blue|displaypos[0];
								diffg=green|displaypos[1];
								diffr=red|displaypos[2];
								break;
							}
							case TAVERAGE:
							{	
								diffb=(blue+displaypos[0])>>1;
								diffg=(green+displaypos[1])>>1;
								diffr=(red+displaypos[2])>>1;
								break;
							}
							case TADDITION:
							{	
								diffb=displaypos[0]+blue;
								diffg=displaypos[1]+green;
								diffr=displaypos[2]+red;
								if(diffb>255) diffb=255;
								if(diffg>255) diffg=255;
								if(diffr>255) diffr=255;
								break;
							}
							case TSUBTRACT:
							{	
								diffb=displaypos[0]-blue;
								diffg=displaypos[1]-green;
								diffr=displaypos[2]-red;
								if(diffb<0) diffb=0;
								if(diffg<0) diffg=0;
								if(diffr<0) diffr=0;
								break;
							}
							default:
							{
								diffb=blue;
								diffg=green;
								diffr=red;
							}
						}
						if(diffb<0)	diffb *= -1;
						bdelta=diffb-displaypos[0];
						tempval=displaypos[0] + int16(float(bdelta) * tfrac);
						if(tempval>255)
//							tempval&=255;
							tempval=255;
						if(tempval<0)
							tempval=0;
						displaypos[0] = tempval;

						if(diffg<0)	diffg *= -1;
						gdelta=diffg-displaypos[1];
						tempval=displaypos[1] + int16(float(gdelta) * tfrac);
						if(tempval>255)
//							tempval&=255;
							tempval=255;
						if(tempval<0)
							tempval=0;
						displaypos[1] = tempval;

						if(diffr<0)	diffr *= -1;
						rdelta=diffr-displaypos[2];
						tempval=displaypos[2] + int16(float(rdelta) * tfrac);
						if(tempval>255)
//							tempval&=255;
							tempval=255;
						if(tempval<0)
							tempval=0;
						displaypos[2] = tempval;

//						adelta=displaypos[3]-layerpos[3];
						adelta=int16(displaypos[3]+(displaypos[3]*tfrac));
						tempval=displaypos[3] + int16(float(adelta) * tfrac);
//						if(tempval<0)
//							tempval *= -1;
						if(tempval>255)
//							tempval&=255;
							tempval=255;
						displaypos[3]=tempval;

					}	// end if alpha != 0
					layerpos +=4;
					displaypos +=4;
				}	// end for columns
			}	// end for rows
		}	// end if visible
	}	// end for number_layers

}