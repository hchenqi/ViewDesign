#include "ViewDesign/platform/win32/cursor.h"


namespace ViewDesign {

namespace Win32 {


HCURSOR CreateCursorFromPixelBuffer(const PixelBuffer& pixel_buffer, std::pair<uint, uint> hotspot) {
	auto [width, height] = pixel_buffer.Size();
	auto [x, y] = hotspot;

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* dibBits = nullptr;
	HDC hdc = GetDC(nullptr);

	HBITMAP hColor = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &dibBits, nullptr, 0);

	ReleaseDC(nullptr, hdc);

	if (!hColor) {
		return nullptr;
	}

	memcpy(dibBits, pixel_buffer.Pixels().data(), width * height * 4);

	HBITMAP hMask = CreateBitmap(width, height, 1, 1, nullptr);

	ICONINFO ii = {};
	ii.fIcon = FALSE;
	ii.xHotspot = x;
	ii.yHotspot = y;
	ii.hbmColor = hColor;
	ii.hbmMask = hMask;

	HCURSOR hCursor = CreateIconIndirect(&ii);

	DeleteObject(hColor);
	DeleteObject(hMask);

	return hCursor;
}


} // namespace Win32

} // namespace ViewDesign
