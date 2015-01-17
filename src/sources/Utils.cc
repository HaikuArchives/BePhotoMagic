#include "Utils.h"
#include "TransModes.h"

void AppDir(BPath *path)
{
	app_info ai; 
	BAppFileInfo afi; 
	BFile file; 

	app_info info;
	be_app->GetAppInfo(&info);
	BEntry tmp_entry(&info.ref);
	BEntry file_entry;
	tmp_entry.GetParent(&file_entry);
	file_entry.GetPath(path); 
}

BBitmap * LoadBitmap(char *filename)
{
	BFile file;
/*	if (file.SetTo(filename, B_READ_ONLY) == B_OK)
	{
		BTranslatorRoster *roster = BTranslatorRoster::Default();
		ProgressiveBitmapStream stream(NULL, NULL, false,false);
		
		roster->Translate(&file, 0, 0, &stream, B_TRANSLATOR_BITMAP);
		stream.SetDispose(false);

		return stream.Bitmap();
	}
*/
	if (file.SetTo(filename, B_READ_ONLY) == B_OK)
	{
		BNodeInfo nodeinfo(&file);
		BBitmap *outbmp;
		BTranslatorRoster *roster = BTranslatorRoster::Default();
		BBitmapStream stream;
		BMessage msg;
		char filetype[255];
		nodeinfo.GetType(filetype);

		roster->Translate(&file, 0, 0, &stream, B_TRANSLATOR_BITMAP,0,filetype);
		stream.DetachBitmap(&outbmp);
		return outbmp;
	}
	else
		return NULL;
}

bool IsBPMFile(const char *path)
{	
	BFile file;
	char name[17];	// more than enough
	
	uint32 stat=file.SetTo(path,B_READ_ONLY);
	if(stat != B_OK)
		return false;

	// get header data
	file.Seek(0,SEEK_SET);
	file.Read((void *)&name,17);
	//printf("BPM file Signature: %s|\n",name);
	file.Unset();

	int stringstat=strncmp(name,"BePhotoMagic 1.00",16);
	if(stringstat==0)
	{	//printf("Is BPM file\n");
		return true;
	}
	//printf("Is _not_ BPM file: status %d\n",stringstat);
	return false;	
}

BRect CenterChildRect(BRect parent, BRect child, bool constrain=true)
{
	float wdiff,hdiff;

	BRect workrect=child;
	
	// if the rect goes offscreen, constrain to screen
	if(!(parent.Contains(workrect)) && constrain==true)
	{	workrect.LeftTop().ConstrainTo(parent);
		workrect.RightBottom().ConstrainTo(parent);
	}
	
	// Center child if bigger than parent
	if( (workrect.Width() > parent.Width()) || 
		(workrect.Height() > parent.Height()) )
	{	wdiff=parent.Width()-workrect.Width();
		hdiff=parent.Height()-workrect.Height();
		workrect.OffsetTo( (parent.Width()-workrect.Width())/2,
				(parent.Height()-workrect.Height())/2 );
	}
	return workrect;
}

BRect CenterToScreen(BRect child)
{	BScreen screen;
	BRect screenrect=screen.Frame();
	return CenterChildRect(screenrect,child);
}

void UninvertRect(BPoint *oldpt, BPoint *newpt)
{	// This takes two points and makes sure that the first point
	// is in the upper-left corner and the second in the bottom right
	// This is necessary for certain drawing functions, such as StrokeEllipse()
	BPoint temp1,temp2;
	
	if(newpt->x < oldpt->x)
	{	temp1.x=newpt->x;
		temp2.x=oldpt->x;
	}
	else
	{	temp1.x=oldpt->x;
		temp2.x=newpt->x;
	}
	
	if(newpt->y < oldpt->y)
	{	temp1.y=newpt->y;
		temp2.y=oldpt->y;
	}
	else
	{	temp1.y=oldpt->y;
		temp2.y=newpt->y;
	}
	oldpt->x=temp1.x;
	oldpt->y=temp1.y;
	newpt->x=temp2.x;
	newpt->y=temp2.y;
}

