#include "FilterWindow.h"

// internal messages
#define FILTER_CANCEL	'fwcl'
#define FILTER_OK		'fwok'
#define FILTER_ABOUT	'fwab'
#define FILTER_PREVIEW	'fwpv'
#define FILTER_ZOOMIN	'fwzi'
#define FILTER_ZOOMOUT	'fwzo'

FilterWindow::FilterWindow(int32 idfilter,bool convert)
	: BWindow(BRect(100,100,500,500),"",B_FLOATING_WINDOW, B_NOT_CLOSABLE |
		B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	scale=1.0;
	desktop=new BView(Bounds(),"filterdesktop",B_FOLLOW_ALL,B_WILL_DRAW);
	desktop->SetViewColor(216,216,216);
	AddChild(desktop);
	
	bmp=new BBitmap(BRect(0,0,319,239),B_RGBA32,true);
	//Undocumented BBitmap constructor
	workbmp=new BBitmap(bmp);
	plugin_id=(image_addon_id)idfilter;
	
	ok=new BButton(BRect(10,180,55,200),"filterokbutton","OK",
		new BMessage(FILTER_OK),B_FOLLOW_NONE);
	desktop->AddChild(ok);

	cancel=new BButton(BRect(65,180,110,200),"filtercancelbutton","Cancel",
		new BMessage(FILTER_CANCEL),B_FOLLOW_NONE);
	desktop->AddChild(cancel);

//	about=new BButton(BRect(120,180,165,200),"filteraboutbutton","About",
//		new BMessage(FILTER_ABOUT),B_FOLLOW_NONE);
//	AddChild(about);

	btpreview=new BButton(BRect(10,210,55,230),"filterpreviewbutton","Preview",
		new BMessage(FILTER_PREVIEW),B_FOLLOW_NONE);
	desktop->AddChild(btpreview);

	zoomin=new BButton(BRect(65,210,110,230),"zoominbutton","+",
		new BMessage(FILTER_ZOOMIN),B_FOLLOW_NONE);
	desktop->AddChild(zoomin);

	zoomout=new BButton(BRect(115,210,160,230),"zoomoutbutton","-",
		new BMessage(FILTER_ZOOMOUT),B_FOLLOW_NONE);
	desktop->AddChild(zoomout);

	preview=new PreView(BRect(10,10,160,160),"filterpreview",B_FOLLOW_NONE,B_WILL_DRAW);
	desktop->AddChild(preview);
	
	preview->view=workbmp->Bounds();
	preview->SetBitmap(workbmp);
//	BBitmap *tempbmp;
//	tempbmp=BTranslationUtils::GetBitmap('bits',"back_text.png");
//	preview->SetViewBitmap(tempbmp,tempbmp->Bounds(),tempbmp->Bounds());
//	delete tempbmp;
	preview->Invalidate();
	
	// Set up invoker for the preview stuff
	preview_invoker=new BInvoker(new BMessage(FILTER_PREVIEW),(BHandler *)this);
	pluginconfig.MakeEmpty();
	pluginconfig.AddPointer("preview_invoker",(void *)preview_invoker);

	// Set up the config view for the filter
	Image_MakeConfigurationView(plugin_id,&pluginconfig,&configview);
	uses_view=false;
	if(configview != NULL)
	{	desktop->AddChild(configview);
		uses_view=true;
		configview->MoveTo(BPoint(180,5));
		ResizeTo(185.0+configview->Bounds().Width()+10,
				(configview->Bounds().Height()+10>240.0)?configview->Bounds().Height()+10:240);
	}
	else
	{	ResizeTo(185.0,240.0);
	}
	Image_GetAddonInfo(plugin_id,&plugin_name,&plugin_info,
				&plugin_category,&plugin_version);
	SetTitle(plugin_name);
	Show();
}

void FilterWindow::AllAttached(void)
{
}

FilterWindow::~FilterWindow(void)
{	delete workbmp;
	delete bmp;
}

