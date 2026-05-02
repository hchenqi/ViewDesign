#pragma once

#include "ViewDesign/event/key_event.h"

#include <GLFW/glfw3.h>


namespace ViewDesign {

namespace GLFW {


inline Key AsKey(int key) {
	if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z')) {
		return static_cast<Key>(key);
	}

	switch (key) {
	case GLFW_KEY_ESCAPE: return Key::Escape;
	case GLFW_KEY_SPACE: return Key::Space;
	}

	return Key::Undefined;
}


} // namespace GLFW

} // namespace ViewDesign