BRect ConstrainToScreen(BRect rect)
{
	BScreen screen;
	BRect screenrect=screen.Frame();

	if(!(screenrect.Contains(rect)))
	{
		// The rectangle somehow doesn't fit on the screen.
		rect.LeftTop().ConstrainTo(screenrect);
		rect.RightBottom().ConstrainTo(screenrect);
	}
	return rect;
}

void ApplyBitmap( BBitmap *sourcebmp, BRect sourcerect,
					  BBitmap *destbmp,BRect destrect,uint8 blendmode)
{
printf("ApplyBitmap()\n");
	int16 	red,green,blue,alpha,
			rdelta,gdelta,bdelta,adelta,
			trans,tempval,diffr,diffg,diffb;

	// data vars for brush and for bitmap
	uint8 *src_data=(uint8 *)sourcebmp->Bits(),
			*dest_data=(uint8 *)destbmp->Bits(),
			*src_pos,*dest_pos;
	int8 	*src_pos_signed,*dest_pos_signed;
	uint32 	src_rowsize=sourcebmp->BytesPerRow(),
			dest_rowsize=destbmp->BytesPerRow();

	uint32 rows,columns,
			lclip,rclip,tclip,bclip;	// integer vars for clipping rectangle

	float tfrac;

	// Clip source and dest rectangles to respective bitmaps
	if(!(sourcebmp->Bounds().Contains(sourcerect)))
	{
		sourcerect.LeftTop().ConstrainTo(sourcebmp->Bounds());
		sourcerect.RightBottom().ConstrainTo(sourcebmp->Bounds());
	}
	if(!(destbmp->Bounds().Contains(destrect)))
	{
		destrect.LeftTop().ConstrainTo(destbmp->Bounds());
		destrect.RightBottom().ConstrainTo(destbmp->Bounds());
	}
	
	// Clip source to dest rectangle
	if(!(destbmp->Bounds().Contains(sourcerect)))
	{
		sourcerect.LeftTop().ConstrainTo(destbmp->Bounds());
		sourcerect.RightBottom().ConstrainTo(destbmp->Bounds());
	}

	// Assign source bitmap clipping rectangle proper values
	lclip=(int32)sourcerect.left;
	tclip=(int32)sourcerect.top;
	rclip=(int32)sourcerect.right;
	bclip=(int32)sourcerect.bottom;

	// Jump to area in question in image
	src_data += int32((sourcerect.top*src_rowsize) + (sourcerect.left *4));
	dest_data += int32((destrect.top*dest_rowsize) + (destrect.left *4));

	for(rows=tclip;rows<=bclip;rows++)
	{	
		dest_pos=(uint8*)(dest_data+(dest_rowsize * rows)+(lclip*4));
		src_pos=(uint8*)(src_data+(src_rowsize * rows)+(lclip*4));
		dest_pos_signed=(int8 *)dest_pos;
		src_pos_signed=(int8 *)src_pos;
				
		for(columns=lclip;columns<=rclip;columns++)
		{
			red=src_pos[2];
			green=src_pos[1];
			blue=src_pos[0];
			alpha=src_pos[3];
			
			// Skip if completely transparent
			if(alpha==0)
			{
				dest_pos+=4;
				src_pos+=4;
				continue;
			}

//if(red!=0 && green!=0 && blue!=0)
//	printf("source: (%d,%d,%d,%d)\n",red,green,blue,alpha);			
//if(dest_pos[2]!=0 && dest_pos[1]!=0 && dest_pos[0]!=0 && dest_pos[3]!=0)
//	printf("dest: (%d,%d,%d,%d)\n",dest_pos[2],dest_pos[1],dest_pos[0],dest_pos[3]);

//			trans=int16( (255-src_pos[1])*(float(alpha)/255.0));
//			tfrac=float(trans)/255;
			trans=alpha;
			tfrac=float(trans)/255.0;
//if(tfrac>0.0)
//	printf("transparency fraction: %f\n",tfrac);
			switch(blendmode)
			{
				case TMULTIPLY:
				{
					diffr=int16(float(red*dest_pos[2])/255.0);
					diffg=int16(float(green*dest_pos[1])/255.0);
					diffb=int16(float(blue*dest_pos[0])/255.0);
					break;
				}
				case TDIVIDE:
				{	if(red==0) red=1;
					if(green==0) green=1;
					if(blue==0) blue=1;
					diffr=int16( float(dest_pos[2]*255)/float(red) );
					diffg=int16( float(dest_pos[1]*255)/float(green) );
					diffb=int16( float(dest_pos[0]*255)/float(blue) );
					if(diffr>255) diffr=255;
					if(diffg>255) diffg=255;
					if(diffb>255) diffb=255;
					break;
				}
				case TDIFFERENCE:
				{
					diffr=red-dest_pos[2];
					diffg=green-dest_pos[1];
					diffb=blue-dest_pos[0];
					break;
				}
				case TEXCLUSION:
				{	
					diffr=dest_pos[2] ^ red;
					diffg=dest_pos[1] ^ green;
					diffb=dest_pos[0] ^ blue;
					break;
				}
				case TLIGHTEN:
				{	
					diffr=MAX(dest_pos[2],red);
					diffg=MAX(dest_pos[1],green);
					diffb=MAX(dest_pos[0],blue);
					break;
				}
				case TDARKEN:
				{	
					diffr=MIN(dest_pos[2],red);
					diffg=MIN(dest_pos[1],green);
					diffb=MIN(dest_pos[0],blue);
					break;
				}
				case TOR:
				{	
					diffr=red|dest_pos[2];
					diffg=green|dest_pos[1];
					diffb=blue|dest_pos[0];
					break;
				}
				case TADDITION:
				{	
					diffr=dest_pos[2]+red;
					diffg=dest_pos[1]+green;
					diffb=dest_pos[0]+blue;
					if(diffr>255) diffr=255;
					if(diffg>255) diffg=255;
					if(diffb>255) diffb=255;
					break;
				}
				case TSUBTRACT:
				{	
					diffr=dest_pos[2]-red;
					diffg=dest_pos[1]-green;
					diffb=dest_pos[0]-blue;
					if(diffr<0) diffr=0;
					if(diffg<0) diffg=0;
					if(diffb<0) diffb=0;
					break;
				}
				default:
				{
					diffb=blue;
					diffg=green;
					diffr=red;
				}
			}
//if(diffr!=0 || diffg!=0 || diffb!=0)
//	printf("deltas: (%d,%d,%d)\n",diffr,diffg,diffb);
			if(diffb<0)	diffb *= -1;
//			bdelta=diffb-dest_pos[0];
			bdelta=diffb-dest_pos_signed[0];
			tempval=dest_pos[0] + int16(float(bdelta) * tfrac);
			if(tempval>255)
//				tempval&=255;
				tempval=255;
			dest_pos[0] = tempval;

			if(diffg<0)	diffg *= -1;
//			gdelta=diffg-dest_pos[1];
			gdelta=diffg-dest_pos_signed[1];
			tempval=dest_pos[1] + int16(float(gdelta) * tfrac);
			if(tempval>255)
//				tempval&=255;
				tempval=255;
			dest_pos[1] = tempval;

			if(diffr<0)	diffr *= -1;
//			rdelta=diffr-dest_pos[2];
			rdelta=diffr-dest_pos_signed[2];
			tempval=dest_pos[2] + int16(float(rdelta) * tfrac);
			if(tempval>255)
//				tempval&=255;
				tempval=255;
			dest_pos[2] = tempval;

//			adelta=dest_pos[3]-alpha;
//			adelta=dest_pos_signed[3]-alpha;
			adelta=dest_pos[3]+int16(float(dest_pos[3])*tfrac);
			tempval=dest_pos[3] + int16(float(adelta) * tfrac);
//			if(tempval<0)	tempval *= -1;
			if(tempval>255)
//				tempval&=255;
				tempval=255;
			dest_pos[3]=tempval;

			dest_pos+=4;
			src_pos+=4;
		}	// end for columns
	}	// end for rows
}

