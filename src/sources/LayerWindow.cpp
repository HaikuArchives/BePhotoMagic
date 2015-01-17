#include "LayerWindow.h"

/*
	This code is a mess.

	LayerWindow needs to
	1) Notify BPMWindow of data changes caused by use of its child views' controls
	2) Be able to change the values of the controls without the controls causing an
		endless loop of passing the message buck.
*/

LayerWindow::LayerWindow(BRect frame, char *title)
				: BWindow(frame, title, B_FLOATING_WINDOW, B_NOT_MINIMIZABLE
							| B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
	BRect a_rect  = Bounds();
	a_rect.bottom=LAYER_TOP_HEIGHT;
	
	options_view = new LayerOptionsView(a_rect);
	AddChild(options_view);
	
	BRect rect  = Bounds();
	rect.top += LAYER_TOP_HEIGHT+1;

	layersview=new LayersView(rect);
	AddChild(layersview);	

	SetSizeLimits( frame.Width(),frame.Width() , options_view->Bounds().Height()+LAYER_TOP_HEIGHT, 3000);
	blackhole_handler=new BHandler("layerwindow_blackhole");
	notify=true;
}


LayerWindow::~LayerWindow()
{	BMessage *msg=new BMessage(LAYERWIN_CLOSED);
	msg->AddRect("frame",Frame());
	bpmwindow->PostMessage(msg);
	delete blackhole_handler;
}

void LayerWindow::FrameMoved(BPoint origin)
{
#ifndef CHILDWIN_MOVED
	#define CHILDWIN_MOVED 'cwmv'
#endif

	BMessage *msg=new BMessage(CHILDWIN_MOVED);
	msg->AddPoint("layerwin",origin);
	bpmwindow->PostMessage(msg);
}

void LayerWindow::SetControlNotification(bool enabled)
{	// This bad boy diverts all control messages to the blackhole_handler -
	// messages go in, but no one comes out...
	if(enabled==true)
	{
		options_view->opacity->SetTarget(this);
		options_view->draw_mode->Menu()->SetTargetForItems(this);
		layersview->activelist->invoker->SetTarget(this);
		layersview->activelist->SetTarget(this);
		layersview->visiblelist->invoker->SetTarget(this);
		layersview->visiblelist->SetTarget(this);
	}
	else
	{
		options_view->opacity->SetTarget(blackhole_handler);
		options_view->draw_mode->Menu()->SetTargetForItems(blackhole_handler);
		layersview->activelist->invoker->SetTarget(blackhole_handler);
		layersview->activelist->SetTarget(blackhole_handler);
		layersview->visiblelist->invoker->SetTarget(blackhole_handler);
		layersview->visiblelist->SetTarget(blackhole_handler);
	}
	notify=enabled;
}

bpm_status LayerWindow::SetLayers(uint8 number,bool update_stack=false)
{
	layersview->number_layers=number;
	if(update_stack==true)
		layersview->UpdateLayerStack(true);
	return BPM_OK;
}

bpm_status LayerWindow::ShowLayer(uint8 index,bool update_stack=false)
{
	layersview->visible[index]=true;
	if(update_stack==true)
		layersview->UpdateLayerStack(true);
	return BPM_OK;
}

bpm_status LayerWindow::HideLayer(uint8 index,bool update_stack=false)
{
	layersview->visible[index]=false;
	if(update_stack==true)
		layersview->UpdateLayerStack(true);
	return BPM_OK;
}

bpm_status LayerWindow::SetActive(uint8 index,bool update_stack=false)
{
	layersview->active_layer=index;
	if(update_stack==true)
		layersview->UpdateLayerStack(true);
	return BPM_OK;
}

bpm_status LayerWindow::SetBlendMode(uint8 mode)
{
	options_view->draw_mode->Menu()->ItemAt(mode)->SetMarked(true);
	return BPM_OK;
}

bpm_status LayerWindow::SetOpacity(uint8 value)
{
	options_view->opacity->SetValue(float(value));
	return BPM_OK;
}

