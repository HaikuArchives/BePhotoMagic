#ifndef __TSLIDER__
#define __TSLIDER__

#include <Slider.h>
#include <math.h>
#include <String.h>

class TSlider : public BSlider
{
public:
		TSlider(BRect frame,
				const char *name,
				const char *label,
				const char *format,
				BMessage *message,
				int32 minValue,
				int32 maxValue,
				float scale = 1.0,
				float offset = 0.0,
				thumb_style thumbType = B_TRIANGLE_THUMB,
				uint32 resizingMode = B_FOLLOW_LEFT |
									B_FOLLOW_TOP,
				uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_PULSE_NEEDED |
									B_FRAME_EVENTS)
			: BSlider(frame, name, label, message, minValue, maxValue, thumbType, resizingMode, flags),
			mPreviewInvoker(NULL), mFormat(format), mScale(scale), mOffset(offset) {}

		virtual	void SetValue(int32);
		virtual void SetValue(float f)			{SetValue((double) f);}
		virtual void SetValue(double d);
		virtual void DrawThumb();
		virtual	void KeyDown(const char *bytes, int32 numBytes);
		virtual void Pulse();
		virtual void ValueChanged();

		BInvoker* mPreviewInvoker;
protected:
		BString		mFormat;
		BString		number;
		float		mScale;
		float		mOffset;

private:
		void		ResetKeys();		
		void		SetValueFromKeys();		
		bigtime_t	lastKey;
		bigtime_t	firstKey;
};

#endif // __TSLIDER__
