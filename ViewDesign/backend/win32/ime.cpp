#include "ViewDesign/backend/win32/ime.h"

#include <Windows.h>


namespace ViewDesign {

namespace {

std::wstring ime_string;
size_t cursor_position;

} // namespace


void ImeEnable(HANDLE hwnd) {
	ImmAssociateContextEx((HWND)hwnd, nullptr, IACE_DEFAULT);
}

void ImeDisable(HANDLE hwnd) {
	ImmAssociateContextEx((HWND)hwnd, nullptr, 0);
}

void ImeSetPosition(HANDLE hwnd, Point point) {
	HIMC imc = ImmGetContext((HWND)hwnd);
	CANDIDATEFORM cf = { 0, CFS_CANDIDATEPOS, { (int)point.x, (int)point.y }, {} };
	ImmSetCandidateWindow(imc, &cf);
	ImmReleaseContext((HWND)hwnd, imc);
}

void ImeUpdateString(HANDLE hwnd, uint type) {
	HIMC imc = ImmGetContext((HWND)hwnd);
	if (type & GCS_COMPSTR) {
		LONG size = ImmGetCompositionStringW(imc, GCS_COMPSTR, nullptr, 0);
		ime_string.resize(size / sizeof(wchar));
		ImmGetCompositionStringW(imc, GCS_COMPSTR, ime_string.data(), size);
	}
	if (type & GCS_RESULTSTR) {
		LONG size = ImmGetCompositionStringW(imc, GCS_RESULTSTR, nullptr, 0);
		ime_string.resize(size / sizeof(wchar));
		ImmGetCompositionStringW(imc, GCS_RESULTSTR, ime_string.data(), size);
	}
	if (type & GCS_CURSORPOS) {
		cursor_position = ImmGetCompositionStringW(imc, GCS_CURSORPOS, NULL, 0);
	}
	ImmReleaseContext((HWND)hwnd, imc);
}


std::wstring ImeGetString() { return std::move(ime_string); }

size_t ImeGetCursorPosition() { return cursor_position; }


} // namespace ViewDesign
