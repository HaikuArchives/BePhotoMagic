#include <String.h>
#include <stdio.h>
#include <Window.h>
//#include <algobase.h>
#include "TSlider.h"

#ifndef MAX
	#define MAX(a,b) (a>b)?a:b
#endif

void TSlider::SetValue(int32 value)
{
	ResetKeys();

	// Don't invoke the preview unless the value actually changes.
	bool hasTarget = !!Target();
	bool changed = value != Value();

	BSlider::SetValue(value);
	
	if (changed && value == Value())
	{
		if (hasTarget)
			ValueChanged();
	}
}

void TSlider::ValueChanged()
{
	if (mPreviewInvoker)
	{
		BLooper* looper;
		BHandler* handler;
		handler = mPreviewInvoker->Target(&looper);
		BMessage msg(*mPreviewInvoker->Message());

		// Gross hack to work around BSlider bug (Modification message is queued up but not delivered while tracking BSlider)

		if (looper && looper == Window())
		{
			looper->DispatchMessage(&msg, handler);
		}
		else
			mPreviewInvoker->Invoke(&msg);
	}
}


void TSlider::DrawThumb()
{
	BSlider::DrawThumb();

	if (!mFormat.Length())
		return;

	font_height fh;
	BView* v = OffscreenView();
	BRect r = ThumbFrame();
	v->SetHighColor(0, 0, 0);
	v->SetFont(be_plain_font);
	be_plain_font->GetHeight(&fh);

	char buf[256];
	if (number.Length())
		strcpy(buf, number.String());
	else if (mScale == 1.0)
	{
		sprintf(buf, mFormat.String(), Value() + (int32) mOffset);
	}
	else
		sprintf(buf, mFormat.String(), mOffset + Value() * mScale);
		
	float w = v->StringWidth(buf);
	float x = 1 + rint(r.left + (r.Width() - v->StringWidth(buf)) / 2);
	if (x + w > Bounds().right)
		x = Bounds().right - w;
	if (x < Bounds().left)
		x = Bounds().left;
	
	v->DrawString(buf, BPoint(x, r.top - fh.descent - 1));
}

void TSlider::KeyDown(const char *bytes, int32 numBytes)
{
	lastKey = system_time();
	if (!firstKey)
		firstKey = lastKey;

	switch(*bytes)
	{
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			number += *bytes;
			DrawSlider();
			break;
		case B_BACKSPACE:
			number.Truncate(MAX(0L, number.Length() - 1));
			DrawSlider();
			break;
		case ' ':
		case B_ENTER:
			SetValueFromKeys();
			break;
		case B_TAB:
			SetValueFromKeys();
			BSlider::KeyDown(bytes, numBytes);
			break;
		default:
		{
			if (number.Length())
			{
				ResetKeys();
				BRect r(ThumbFrame());
				r.top = 0;
				r.InsetBy(-10, 0);
				Invalidate(r);
			}
			BSlider::KeyDown(bytes, numBytes);
		}
	}
}

void TSlider::SetValueFromKeys()
{
//	if (firstKey == lastKey)
//	{
//		ResetKeys();
//		return;
//	}
	
	BRect r(ThumbFrame());
	r.top = 0;
	r.InsetBy(-10, 0);
	Invalidate(r);

	float value;
	if (1 != sscanf(number.String(), "%f", &value))
	{
		ResetKeys();
	}
	else
		SetValue(value);
}

void TSlider::ResetKeys()
{
	number.Truncate(0);
	firstKey = lastKey = 0;
}

void TSlider::Pulse()
{
	if (firstKey && system_time() - lastKey > 1500000)
		SetValueFromKeys();
}

void TSlider::SetValue(double d)
{
	int32 v = (int32) rint((d - mOffset) / mScale); 
	SetValue(v);
}
