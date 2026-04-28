# ViewDesign

A C++ GUI library

## Introduction

**ViewDesign** is a GUI framework in C++, currently still under development. It is object-oriented, highly modular and flexible, featuring combination of components with frames and static layout compatibility check through C++ templating which are particularly helpful for designing complex UI components with minimal run-time overhead. It also provides a basic but easily extensible component library for intuitive and explicit UI building.

### Highlight

- written in modern C++, object-oriented and without any markup language
- lightweight, highly modular, flexible and extensible
- simple and clear logic for reflow and redraw
- static check of layout compatibility between components by size traits
- conceptual separation of components as control, frame and layout

### Limitation

- detailed documentation and cross-platform support in progress
- component library to be extended
- extensive correctness and performance testing needed
- hot reloading unsupported
- lack of a high-level and user-friendly designer for UI building
- no particular support for exception handling and multi-threading
- no support for accessibility and internationalization yet

### Comparison

#### Web Development with HTML/CSS/JavaScript

In web development, elements, styles and event handling are described separately by HTML, CSS and Javascript which can get distracting, complicate the development and affect the performance. In `ViewDesign`, they can be all implemented in pure C++ which directly compiles to machine code. One can separate the style definitions and main logic in different files at will without losing the integrity.

In web development, each element consists of padding, border and margin along with its content according to the CSS box model, which makes it easy to design and change the style of an element but also makes an element unnecessarily large to store all the style information. In `ViewDesign`, each view as a component is aimed to be minimal and lightweight. For example, a `TextBox` just displays text within a given size reference without any border, padding, background color or layout styles. It can however be combined with other components like `BorderFrame`, `PaddingFrame`, `MaxFrame`, `CenterFrame` to implement such additional attributes.

In web development, the layout styles of parent and child elements can be conflicting. For example, a parent element might have style `width: fix-content` while its child element has style `width: 100%`, which doesn't make sense. In `ViewDesign`, this scenario can be blocked at compile-time when parent view expects the width trait of child view to be `Auto` but child view's width is `Fixed`. This usually encourages developers to think about the sizing behaviour of a component and the compatibility between components. Nevertheless, there remains the possibility of building a general-purpose, style-guided and run-time calculated rich component like a HTML-element on top of the core library, while the core library remains minimal and modular.

#### Win32 Desktop Development

Traditional Win32 APIs treat each component as a window with different pre-defined styles through low-level system APIs which is not object-oriented and makes it difficult to build complex and custom UI components. `ViewDesign` internally relies on Win32 and DirectX APIs, nevertheless, for creating root-level windows and receiving system messages on Windows platform, but all components in a window are managed and drawn by the library itself.

#### Flutter

Flutter defines its own programming language and compiler just for multi-platform GUI development. `ViewDesign` is written in C++ and is open to various existing tool sets and custom design patterns.

#### Qt

Qt provides multiple frameworks and rich tool sets for cross-platform GUI and software development in general under a large community. `ViewDesign` is a single static, mostly header-based C++ library which is extremely lightweight without the need of a particular development environment but remains highly flexible and extensible.

Qt uses size policy to similarly describe how a widget is to be resized, but this is rather a suggestion that is performed by certain layout boxes at run-time, then a specification with compatibility check. In `ViewDesign`, size traits can not only be used to check and catch ill-combined layouts at compile-time, but also automatically select the compatible one from different implementations of common frames and layouts like `BorderFrame`, `ScrollFrame`, `SplitLayout` or `ListLayout` with help of C++ template deduction guide, making it seamless to build up view components with little effort.

Qt uses signals and slots mechanism historically for communication between objects that extends the C++ language. `ViewDesign` sticks to native C++ and provides multiple mechanisms for messaging like `Context` and `State` for developers to optionally choose or develop their own.

#### Dear ImGui

Dear ImGui allows for quick GUI development with simple commands of displaying components at each frame. But it is less modular or object-oriented, making it hard to build complex and layout-sensitive UI components.

### Summary

`ViewDesign` is an experimental and promising C++ GUI library that is not yet mature for production use, and some further effort is needed for cross-platform support. The compile-time layout compatibility check could be initially overwhelming and make it slightly more difficult for fast iterations. But due to its exceeding modularity and abstraction, `ViewDesign` is easily maintainable, modifiable, adaptive and extensible. It provides a new and clean approach for designing GUI applications, and is well-suited for learning purposes and for verifying prototypes and ideas.

