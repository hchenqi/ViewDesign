#include "ViewDesign/platform/glfw/string.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

using namespace GLFW;


void SetClipboardText(u16string text) {
	glfwSetClipboardString(nullptr, as_char_str(to_u8string(text).c_str()));
}

void GetClipboardText(u16string& text) {
	if (const char* str = glfwGetClipboardString(nullptr); str) {
		text = to_u16string(u8string(as_u8char_str(str)));
	} else {
		text.clear();
	}
}


} // namespace ViewDesign
