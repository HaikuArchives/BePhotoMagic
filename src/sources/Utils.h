#include <Application.h>
#include <Screen.h>
#include <Bitmap.h>
#include <Rect.h>
#include <Path.h>
#include <Entry.h>
#include <File.h>
#include <AppFileInfo.h>
#include <Roster.h>
#include <TranslatorRoster.h>
#include <TextControl.h>
#include <BitmapStream.h>
#include <NodeInfo.h>
#include <View.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "PBitmapModif.h"
//#include "BBitmapAccessor.h"

#ifndef MIN
	#define MIN(a,b) ( (a<b)?a:b )
#endif

#ifndef MAX
	#define MAX(a,b) ( (a>b)?a:b )
#endif

#ifndef ROUND
	#define ROUND(a)	( (a-long(a))>=.5)?(long(a)+1):(long(a))
#endif

#ifndef PI
	#define PI 3.14159265358979323846
#endif

#ifndef POST_BITMAP
	#define POST_BITMAP 'pbmp'
#endif

#ifndef HSV_COLOR_DEF

#define HSV_COLOR_DEF
typedef struct hsv_color
{
	uint16 hue;
	uint8 saturation;
	uint8 value;
	uint8 alpha;
};

#endif

void AppDir(BPath *path);
BBitmap * LoadBitmap(char *filename);
bool IsBPMFile(const char *path);
BRect CenterChildRect(BRect parent, BRect child, bool constrain=true);
BRect CenterToScreen(BRect child);
BRect ConstrainToScreen(BRect rect);
void UninvertRect(BPoint *oldpt, BPoint *newpt);
void CopyRect(BRect sourcerect, BBitmap *sourcebmp, 
					 BRect destrect, BBitmap *destbmp, uint8 colorspace_size);
void FastCopyRect(BRect sourcerect, BBitmap *sourcebmp, 
					 BRect destrect, BBitmap *destbmp, uint8 colorspace_size);

BMessage *Bitmap2Message(BBitmap *bmp);
BBitmap *Message2Bitmap(BMessage *clip);
void ApplyBitmap( BBitmap *sourcebmp, BRect sourcerect,
					  BBitmap *destbmp,BRect destrect,uint8 blendmode);

hsv_color RGBtoHSV(rgb_color rgbcol);
rgb_color HSVtoRGB(hsv_color hsvcol);
int32 CheckIntegralValue(BTextControl* field, int32 min, int32 max);

uint16 InvSine(float value);
uint16 InvCosine(float value);
void InitTrigTables(void);
extern float sintable[360];
extern float costable[360];

