#include "BPMPrefs.h"

BPMPrefs::BPMPrefs(void)
{
	toolwin_open=false;
	layerwin_open=false;
	brushwin_open=false;
	optionswin_open=false;
	mousewin_open=false;
	backwin_open=false;
	forewin_open=false;
	
	mainwin_frame.Set(0,0,0,0);
	toolwin_frame.Set(0,0,0,0);
	layerwin_frame.Set(0,0,0,0);
	brushwin_frame.Set(0,0,0,0);
	optionswin_frame.Set(0,0,0,0);
	mousepoint.Set(0,0);
	backpoint.Set(0,0);
	forepoint.Set(0,0);
	
}

BPMPrefs& BPMPrefs::operator=(const BPMPrefs &from)
{
	toolwin_open=from.toolwin_open;
	layerwin_open=from.layerwin_open;
	brushwin_open=from.brushwin_open;
	optionswin_open=from.optionswin_open;
	backwin_open=from.backwin_open;
	forewin_open=from.forewin_open;
	
	mainwin_frame=from.mainwin_frame;
	toolwin_frame=from.toolwin_frame;
	layerwin_frame=from.layerwin_frame;
	brushwin_frame=from.brushwin_frame;
	optionswin_frame=from.optionswin_frame;
	mousepoint=from.mousepoint;
	backpoint=from.backpoint;
	forepoint=from.forepoint;
	return *this;
}

bool BPMPrefs::Save(const char *path)
{
	yprefs prefs;

	prefs.PutParam("mainwin.left",mainwin_frame.left);
	prefs.PutParam("mainwin.top",mainwin_frame.top);
	prefs.PutParam("mainwin.right",mainwin_frame.right);
	prefs.PutParam("mainwin.bottom",mainwin_frame.bottom);

	prefs.PutParam("toolwin",toolwin_open);
	prefs.PutParam("toolwin.left",toolwin_frame.left);
	prefs.PutParam("toolwin.top",toolwin_frame.top);
	prefs.PutParam("toolwin.right",toolwin_frame.right);
	prefs.PutParam("toolwin.bottom",toolwin_frame.bottom);

	prefs.PutParam("layerwin",layerwin_open);
	prefs.PutParam("layerwin.left",layerwin_frame.left);
	prefs.PutParam("layerwin.top",layerwin_frame.top);
	prefs.PutParam("layerwin.right",layerwin_frame.right);
	prefs.PutParam("layerwin.bottom",layerwin_frame.bottom);

	prefs.PutParam("brushwin",brushwin_open);
	prefs.PutParam("brushwin.left",brushwin_frame.left);
	prefs.PutParam("brushwin.top",brushwin_frame.top);
	prefs.PutParam("brushwin.right",brushwin_frame.right);
	prefs.PutParam("brushwin.bottom",brushwin_frame.bottom);

	prefs.PutParam("optionswin",optionswin_open);
	prefs.PutParam("optionswin.left",optionswin_frame.left);
	prefs.PutParam("optionswin.top",optionswin_frame.top);
	prefs.PutParam("optionswin.right",optionswin_frame.right);
	prefs.PutParam("optionswin.bottom",optionswin_frame.bottom);

	prefs.PutParam("forewin",forewin_open);
	prefs.PutParam("forepoint.x",forepoint.x);
	prefs.PutParam("forepoint.y",forepoint.y);

	prefs.PutParam("backwin",backwin_open);
	prefs.PutParam("backpoint.x",backpoint.x);
	prefs.PutParam("backpoint.y",backpoint.y);

	prefs.PutParam("mousewin",mousewin_open);
	prefs.PutParam("mousepoint.x",mousepoint.x);
	prefs.PutParam("mousepoint.y",mousepoint.y);

	prefs.SavePrefs(path);
	return true;
}