void CopyRect(BRect sourcerect, BBitmap *sourcebmp, 
					 BRect destrect, BBitmap *destbmp, uint8 colorspace_size)
{
	if(sourcebmp==NULL || destbmp==NULL)
		return;

	colorspace_size=4;
	// First, clip source rect to destination
	if(sourcerect.Width() > destrect.Width())
	{	sourcerect.right=sourcerect.left+destrect.Width();
	}

	if(sourcerect.Height() > destrect.Height())
	{	sourcerect.bottom=sourcerect.top+destrect.Height();
	}

	// Second, check rectangle bounds against their own bitmaps
	BRect work_rect;

	work_rect.Set(	sourcebmp->Bounds().left,
					sourcebmp->Bounds().top,
					sourcebmp->Bounds().right,
					sourcebmp->Bounds().bottom	);
	if( !(work_rect.Contains(sourcerect)) )
	{	// something in selection must be clipped
		if(sourcerect.left < 0)
			sourcerect.left = 0;
		if(sourcerect.right > work_rect.right)
			sourcerect.right = work_rect.right;
		if(sourcerect.top < 0)
			sourcerect.top = 0;
		if(sourcerect.bottom > work_rect.bottom)
			sourcerect.bottom = work_rect.bottom;
	}

	work_rect.Set(	destbmp->Bounds().left,
					destbmp->Bounds().top,
					destbmp->Bounds().right,
					destbmp->Bounds().bottom	);
	if( !(work_rect.Contains(destrect)) )
	{	// something in selection must be clipped
		if(destrect.left < 0)
			destrect.left = 0;
		if(destrect.right > work_rect.right)
			destrect.right = work_rect.right;
		if(destrect.top < 0)
			destrect.top = 0;
		if(destrect.bottom > work_rect.bottom)
			destrect.bottom = work_rect.bottom;
	}

	// Set pointers to the actual data
	uint8 *src_bits  = (uint8*) sourcebmp->Bits();	
	uint8 *dest_bits = (uint8*) destbmp->Bits();	

	// Get row widths for offset looping
	uint32 src_width  = uint32 (sourcebmp->BytesPerRow());
	uint32 dest_width = uint32 (destbmp->BytesPerRow());

	// Offset bitmap pointers to proper spot in each bitmap
	src_bits += uint32 ( (sourcerect.top  * src_width)  + (sourcerect.left  * colorspace_size) );
	dest_bits += uint32 ( (destrect.top * dest_width) + (destrect.left * colorspace_size) );

	uint32 line_length = uint32 ((destrect.right - destrect.left)*colorspace_size);
	uint32 lines = uint32 (destrect.bottom-destrect.top+1);

	for (uint32 pos_y = 0; pos_y != lines; pos_y++)
	{ 
		memcpy(dest_bits,src_bits,line_length);

		// Increment offsets
   		src_bits += src_width;
   		dest_bits += dest_width;
	}

}