## Example

A basic example program below displays "Hello World!" at the center of the main window: (from [Test/TextBoxTest.cpp](Test/TextBoxTest.cpp))

```cpp
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"

using namespace ViewDesign;

struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(u"Example");
	}
};

struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75).color(Color::Black);
	}
};

void App() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), u"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
```

We first include the header files for `Desktop` and the components we need. Then we extend and define the styles for `TitleBarWindow` and `TextBox`. Finally in the entrypoint `void App()`, we create the component instances, combine the components, add the main window and enter the event loop. `CenterFrame<Fixed, Fixed>` makes the `TextBox` compatible to `TitleBarWindow` and at the same time places it at the center of the main window.

The main window has a fixed initial size and a resizable border to change its size, thus expecting both the child view's width and height to be fixed. But a `TextBox` always determines its size based on its content, making its layout incompatible to the main window. This enforces one to specify where to put the `TextBox` in the main window since the `TextBox` doesn't have a fixed size.

We say that the main window accepts a child view with size trait `<Fixed, Fixed>`, meaning that both width and height of the child view should be fixed by the main window. A `TextBox` has size trait `<Relative, Relative>`, meaning both width and height of a `TextBox` is dependent on but not strictly assigned by its parent view. A `CenterFrame<Fixed, Fixed>` itself has size trait `<Fixed, Fixed>` whose width and height are assigned by its parent view, but accepts a child view with size trait `<Relative, Relative>` by always putting the child view at its center clipping the overflowing part of the child view. Without the `CenterFrame<Fixed, Fixed>`, the code below doesn't compile with a clear message `size traits incompatible` followed by detailed error information that helps developers to diagnose the incompatibility and clarify the intention.

```cpp
	new TitleBarWindow(
		MainWindowStyle(),
		new TextBox(TextBoxStyle(), u"Hello World!")
	)
```

One can replace `CenterFrame<Fixed, Fixed>` with `ClipFrame<Fixed, Fixed, TopLeft>` to make the text appear at the top-left corner of the window. If the text is too long, the overflowing part will also be clipped away.

```cpp
	// need to include "ViewDesign/view/frame/ClipFrame.h"

	new TitleBarWindow(
		MainWindowStyle(),
		new ClipFrame<Fixed, Fixed, TopLeft>(
			new TextBox(TextBoxStyle(), u"Hello World!")
		)
	)
```

One can use `create` (an alias of `std::make_unique`) instead of `new` to create component instances with strong exception-safe guarantee. Using `new` here is just a little simpler syntactically and in most cases safe because in the constructor functions of most components each argument will be immediately converted to a `view_ptr` parameter as `unique_ptr`. However, for components like `ListLayout`, `DivideLayout` and `StackLayoutMultiple` that accept variable number of arguments, passing raw pointers created by `new` is forbidden.

```cpp
	desktop.AddWindow(
		create<TitleBarWindow>(
			MainWindowStyle(),
			create<CenterFrame<Fixed, Fixed>>(
				create<TextBox>(TextBoxStyle(), u"Hello World!")
			)
		)
	);
```

UTF-16 strings are used across this project. Prefix `u` is used for UTF-16 string literals.

[`TitleBarWindow`](ViewDesign/view/widget/TitleBarWindow.h) is a more complex example itself as a pre-defined view component combined and extended from other components.

More examples can also be found in sub-folder [Test](Test).

## Build Instruction

The static library `ViewDesign` along with test executables can be built with CMake by various compilers targeting multiple backends cross-platform.

Tools to install:
- CMake: https://cmake.org/download/
- Ninja: https://ninja-build.org/ (recommended CMake generator for faster build)
- Visual Studio Code: https://code.visualstudio.com/ (recommended code editor)
	- CMake Tools (extension integrating CMake in VS Code)
- vcpkg: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started (recommended C++ package manager for installing and managing libraries)

The configuring and building of this library follows CMake routines. Possible base presets are specified in `CMakePresets.json` and can be inherited as in example `CMakeUserPresets.example.json`. One may create a copy and rename it to `CMakeUserPresets.json` for actual use.

`ViewDesign` can be included in a C++ project by CMake using `add_subdirectory` and `target_link_libraries`.

### Compiler