bool BPMPrefs::Load(const char *path)
{
	yprefs prefs;
	BDirectory dir;
	BEntry entry;
	bool bad_prefs=false;
	
	// Check for the preferences directory of the old BePhotoMagic codebase
	if(dir.SetTo(path)==B_OK)
	{
		if(entry.Remove() != B_OK)
		{	bad_prefs=true;

			// Using a BAlert include because this doesn't already use Utils.cc
			BAlert *alert=new BAlert("BePhotoMagic","BePhotoMagic has detected preferences"
				"from a previous version. If you wish to save your preferences in this version,"
				" delete the folder /boot/home/config/settings/BePhotoMagic\n",
				"OK");
			alert->Go();
		}
	}
	
	if(prefs.LoadPrefs(path)==-1 || bad_prefs==true)
	{
		// Preferences nonexistent. Reset to defaults and create
		toolwin_frame.Set(8,80,55,350);
		mainwin_frame.Set(100,50,650,550);
		layerwin_frame.Set(500,400,770,590);
		brushwin_frame.Set(60,120,260,350);
		optionswin_frame.Set(300,400,570,520);

		toolwin_open=true;
		optionswin_open=false;
		layerwin_open=false;
		brushwin_open=false;	
		forewin_open=false;
		forepoint.Set(400,200);
		backwin_open=false;
		backpoint.Set(400,400);
		mousepoint.Set(300,10);
		prefs.PutParam("mainwin.left","100");
		prefs.PutParam("mainwin.top","50");
		prefs.PutParam("mainwin.right","650");
		prefs.PutParam("mainwin.bottom","550");

		prefs.PutParam("toolwin","1");
		prefs.PutParam("toolwin.left","8");
		prefs.PutParam("toolwin.top","80");
		prefs.PutParam("toolwin.right","55");
		prefs.PutParam("toolwin.bottom","350");

		prefs.PutParam("layerwin","0");
		prefs.PutParam("layerwin.left","500");
		prefs.PutParam("layerwin.top","400");
		prefs.PutParam("layerwin.right","770");
		prefs.PutParam("layerwin.bottom","590");

		prefs.PutParam("brushwin","0");
		prefs.PutParam("brushwin.left","60");
		prefs.PutParam("brushwin.top","120");
		prefs.PutParam("brushwin.right","260");
		prefs.PutParam("brushwin.bottom","350");

		prefs.PutParam("optionswin","0");
		prefs.PutParam("optionswin.left","225");
		prefs.PutParam("optionswin.top","400");
		prefs.PutParam("optionswin.right","295");
		prefs.PutParam("optionswin.bottom","520");

		prefs.PutParam("forewin","0");
		prefs.PutParam("forepoint.x","400");
		prefs.PutParam("forepoint.y","200");

		prefs.PutParam("backwin","0");
		prefs.PutParam("backpoint.x","400");
		prefs.PutParam("backpoint.y","400");

		prefs.PutParam("mousewin","0");
		prefs.PutParam("mousepoint.x","300");
		prefs.PutParam("mousepoint.y","10");

		prefs.SavePrefs(path);
		return false;
	}
	else
	{	
		mainwin_frame.Set(prefs.ptof("mainwin.left"),
							prefs.ptof("mainwin.top"),
							prefs.ptof("mainwin.right"),
							prefs.ptof("mainwin.bottom") );
		toolwin_frame.Set(prefs.ptof("toolwin.left"),
							prefs.ptof("toolwin.top"),
							prefs.ptof("toolwin.right"),
							prefs.ptof("toolwin.bottom") );
		layerwin_frame.Set(prefs.ptof("layerwin.left"),
							prefs.ptof("layerwin.top"),
							prefs.ptof("layerwin.right"),
							prefs.ptof("layerwin.bottom") );
		brushwin_frame.Set(prefs.ptof("brushwin.left"),
							prefs.ptof("brushwin.top"),
							prefs.ptof("brushwin.right"),
							prefs.ptof("brushwin.bottom") );
		optionswin_frame.Set(prefs.ptof("optionswin.left"),
							prefs.ptof("optionswin.top"),
							prefs.ptof("optionswin.right"),
							prefs.ptof("optionswin.bottom") );

		if(prefs.ptoi("toolwin")==1)
			toolwin_open=true;
		else
			toolwin_open=false;

		if(prefs.ptoi("layerwin")==1)
			layerwin_open=true;
		else
			layerwin_open=false;

		if(prefs.ptoi("brushwin")==1)
			brushwin_open=true;
		else
			brushwin_open=false;

		if(prefs.ptoi("optionswin")==1)
			optionswin_open=true;
		else
			optionswin_open=false;

		if(prefs.ptoi("forewin")==1)
			forewin_open=true;
		else
			forewin_open=false;

		if(prefs.ptoi("backwin")==1)
			backwin_open=true;
		else
			backwin_open=false;

		if(prefs.ptoi("mousewin")==1)
			mousewin_open=true;
		else
			mousewin_open=false;

		forepoint.Set(prefs.ptof("forepoint.x"),
					prefs.ptof("forepoint.y") );
		backpoint.Set(prefs.ptof("backpoint.x"),
					prefs.ptof("backpoint.y") );
		mousepoint.Set(prefs.ptof("mousepoint.x"),
					prefs.ptof("mousepoint.y") );

		// Validate frames
		BScreen screen;
		BRect srect=screen.Frame();

		if(!(srect.Contains(mainwin_frame)))
			mainwin_frame.Set(100,50,650,550);
		if(!(srect.Contains(toolwin_frame)))
			toolwin_frame.Set(8,80,55,350);
		if(!(srect.Contains(layerwin_frame)))
			layerwin_frame.Set(500,400,770,590);
		if(!(srect.Contains(brushwin_frame)))
			brushwin_frame.Set(60,120,260,350);
		if(!(srect.Contains(optionswin_frame)))
			optionswin_frame.Set(225,400,495,520);

		if(!(srect.Contains(forepoint)))
			forepoint.ConstrainTo(srect);
		if(!(srect.Contains(backpoint)))
			backpoint.ConstrainTo(srect);
		if(!(srect.Contains(mousepoint)))
			mousepoint.ConstrainTo(srect);
	}
	return true;
}