void FastCopyRect(BRect sourcerect, BBitmap *sourcebmp, 
					 BRect destrect, BBitmap *destbmp, uint8 colorspace_size)
{	// New algorithm which uses DrawBitmap for speed - destination needs to
	// accept children

	colorspace_size=4;
	BView *tempview;

	// First, clip source rect to destination
	if(sourcerect.Width() > destrect.Width())
	{	sourcerect.right=sourcerect.left+destrect.Width();
	}

	if(sourcerect.Height() > destrect.Height())
	{	sourcerect.bottom=sourcerect.top+destrect.Height();
	}

	// Second, check rectangle bounds against their own bitmaps
	BRect work_rect;

	work_rect.Set(	sourcebmp->Bounds().left,
					sourcebmp->Bounds().top,
					sourcebmp->Bounds().right,
					sourcebmp->Bounds().bottom	);
	if( !(work_rect.Contains(sourcerect)) )
	{	// something in selection must be clipped
		if(sourcerect.left < 0)
			sourcerect.left = 0;
		if(sourcerect.right > work_rect.right)
			sourcerect.right = work_rect.right;
		if(sourcerect.top < 0)
			sourcerect.top = 0;
		if(sourcerect.bottom > work_rect.bottom)
			sourcerect.bottom = work_rect.bottom;
	}

	work_rect.Set(	destbmp->Bounds().left,
					destbmp->Bounds().top,
					destbmp->Bounds().right,
					destbmp->Bounds().bottom	);

	if( !(work_rect.Contains(destrect)) )
	{	// something in selection must be clipped
		if(destrect.left < 0)
			destrect.left = 0;
		if(destrect.right > work_rect.right)
			destrect.right = work_rect.right;
		if(destrect.top < 0)
			destrect.top = 0;
		if(destrect.bottom > work_rect.bottom)
			destrect.bottom = work_rect.bottom;
	}
	tempview= new BView(destbmp->Bounds(),"tempview",B_FOLLOW_NONE,B_WILL_DRAW);
	destbmp->Lock();
	if(tempview->Parent()!=NULL)
		tempview->RemoveSelf();
	if(destbmp->ChildAt(0)!=NULL)
		destbmp->RemoveChild(destbmp->ChildAt(0));
	destbmp->AddChild(tempview);
	tempview->DrawBitmap(sourcebmp,sourcerect,destrect);
	destbmp->RemoveChild(tempview);
	destbmp->Unlock();
	delete tempview;
}

