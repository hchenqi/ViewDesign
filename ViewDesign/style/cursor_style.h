#pragma once

#include "ViewDesign/common/type.h"


namespace ViewDesign {


enum class Cursor : uint {
	Default = 1,
	Hide = 0,
	Arrow = 32512,			// IDC_ARROW
	Hand = 32649,			// IDC_HAND
	Text = 32513,		    // IDC_IBEAM
	Move = 32646,			// IDC_SIZEALL
	Help = 32651,			// IDC_HELP
	Crosshair = 32515,		// IDC_CROSS
	Wait = 32514,			// IDC_WAIT
	Progress = 32650,		// IDC_APPSTARTING
	No = 32648,				// IDC_NO
	ResizeWE = 32644,		// IDC_SIZEWE
	ResizeNESW = 32643,		// IDC_SIZENESW
	ResizeNS = 32645,		// IDC_SIZENS
	ResizeNWSE = 32642,	    // IDC_SIZENWSE
};


} // namespace ViewDesign
