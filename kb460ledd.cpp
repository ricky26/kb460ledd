// kb460ledd.c
// Copyright (c) 2013 Richard Ian Taylor

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// This program tells X11 to tell the keyboard to set its LEDs to the state which
// X11 tells the program the LEDs are set to. This may seem somewhat tautological
// but the FUNC KB-460 doesn't update its LEDs when you press any of the lock keys!

// Put this in your startup programs in XFCE for make great profits!

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdint.h>
#include <iostream>
#include <memory>

//#define DEBUG

enum EKbdControlLED
{
	EKbdControlLED_ScrolLock = 3,
};

enum ELED
{
	ELED_ScrollLock 	= 1 << 0,
	ELED_CapsLock 		= 1 << 1,
	ELED_NumLock		= 1 << 4,
};
typedef unsigned int ELEDs;

enum EIndicator
{
	EIndicator_CapsLock		= 1 << 0,
	EIndicator_NumLock		= 1 << 1,
	EIndicator_ScrollLock	= 1 << 2,
};
typedef unsigned int EIndicators;

struct XDisplayFree
{
	inline void operator() (Display *_ptr)
	{
		XCloseDisplay(_ptr);
	}
};
typedef std::unique_ptr<Display, XDisplayFree> DisplayPtr;

static ELEDs indicatorsToLeds(EIndicators _inds)
{
	ELEDs ret = 0;

	if(_inds & EIndicator_CapsLock)
		ret |= ELED_CapsLock;
	if(_inds & EIndicator_NumLock)
		ret |= ELED_NumLock;
	if(_inds & EIndicator_ScrollLock)
		ret |= ELED_ScrollLock;
	
	return ret;
}

void setLeds(Display *_dpy, int _device, ELEDs _leds)
{
#ifdef DEBUG
	printf("Setting LEDS to %08x\n", _leds);
#endif

	XKeyboardControl values;
	values.led_mode = (_leds & ELED_ScrollLock) ? LedModeOn : LedModeOff;
    values.led = EKbdControlLED_ScrolLock;
    XChangeKeyboardControl(_dpy, KBLedMode, &values);

    XkbLockModifiers(_dpy, _device, ELED_CapsLock | ELED_NumLock,
        _leds & (ELED_CapsLock | ELED_NumLock) );
    XFlush(_dpy);
}

int main(int argc, char **argv)
{
	int xkbEventBase, xkbErrorBase;
	EIndicators indicators = 0;
	DisplayPtr dpy(XkbOpenDisplay(NULL, &xkbEventBase, &xkbErrorBase, NULL, NULL, NULL));

	if(!dpy)
	{
		std::cerr << "Failed to open X11 display!" << std::endl;
		return 1;
	}

	XkbSelectEvents(dpy.get(), XkbUseCoreKbd, XkbIndicatorStateNotifyMask, XkbIndicatorStateNotifyMask);

	// Set the initial LED state.
	XkbGetIndicatorState(dpy.get(), XkbUseCoreKbd, &indicators);
	setLeds(dpy.get(), XkbUseCoreKbd, indicatorsToLeds(indicators));

	for(;;)
	{
		XkbEvent event;
		XNextEvent(dpy.get(), &event.core);

		if(event.core.type == xkbEventBase+XkbEventCode)
		{
			if(event.any.xkb_type == XkbIndicatorStateNotify)
			{
				if(event.indicators.changed)
				{
					indicators = event.indicators.state;
					setLeds(event.indicators.display, event.indicators.device, indicatorsToLeds(indicators));
				}
			}
		}
	}

	return 0;
}