BMessage *Bitmap2Message(BBitmap *bmp)
{
	// Creates a new message containing all the data necessary to
	// pass the bitmap around, such as to the Clipboard

	if(bmp==NULL)
		return NULL;

	// will also flatten mask to alpha when completely finished
	BMessage *clip=new BMessage(POST_BITMAP);

	clip->AddString("class","bitmap");
	clip->AddRect("_frame",bmp->Bounds());
	clip->AddInt32("_cspace",bmp->ColorSpace());
	
	clip->AddInt32("_bmflags",0);
	clip->AddInt32("_rowbytes",bmp->BytesPerRow());
	clip->AddData("_data",B_RAW_TYPE,bmp->Bits(), bmp->BitsLength());

	return clip;
}

BBitmap *Message2Bitmap(BMessage *clip)
{
	const void *buffer;
	char *tempstr;
	int32 bufferLength;
	BBitmap *bmp;
	
	clip->FindString("class",(const char **)&tempstr);
	if(tempstr==NULL)
		return NULL;
		
	if (strcmp(tempstr,"bitmap")==0)
	{
		bmp = new BBitmap( clip->FindRect("_frame"),
					(color_space) clip->FindInt32("_cspace"),true );
		clip->FindData("_data",B_RAW_TYPE,(const void **)&buffer, &bufferLength);
		memcpy(bmp->Bits(), buffer, bufferLength);

		return bmp;
	}

	return NULL;
}

hsv_color RGBtoHSV(rgb_color rgbcol)
{
	float 	r=float(rgbcol.red)/255,
			g=float(rgbcol.green)/255,
			b=float(rgbcol.blue)/255,
			h,s,v;
	hsv_color returncolor;
	
	float min, max, delta;
	
	min = MIN( r, g); 
	max = MAX( r, g);
	min = MIN(min,b);
	max = MAX(max,b);	//had to change from the (min,b) in original code
	
	v = max;
	delta = max - min;
	if( max != 0 ) 
		s = delta / max;
	else
	{ 
		// r = g = b = 0	// v = s = 0, h is undefined 
		returncolor.hue=0;
		returncolor.saturation=0;
		returncolor.value=0;
		return returncolor; 
	}
	if( r == max ) 
		h = ( g - b ) / delta;			// between yellow & magenta 
	else
	if( g == max ) 
		h = 2 + ( b - r ) / delta;		// between cyan & yellow 
	else 
		h = 4 + ( r - g ) / delta;		// between magenta & cyan
	h *= 60;                               // degrees 
	if( h < 0 ) 
		h += 360;
	s *=255;
	v *=255;
	returncolor.hue=(uint8)ROUND(h);
	returncolor.saturation=(uint8)ROUND(s);
	returncolor.value=(uint8)ROUND(v);
	return returncolor; 
}

