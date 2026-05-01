#pragma once


namespace ViewDesign {

using Handle = void*;


namespace Win32 {


void SetWndStyle(Handle handle, int mask);

void SetWndTopMost(Handle handle);


} // namespace Win32

} // namespace ViewDesign