The following compilers can be used to build this project.

*Windows Target*

#### MSVC

- Install MSVC Build Tools (Visual Studio): https://visualstudio.microsoft.com/downloads/

#### Mingw-w64

- Install msys2: https://www.msys2.org/

Mingw-w64 can be used with GCC or Clang/LLVM.

> Mingw-w64 might use an older version of Windows SDK that doesn't include some headers like `icu.h`.

##### G++

Different packages need to be installed for x64 target and x86 target.

###### Target x64

- Install packages from msys2:
	- `pacman -S mingw-w64-ucrt-x86_64-gcc`
	- `pacman -S mingw-w64-ucrt-x86_64-gdb` (optional for debugging)
- Add `C:\msys64\ucrt64\bin` to path (or your installation directory)

###### Target x86

- Install packages from msys2:
	- `pacman -S mingw-w64-i686-gcc`
- Add `C:\msys64\mingw32\bin` to path (or your installation directory)

##### Clang/LLVM

With Clang/LLVM only x64 target is supported.

- Install packages from msys2:
	- `pacman -S mingw-w64-clang-x86_64-clang`
- Add `C:\msys64\clang64\bin` to path (or your installation directory)

#### Mingw-w64 (Linux host)

Mingw-w64 can also be used for cross-compiling windows applications on Linux host.

(Debian / Ubuntu)
- `sudo apt install g++-mingw-w64-x86-64`

*Linux Target*

### Platform

The following platform tools will be searched and included automatically in the static library when available.

#### Win32

Windows SDK is included in the compiler tool chains targeting Windows. No additional library needs to be installed.

#### OpenGL

OpenGL is available for major operating systems. No additional library needs to be installed.

#### GLFW

GLFW can be installed from vcpkg by `vcpkg install glfw3`.

