#include "ViewDesign/event/ime.h"
#include "ViewDesign/platform/win32/ime.h"


namespace ViewDesign {


u16string Ime::GetString() { return Win32::ImeGetString(); }

size_t Ime::GetCursorPosition() { return Win32::ImeGetCursorPosition(); }


} // namespace ViewDesign