void FilterWindow::MessageReceived(BMessage *msg)
{
	switch(msg->what)
	{
		case FILTER_ABOUT:
			break;
		case FILTER_PREVIEW:
		{
			delete workbmp;
			workbmp=new BBitmap(bmp);
			preview->SetBitmap(workbmp);
			Image_GetConfigurationMessage(plugin_id,&pluginconfig,NULL);
			BBitmapAccessor bmp_accessor(workbmp);
			bmp_accessor.SetDispose(false);
			Image_Manipulate(plugin_id, &bmp_accessor, &pluginconfig);
			preview->Draw(Bounds());
			break;
		}
		case FILTER_OK:
		{	// Send through the filter and then post to app
			be_app->PostMessage(SHOW_WAIT);
			delete workbmp;
			workbmp=new BBitmap(bmp);
			preview->SetBitmap(workbmp);
			Image_GetConfigurationMessage(plugin_id,&pluginconfig,NULL);
			BBitmapAccessor bmp_accessor(workbmp);
			bmp_accessor.SetDispose(false);
			Image_Manipulate(plugin_id, &bmp_accessor, &pluginconfig);
			preview->Draw(Bounds());
			BMessage *filtered=Bitmap2Message(workbmp);
			filtered->what=(int32)FILTERED_BITMAP;
			be_app->PostMessage(filtered);
			be_app->PostMessage(CLOSE_WAIT);
			Lock();
			Close();
			break;
		}
		case FILTER_CANCEL:
			be_app->PostMessage(FILTERED_BITMAP);
			Lock();
			Close();
			break;
		case FILTER_ZOOMIN:
			preview->SetZoom(true);
			break;
		case FILTER_ZOOMOUT:
			preview->SetZoom(false);
			break;
		case FLATTENED_BITMAP:
		{	delete bmp;
			bmp=Message2Bitmap(msg);
			delete workbmp;
			if(bmp!=NULL)
			{	workbmp=new BBitmap(bmp);
			}
			else
			{	bmp=new BBitmap(BRect(0,0,319,239),B_RGBA32,true);
				workbmp=new BBitmap(bmp);
			}
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}

BBitmap *FilterWindow::MergeSelection(BBitmap *bitmap,BBitmap *selection)
{	return bitmap;
}

BBitmap *FilterWindow::CreatePreview(BBitmap *source)
{	return source;
}

PreView::PreView(BRect frame, const char *name, uint32 resize, uint32 flags)
	: BView (frame, name, resize, flags)
{	
	use_bitmap=false;
	view=Bounds();
	zoom=1.0;
	boundsrect=view;
	unzoomed=view;
}

PreView::~PreView(void)
{
}

void PreView::Draw(BRect update_rect)
{	
	FillRect(boundsrect,B_SOLID_LOW);
	SetDrawingMode(B_OP_ALPHA);
	if(use_bitmap==true)
		DrawBitmap(display,view,Bounds());
	SetDrawingMode(B_OP_COPY);
}

void PreView::SetZoom(bool zoomin)
{
	if(zoomin==false)
	{
		if(zoom >= 1.0)
			zoom -=0.5;
		else
			zoom /=2;
	}
	else
	{	if(zoom >= 1.0)
			zoom +=0.5;
		else
			zoom *=2;
	}
	if(zoom==1.0)
	{	boundsrect=unzoomed;
	}
	else
	{	boundsrect.left=floor(boundsrect.left/zoom);
		boundsrect.top=floor(boundsrect.top/zoom);
		boundsrect.right=floor(boundsrect.right/zoom);
		boundsrect.bottom=floor(boundsrect.bottom/zoom);
	}
	SetScale(zoom);
	Draw(Bounds());
}

void PreView::MouseDown(BPoint pt)
{
	oldpt=pt;
}

void PreView::MouseMoved(BPoint pt,uint32 transit,BMessage *msg)
{
	float dx,dy;
	BPoint where;
	uint32 buttons;

	// Drag the view if mouse button is down
	GetMouse(&where,&buttons);
	if(buttons!=0)
	{	newpt=pt;
	
		dx=pt.x-oldpt.x;
		dy=pt.y-oldpt.y;
		view.OffsetBy(dx,dy);
		Window()->PostMessage(FILTER_PREVIEW);
	}
}

void PreView::SetBitmap(BBitmap *bmp)
{	use_bitmap=true;
	view=bmp->Bounds();
	display=bmp;
}

void PreView::DisableBitmap(void)
{	use_bitmap=false;
}

void PreView::SetDisplay(BRect rect)
{
}
