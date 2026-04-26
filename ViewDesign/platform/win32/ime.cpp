#include "ViewDesign/platform/win32/ime.h"
#include "ViewDesign/platform/win32/string.h"


namespace ViewDesign {

namespace Win32 {

namespace {

u16string ime_string;
size_t cursor_position;

} // namespace


void ImeEnable(HWND hwnd) {
	ImmAssociateContextEx(hwnd, nullptr, IACE_DEFAULT);
}

void ImeDisable(HWND hwnd) {
	ImmAssociateContextEx(hwnd, nullptr, 0);
}

void ImeSetPosition(HWND hwnd, Point point) {
	HIMC imc = ImmGetContext(hwnd);
	CANDIDATEFORM cf = { 0, CFS_CANDIDATEPOS, { (int)point.x, (int)point.y }, {} };
	ImmSetCandidateWindow(imc, &cf);
	ImmReleaseContext(hwnd, imc);
}

void ImeUpdateString(HWND hwnd, uint type) {
	HIMC imc = ImmGetContext(hwnd);
	if (type & GCS_COMPSTR) {
		LONG size = ImmGetCompositionStringW(imc, GCS_COMPSTR, nullptr, 0);
		ime_string.resize(size / sizeof(u16char));
		ImmGetCompositionStringW(imc, GCS_COMPSTR, ime_string.data(), size);
	}
	if (type & GCS_RESULTSTR) {
		LONG size = ImmGetCompositionStringW(imc, GCS_RESULTSTR, nullptr, 0);
		ime_string.resize(size / sizeof(u16char));
		ImmGetCompositionStringW(imc, GCS_RESULTSTR, ime_string.data(), size);
	}
	if (type & GCS_CURSORPOS) {
		cursor_position = ImmGetCompositionStringW(imc, GCS_CURSORPOS, NULL, 0);
	}
	ImmReleaseContext(hwnd, imc);
}


u16string ImeGetString() { return std::move(ime_string); }
size_t ImeGetCursorPosition() { return cursor_position; }


} // namespace Win32

} // namespace ViewDesign