rgb_color HSVtoRGB(hsv_color hsvcol) 
{
	float 	h=hsvcol.hue,
			s=float(hsvcol.saturation)/255,
			v=float(hsvcol.value)/255,
			r,g,b;
	rgb_color returncolor;
	
	int i; 
	float f, p, q, t;
	if( s == 0 )
	{	// grayscale
		returncolor.red=(uint8)v;
		returncolor.green=(uint8)v;
		returncolor.blue=(uint8)v;
		return returncolor; 
	}
	h /= 60;                        // sector 0 to 5 
	i = (int)floor( h ); 
	f = h - i;                      // factorial part of h 
	p = v * ( 1 - s ); 
	q = v * ( 1 - s * f ); 
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i )
	{ 
		case 0: 
			r = v; g = t; b = p; 
			break; 
		case 1: 
			r = q; g = v; b = p; 
			break; 
		case 2: 
			r = p; g = v; b = t; 
			break; 
		case 3: 
			r = p; g = q; b = v; 
			break; 
		case 4: 
			r = t; g = p; b = v; 
			break; 
		default:                // case 5: 
			r = v; g = p; b = q; 
			break; 
	}
	r*=255;
	g*=255;
	b*=255;
	returncolor.red=(uint8)ROUND(r);
	returncolor.green=(uint8)ROUND(g);
	returncolor.blue=(uint8)ROUND(b);
	return returncolor; 
}

float sintable[360];
float costable[360];

void InitTrigTables(void)
{	int8 i;
	double sval,cval,current_radian;
	
	for(i=0;i<90; i++)
	{	current_radian=(i * PI)/180.0;

		// Get these so that we can do some superfast assignments
		sval=(float)sin(current_radian);
		cval=(float)cos(current_radian);
		
		// Do 4 assignments, taking advantage of sin/cos symmetry
		sintable[i]=sval;
		sintable[i+90]=cval;
		sintable[i+180]=sval * -1;
		sintable[i+270]=cval * -1;
		
		costable[i]=cval;
		costable[i+90]=sval * -1;
		costable[i+180]=cval * -1;
		costable[i+270]=sval;
	}
}

uint16 InvSine(float value)
{	// Returns the inverse sine of a value in the range 0 <= value <= 1 via 
	//	reverse-lookup any value out of range causes the function to return 0
	uint16 i=90;
	
	// Filter out bad values
	if(value > 1 || value < 0)
		return 0;
		
	while(value < sintable[i])
		i--;

	// current sintable[i] is less than value. Pick the degree value which is closer
	// to the passed value
	if( (value - sintable[i]) > (sintable[i+1] - value) )
		return (i+1);
	
	return i;		// value is closer to previous
}


uint16 InvCosine(float value)
{	// Returns the inverse cosine of a value in the range 0 <= value <= 1 via 
	//	reverse-lookup any value out of range causes the function to return 0
	uint16 i=90;

	// Filter out bad values
	if(value > 1 || value < 0)
		return 0;
		
	while(value > costable[i])
		i--;

	// current costable[i] is less than value. Pick the degree value which is closer
	// to the passed value
	if( (value - costable[i]) < (costable[i+1] - value) )
		return (i+1);

	return i;		// value is closer to previous
}

// This function is from Santa's Gift Bag
int32 CheckIntegralValue(BTextControl* field, int32 min, int32 max)
{
	const char* text = field->Text();
	int length = strlen(text);
	int32 value = 0;
	bool legal = true;
	for(int i=0; i<length; i++)
		if(!((text[i]>='0' && text[i]<='9') || (text[0]=='-' && i==0)))
			legal = false;
	if(legal)
		value = atoi(text);
	if(value < min || value > max || !legal)
	{
		if(value < min)
			value = min;
		else if(value > max)
			value = max;
		char num_buffer[12];
		sprintf(num_buffer,"%ld",value);
		field->SetText(num_buffer);
	}
	return value;
}