GLFW (https://www.glfw.org/) is a cross-platform library for managing windows, interacting with OpenGL, Vulkan or other renderers and receiving input and events.

#### glad

glad can be installed from vcpkg by `vcpkg install glad`.

glad dynamically loads OpenGL functions as global variables that can be directly accessed. A function might be nullptr if it's not provided by OpenGL and should be checked before use.

#### Vulkan

Vulkan SDK can be installed from vcpkg by `vcpkg install vulkan` or its website https://vulkan.lunarg.com/.

#### ICU

ICU (International Components for Unicode) can be installed from vcpkg by `vcpkg install icu`.

### Backend

The following backends can be selected for building `ViewDesign`:
- Win32-DirectX
- GLFW-OpenGL (in progress)
- GLFW-Vulkan (in progress)

## Concepts

This section explains the basic concepts related with the view tree, layout, drawing and event handling.

### View / Desktop / Window

A view is a component derived from `ViewBase` defining its own logic of calculating layout, drawing content and handling events.

Each view can have a parent view and multiple child views. All views form a view tree.

`Desktop` is the virtual root of the view tree that manages all windows of the program on the system desktop. Each `Window` as a direct child view of `Desktop` presents one window and receives system events for this window.

`view_ref` holds a reference to a view, and `view_ptr` holds an allocated unique pointer to a view.

### Control / Frame / Layout

Control, frame and layout are different types of view components.

- A control has no child view and does basic functions or displays raw content, like `Button`, `TextBox`, `ImageBox`, etc.

- A frame has one child view to decorate, like `BorderFrame` and `BackgroundFrame`.

- A layout may have multiple child views that are visually arranged in a certain way, like `ListLayout`, `SplitLayout`, `OverlapLayout`, etc.

### Fixed / Auto / Relative

Each view occupies a rectangular region on its parent view. Usually the position of a view on its parent view is decided by the parent view completely, but the size of a view can be decided by both the view and its parent view.

The size traits `Fixed`, `Auto` and `Relative` marks how the width or the height of a view should be decided. This is not enforced as a general API but largely adopted by common view components.

- `Fixed` means a dimension of a view is assigned by its parent view.

- `Auto` means a dimension of a view is determined by the view itself.

- `Relative` means a dimension of a view is calculated by the view based on some information provided by its parent view.

`Fixed` and `Auto` are naturally also `Relative`, but not vice versa.

A view and a `view_ref` or a `view_ptr` can be marked by a certain size trait. And a parent view may only accept child views with certain size traits. This is checked at compile-time.

Some frames act as a conversion layer for converting size traits of child views:

- `ClipFrame`, `CenterFrame` and `ScrollFrame` convert a dimension from `Auto` or `Relative` to `Fixed`, clipping the overflowing part of the child view. `ClipFrame` puts the child view at a corner. `CenterFrame` puts the child view at the center. `ScrollFrame` makes it possible to scroll to the overflowing part of the child view.

- `FixedFrame` converts a dimension from `Fixed` to `Auto`. It sets a fixed value to the dimension for the child view.

- `MaxFrame` and `MinFrame` convert a dimension from `Relative` to `Auto`, normalizing its child view with a max or min size.

### Reflow / Redraw

Reflow is the process of propagating size change and calculating the layout of a view, determining the sizes and positions of its child views. Redraw is the process of propagating an updated region of a view and drawing its content including child views on the updated region.

As a general process regardless of size traits, the parent view provides a size reference `size_ref` to a child view, the child view calculates its size based on the `size_ref` or on its own, and returns its actual size to the parent view, which then calculates its layout and its own size. Some particular views like `OverlapLayout` also define their own mechanism of calculating layout.

If a view later changes its size by itself, it notifies its parent view, and the parent view calculates its layout again and might also change its own size. Reflow stops at the first parent view who updated its layout but doesn't change its own size. This final parent view then initiates redraw over its updated region.

A view whose content is updated without size change can also initiate redraw. A view who changes its size doesn't initiate redraw because the final parent view will.

A view initiating redraw notifies its parent view about its updated region, and the parent view translates and clips the region as its own region and notifies its own parent view until the `Window` which then draws all child views in the updated region and presents the window.

### Figure / Canvas / Layer / Texture / RenderTarget

`Figure` is the unit for drawing. A figure can draw itself on a backend-dependent `RenderTarget`. Different figure types like `Rectangle`, `Circle`, `TextBlock` and `Image` all inherit the abstract `Figure` base class.

A view can draw its content as figures on a `Canvas` provided to it by its parent view. It can transform the coordinates of the canvas relative to a child view and pass the canvas further for the child view to draw on.

A canvas is an abstract render target for views, which actually collects the figures provided by the views with their positions and transforms as drawing commands for rendering on a `Layer`.

A layer is where a canvas is rendered on. It holds a `Texture` which is a wrapper for the backend-dependent render target where each figure collected in the canvas draws itself on.

A `LayerFrame` is a view maintaining a layer in itself as a cache where its child views are directly drawn on. The layer can be further rendered as a figure on another layer.

A `Window` maintains a backend-dependent `WindowLayer` for rendering and presenting the window.

### MouseEvent / KeyEvent / FocusEvent

A `Window` receives mouse events from the system and dispatches the events as instances of `MouseEvent` which includes the event type, the mouse position and key statuses. The view that consumes the mouse event is selected by hit test.

During hit test, each view checks the mouse event and returns a child view that should further process the event, or itself if it will consume the event, or a `nullptr` when a view is not found. The mouse position in the event is translated to a point on the child view.

If a view to consume the mouse event is finally found, this view is tracked by `Desktop` in a stack, and all parent views of the view and the view itself will be sent a `MouseEnter` event as `FocusEvent`. This view in addition will be sent a `MouseOver` event as `FocusEvent` and then the mouse event. If a view to consume the mouse event is not found, then the last view that processed the mouse event is still tracked even though it won't be sent the mouse event.

If another view is to be tracked or to consume the next mouse event, the view tracked before will be sent a `MouseOut` event, and the parent views that not the parent views of the new view will be sent a `MouseLeave` event.

If a view acquires mouse capture, all subsequent mouse events will be directly translated and sent to this view.

A view can acquire focus to receive key events as instances of `KeyEvent`. This view is also tracked by `Desktop` in another stack, and all its parent views and itself will receive `FocusIn` event as `FocusEvent`, and the view itself will additionally receive `Focus` event. The view which acquired focus before will receive `Blur` event and its parent views that are not the parent views of the newly tracked view will receive `FocusOut` event.

### Backend / Platform

Backend implements common interfaces, and a user programs is unaware by which backend these interfaces are implemented. For each build there's only one backend to be chosen at configure time.

Platform implements platform-specific interfaces under its own namespace. Multiple platforms that are compatible to the current OS can be available for user programs in one build.

## Implementation

This section gives an overview of the implementation of `ViewDesign` in accordance with the source code structure.

### Common

The `common` sub-folder keeps the core type definitions and utility functions.

#### ref_ptr / owner_ptr

This project tries to avoid using raw pointers and prefers `std::unique_ptr` or `std::reference_wrapper` in most scenarios. But raw pointers are still useful sometimes for nullable references or holding special resources, and for accepting constructed view objects by the simpler `operator new`. Therefore, the two aliases for `T*`, `ref_ptr<T>` and `owner_ptr<T>` are defined in this project as a coding convention to mark and distinguish the usage of a raw pointer. As aliases, there is no safety check for these pointers and it is advised to use them only in a controlled manner.

#### u16char / u16string

In alignment with ICU, UTF-16 strings are used in this project. `u16char` is an alias of `char16_t` and `u16string` an alias of `std::u16string`.

Converters between `std::u8string` and `std::u16string` are provided. `reinterpret_cast` is used at platform boundaries for casting between `char16_t*` and `wchar_t*` on Windows, and between `char8_t*` and `char*`.

### Geometry

#### Point

#### Size

#### Rect

#### Vector

#### Scale

#### Transform

### Drawing

#### Layer

#### Canvas

#### Figure

##### Shape

##### TextBlock

##### Image

##### Layer

### Event

#### KeyEvent

#### MouseEvent

#### FocusEvent

### System

#### Cursor

#### Clipboard

### Style

### View

#### ViewBase

##### Style

##### Parent / Child

##### Layout

##### Drawing

##### Event

#### ViewFrame

#### SizeTrait

#### view_ref / view_ptr

#### Window / Desktop

### Backend

#### Geometry

#### Rendering

#### Event

#### Window

### Messaging

#### Context

#### State

### Platform

### View Components

#### Control

##### Placeholder

##### TextBox

##### EditBox

#### Frame

##### MutableFrame

##### ClipFrame / CenterFrame

##### ScrollFrame

##### FixedFrame

##### MaxFrame / MinFrame

##### BorderFrame / PaddingFrame

##### BackgroundFrame / InnerBorderFrame

##### ScaleFrame

##### LayerFrame

#### Layout

##### SplitLayout

##### StackLayout

##### ListLayout

##### DivideLayout

##### OverlapLayout

#### Wrapper

##### Cursor

##### Background

##### HitTestHelper

##### DelayedUpdate

#### Widget

##### Tooltip

##### TitleBarWindow

## Known Issues

- TextBox/EditBox don't well support vertical text layout (the text layout direction is related with the choice of ClipFrame)
- Tooltip might display an additional thin border in background color (because the region of a window is rounded up to integer coordinates and background is drawn without clipping)

## History

This project spans several years of development under a former name `WndDesign`. The design principle and the coding style has shifted largely after multiple initial rewrites until reaching its current pattern at the year of 2021, and was since then under maintenance and further underwent several major refactorings in the course of refinement without changing the core architectural design. So as to say, this current project is not completely built up from scratch, but rather a simplification from more complex previous implementations that are not satisfactory.

The very first version sticks to Win32 API, without much object-oriented and modular design, even without proper version controlling with git. I was at an early stage of learning C++/STL and Win32 APIs and tried to implement basic data structures on my own and manually draw everything on bitmaps by manipulating the pixels. I had to stop at font and text rendering because of its huge complexity, and turned to using Direct2D and DirectWrite libraries.

The next two versions over-complicated the framework by having both a dynamic library and a static library, and enforcing every window (the former name for a view) to have a non-client region and styles for layout calculation of both client and non-client regions, making it difficult to check all scenarios of layout dependencies for abstracting a minimal interface. A queue for reflowing was used to store the windows whose size has changed, and the windows in the queue are then checked if their size change will affect other windows bottom-up by the order of the depth of the window on the tree, whose layouts are then finally updated top-down. It also tried to optimize drawing of large windows, introducing layers and tiling mechanism for every window, making it costly for creating a single window. Nevertheless, in the previous versions a lot of helpful ideas, structures and also critics were established, providing a strong basis and reference for designing the current and final version.

By adopting a lightweight view definition without non-client region and layer by default, and with the concept of size reference and frames, the current version abstracted the minimal logic for reflow, redraw and event handling, and became modular and elegant.