void LayerWindow::MessageReceived(BMessage *msg)
{
int8 tempint8,i,hidden_index;
BMessage *tempmsg;
	switch(msg->what)
	{
		case LAYER_ACTIVATE:
			// Received when Active list is clicked
			if(layersview->activelist->CurrentSelection()==-1 || notify==false)
				break;
			tempmsg=new BMessage(ALTER_LAYER_DATA);
			tempint8=int8(layersview->activelist->CountItems()
						-(layersview->activelist->CurrentSelection()+1) );
			tempmsg->AddInt8("active_layer",tempint8);
			bpmwindow->PostMessage(tempmsg);

			// Update layer visibility if current layer hidden
			if(layersview->visible[tempint8]==false)
				layersview->visiblelist->Select(tempint8,true);
			// no need to post message - Select() automagically generates one
			break;
			
		case LAYER_TOGGLE_VISIBLE:
			// Received when user clicks Visible list
			if(notify==false)
				break;
			tempmsg=new BMessage(ALTER_LAYER_DATA);
			hidden_index=0;
			
			for(i=0;i<layersview->number_layers;i++)
			{	
				layersview->visible[(layersview->number_layers-1)-i]=layersview->visiblelist->ItemAt(i)->IsSelected();
				
				if(layersview->visible[(layersview->number_layers-1)-i]==false)
				{	tempmsg->AddInt8("hidden_layer",(layersview->number_layers-1)-i);
					hidden_index++;
				}
			}
			tempmsg->AddInt8("num_hidden_layers",hidden_index);
			bpmwindow->PostMessage(tempmsg);
			break;
		case UPDATE_OPACITY:
		{
			int8 tempint8=options_view->opacity->Value();
			tempmsg=new BMessage(ALTER_LAYER_DATA);
			tempmsg->AddInt8("opacity",tempint8);
			bpmwindow->PostMessage(tempmsg);
			break;
		}	
		case UPDATE_TMODE:
		{
			int8 tempint8=options_view->draw_mode->Menu()->IndexOf(options_view->draw_mode->Menu()->FindMarked());
			tempmsg=new BMessage(ALTER_LAYER_DATA);
			tempmsg->AddInt8("blendmode",tempint8);
			bpmwindow->PostMessage(tempmsg);
			break;
		}	
		default:
			BWindow::MessageReceived(msg);
	}
}

