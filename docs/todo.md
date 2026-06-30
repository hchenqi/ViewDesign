# To Do

- (feature) adding styled text controls like `TextButton`, `TextLabel`, `TextInput`, etc as widgets that have margin, border, padding, background, etc styled individually or based on themes and react to mouse hovering/clicking/focus events

- (feature) adding more common components like  `Slider`, `Progress`, `Scrollbar`, `ColorPicker`, `Chart`, `Menu`, `TableView`, `PanelView`, etc in the standard component library

- (feature) adding example `ControlGallery` that displays a collection of standard components

- (feature) adding example `MapView` that displays an infinitely large, scrollable and scalable map with multi-level tiled layers

- (improvement) `TextEditor`: supporting right-to-left/vertical text editing

- (improvement) implementing text layout engine for OpenGL/Vulkan backend (possibly with HarfBuzz and FreeType, or other existing 2D graphics libraries like Skia that can draw texts)

- (improvement) completing the documentation of the standard component library

- (bug) `Example/HotReload.cpp`: inconsistent GLFW mouse capture behaviour as Win32

- (bug) `Test/ListLayoutTest.cpp`: nothing drawn by `LayerFrameTiled` within `ScaleFrame` under certain large scale values with OpenGL/Vulkan backend

- (improvement) support customized window icon

- (improvement) handling desktop size change and per-monitor window behaviour

- (improvement) supporting mouse drag and drop of texts, files, etc from out of a window

- (feature) supporting fully keyboard-based interaction (possibly registering all components that are focusable in a separate set, but there needs a way to iterate over them)

- (improvement) handling device lost with Vulkan backend (supporting recreation of Vulkan Instance, Device, and device-dependent resources) (possibly simulating static initialization/destruction with respect to resource creation dependencies)

- (improvement) Vulkan supporting drawing with index buffers

- (feature) adding example `ViewEmbedding` that renders view components within 3D scenes

- (feature) supporting loading images directly from a URL

- (feature) implementing `TextView` with range-based font/paragraph formats for building a rich-text editor (possibly as separate components like `RichTextView`, `RichTextEditor`)

- (improvement) implementing integer value region arithmetic for non-Win32 platforms (possibly with Clipper2, Boost.Geometry, etc)

- (feature) making *ViewDesign* installable by CMake

- (feature) setting up GitHub Actions for automatically checking commits and pull-requests

- (questionable) `ViewBase`/`Desktop`: allowing a view component to initiate `SetTrack()`

- (questionable) adding other size traits (like `Bounded` for `TextView` and `MaxFrame`) to replace the ambiguous `Relative` trait

- (questionable) adding `OnMount()`/`OnDismount()` callbacks when a component is attached to/detached from the view tree
