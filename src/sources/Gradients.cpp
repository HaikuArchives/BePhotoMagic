void PicView::RadialGradient(BPoint startpt,rgb_color start,
		BPoint endpt, rgb_color end)
{
	float rstep=0.0,gstep=0.0,bstep=0.0,astep=0.0;
	
	int32 	i,				// index var
			x1,y1,x2,y2,	// int points for the line
			dx,dy,
			left,right,top,bottom,
			length;
	rgb_color currentcolor=start;

	BRect viewbounds=Bounds();
	left=int32(viewbounds.left);
	top=int32(viewbounds.top);
	right=int32(viewbounds.right);
	bottom=int32(viewbounds.bottom);

	// we're doing integer math here, going left to right
	if(startpt.x<endpt.x)
	{	x1=int32(startpt.x);
		x2=int32(endpt.x);
		y1=int32(startpt.y);
		y2=int32(endpt.y);
	}
	else
	{	x1=int32(endpt.x);
		x2=int32(startpt.x);
		y1=int32(endpt.y);
		y2=int32(startpt.y);
	}

	// Short ciruit in special cases
	if(x1==x2 && y1==y1)	// pointless case
		return;

	dx=x2-x1;
	dy=y2-y1;

	// Calculate radius of gradient	
	length=int32(sqrt((dx*dx)+(dy*dy)));
		
	// get step size from beginning color to end color
	rstep=float(end.red-start.red)/length;
	gstep=float(end.green-start.green)/length;
	bstep=float(end.blue-start.blue)/length;
	astep=float(end.alpha-start.alpha)/length;
	
	SetPenSize(2);

	SetDrawingMode(B_OP_ALPHA);
	// loop to actually draw the gradient
	for(i=0;i<=length;i++)
	{
		SetHighColor(currentcolor);
		StrokeEllipse(startpt,i,i);
		currentcolor.red=uint8(start.red+(float(i)*rstep));
		currentcolor.green=uint8(start.green+(float(i)*gstep));
		currentcolor.blue=uint8(start.blue+(float(i)*bstep));
		currentcolor.alpha=uint8(start.alpha+(float(i)*astep));
	}
}

void PicView::RectGradient(BPoint startpt,rgb_color start,
		BPoint endpt, rgb_color end,bool round=false)
{
	BRect viewbounds(50,50,400,400);
	float rstep=0.0,gstep=0.0,bstep=0.0,astep=0.0;
	
	uint32 	i,				// index var
			length;
	rgb_color currentcolor=end;
	BRect looprect(startpt,endpt);

	// get step size from beginning color to end color
	length=MIN(abs(looprect.IntegerWidth()),abs(looprect.IntegerHeight()));

	rstep=float(end.red-start.red)/(length>>1);
	gstep=float(end.green-start.green)/(length>>1);
	bstep=float(end.blue-start.blue)/(length>>1);
	astep=float(end.alpha-start.alpha)/(length>>1);
	
	SetPenSize(2);

	SetDrawingMode(B_OP_ALPHA);
	// loop to actually draw the gradient
	i=0;
	while(looprect.IntegerWidth()>0 && looprect.IntegerWidth()>0)
	{	
		SetHighColor(currentcolor);
		StrokeRect(looprect);
		looprect.InsetBy(1,-1);
		
		currentcolor.red=uint8(end.red-(float(i)*rstep));
		currentcolor.green=uint8(end.green-(float(i)*gstep));
		currentcolor.blue=uint8(end.blue-(float(i)*bstep));
		currentcolor.alpha=uint8(end.alpha-(float(i)*astep));
		i++;
	}
}