LayerOptionsView::LayerOptionsView(BRect r) :
	BView(r, "layeroptions", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	BRect rect;
	
	rect.Set(4,12,74,24);
	SetViewColor(216,216,216);
	
	BMenu *modemenu = new BMenu("Drawing Mode");
	modemenu->AddItem(new BMenuItem(SpTranslate("Normal"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Multiply"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Divide"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Difference"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Exclusion"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Lighten"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Darken"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("OR"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Addition"), new BMessage(UPDATE_TMODE)));
	modemenu->AddItem(new BMenuItem(SpTranslate("Subtract"), new BMessage(UPDATE_TMODE)));

	modemenu->SetLabelFromMarked(true);

	draw_mode = new BMenuField(rect,"",NULL,modemenu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
	modemenu->ItemAt(0)->SetMarked(true);
	modemenu->SetLabelFromMarked(true); 
	AddChild(draw_mode);
	
	rect = Bounds();
	rect.left =78;
	rect.right = 228;
	rect.top = 4;
	
	
	opacity = new TSlider(rect,"opacityts",SpTranslate("Opacity"),"%d",
							new BMessage(UPDATE_OPACITY),1,255);
	
	rgb_color a_col={128,128,255}; 
	rgb_color a_colb={0,0,155}; 
	opacity->SetBarColor(a_col);
	opacity->UseFillColor(true,&a_colb);
	opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
	opacity->SetHashMarkCount(10+1);

	opacity->SetValue(255L);
	AddChild(opacity);
	
	rect.left = Bounds().right-15;
	rect.top = 15;
	rect.right = Bounds().right;
	rect.bottom = 30;
	rect.OffsetBy(-2,2);
	
	tmenu = new TriangleMenuView(rect);
	AddChild(tmenu);
}

void LayerOptionsView::Draw(BRect r)
{
	DrawString(SpTranslate("Blending mode:"),BPoint(5,9));
}

void LayerOptionsView::AttachedToWindow(void)
{
	if(Window()==NULL)
		return;
		
	opacity->SetTarget(Window());
	draw_mode->Menu()->SetTargetForItems(Window());
}

/*
void LayerOptionsView::MessageReceived(BMessage *msg)
{
BMessage *updatemsg;
	switch(msg->what)
	{
		case UPDATE_OPACITY:
		{
			int8 tempint8=opacity->Value();
			updatemsg=new BMessage(ALTER_LAYER_DATA);
			updatemsg->AddInt8("opacity",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		case UPDATE_TMODE:
		{
			int8 tempint8=draw_mode->Menu()->IndexOf(draw_mode->Menu()->FindMarked());
			updatemsg=new BMessage(ALTER_LAYER_DATA);
			updatemsg->AddInt8("blendmode",tempint8);
			bpmwindow->PostMessage(updatemsg);
			break;
		}	
		default:
			BView::MessageReceived(msg);
			break;
	}
}
*/
LayersView::LayersView(BRect frame) :
	BView(frame,"layersview",B_FOLLOW_ALL,B_WILL_DRAW)
{	
	SetViewColor(216,216,216);
	BRect rect(Bounds());
	rect.top=15;
	rect.right=rect.left+floor( (rect.Width()/2)-5);
	rect.right -=B_V_SCROLL_BAR_WIDTH;

	visiblelist=new LayerList(rect,"visiblelist",LAYER_TOGGLE_VISIBLE,B_MULTIPLE_SELECTION_LIST);
	scrollvisible = new BScrollView("scrollvisible", visiblelist,
											    B_FOLLOW_ALL, B_WILL_DRAW, FALSE, TRUE);
	AddChild(scrollvisible);
	rect.Set( (Bounds().Width()/2)+5,15,Bounds().Width(),Bounds().Height());
	rect.right -=B_V_SCROLL_BAR_WIDTH;

	activelist=new LayerList(rect,"activelist",LAYER_ACTIVATE,B_SINGLE_SELECTION_LIST);
	scrollactive = new BScrollView("scrollactive", activelist,
									    B_FOLLOW_ALL, B_WILL_DRAW, FALSE, TRUE);
	AddChild(scrollactive);

	for(int16 i=0;i<256;i++)
		visible[i]=true;
	active_layer=0;
	number_layers=0;
}

LayersView::~LayersView(void)
{
}

void LayersView::Draw(BRect update_rect)
{
	DrawString(SpTranslate("Visible"),BPoint(1,12));
	DrawString(SpTranslate("Active"),BPoint((Bounds().Width()/2)+5,12));
}

void LayersView::UpdateLayerStack(bool imagesopen)
{	int8 index;
	visiblelist->EmptyList();
	activelist->EmptyList();
	
	visiblelist->PopulateList(number_layers);
	activelist->PopulateList(number_layers);
	
	visiblelist->DeselectAll();
	
	for(int16 i=0;i<number_layers; i++)
		visiblelist->Select(i,visible[i]);

	// Select currently active layer in list
	index=activelist->CountItems()-(active_layer+1);
	if(index<0)
		index=0;
	if(activelist->CountItems()>0)
		activelist->Select(index);
}

LayerList::LayerList(BRect frame,const char *name,int32 message,list_view_type type) 
	: BListView(frame,name,type)
{
	selectionmsg=(message != 0)?selectionmsg=message:0;
	invoker=new BInvoker(new BMessage(message),(BHandler *)be_app);
}

void LayerList::AttachedToWindow(void)
{	invoker->SetTarget(Window());
}
	
void LayerList::SelectionChanged(void)
{	if(selectionmsg != 0)
//		Window()->PostMessage(new BMessage(selectionmsg));
		invoker->Invoke();
}	

LayerList::~LayerList(void)
{	delete invoker;
}

void LayerList::PopulateList(int8 number_layers)
{	
	EmptyList();
	
	char tempstr[255];
	for(int8 i=0;i<number_layers;i++)
	{	sprintf(tempstr,"Layer %d",i);
	
		AddItem(new BStringItem(tempstr),0);
	}
}

void LayerList::EmptyList(void)
{
	while(CountItems()!=0)
		delete RemoveItem( (int32)0 );
}

TriangleMenuView::TriangleMenuView(BRect r) : 
	BView(r, "", B_FOLLOW_TOP | B_FOLLOW_RIGHT, B_WILL_DRAW)
{
	SetViewColor(216,216,216);
	
	off = BTranslationUtils::GetBitmap('bits',"triangle_menu.png");
	on = BTranslationUtils::GetBitmap('bits',"triangle_menu_p.png");
	img = off;
	
	pm = new  BPopUpMenu("", false, false, B_ITEMS_IN_COLUMN) ;
	pm->AddItem(new BMenuItem(SpTranslate("New"),new BMessage(LAYER_NEW)));
	pm->AddItem(new BMenuItem(SpTranslate("Duplicate"),new BMessage(LAYER_DUPLICATE)));
	pm->AddItem(new BMenuItem(SpTranslate("Delete"),new BMessage(LAYER_DELETE)));
	pm->AddSeparatorItem();
	pm->AddItem(new BMenuItem(SpTranslate("Make Composite Layer"),new BMessage(LAYER_COMPOSITE)));
	pm->AddItem(new BMenuItem(SpTranslate("Flatten"),new BMessage(LAYER_FLATTEN)));
	pm->AddSeparatorItem();
	pm->AddItem(new BMenuItem(SpTranslate("Move Up"),new BMessage(LAYER_MOVE_UP)));
	pm->AddItem(new BMenuItem(SpTranslate("Move Down"),new BMessage(LAYER_MOVE_DOWN)));
	pm->AddItem(new BMenuItem(SpTranslate("Move to Top"),new BMessage(LAYER_MOVE_TOP)));
	pm->AddItem(new BMenuItem(SpTranslate("Move to Bottom"),new BMessage(LAYER_MOVE_BOTTOM)));
//	pm->SetTargetForItems(bpmwindow);
	pm->SetTargetForItems(be_app);
}

TriangleMenuView::~TriangleMenuView()
{
	delete on;
	delete off;
}


void TriangleMenuView::Draw(BRect update_rect)
{
	DrawBitmap(img,BPoint(0,0));
}

void TriangleMenuView::MouseDown(BPoint point)
{
	img = on;
	Draw(Bounds());
	pm->Go(ConvertToScreen(point),true,false,false);
	img = off;
	Draw(Bounds());
}
