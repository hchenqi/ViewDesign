# To Do

- (improvement) completing the documentation of the standard component library

- (feature) supporting mouse drag and drop of texts, files, etc from out of a window

- (feature) adding example `MapView` that displays an infinitely large, scrollable and scalable map with multi-level tiled layers

- (feature) adding example `ViewEmbedding` that renders view components within 3D scenes

- (feature) adding more common components like  `Slider`, `Progress`, `Scrollbar`, `ColorPicker`, `Chart`, `Menu`, `TableView`, `PanelView`, etc in the standard component library

- (feature) adding styled text controls like `TextButton`, `TextLabel`, `TextInput`, etc as widgets that have border, padding, background, etc and react to mouse hovering/clicking events

- (feature) adding example `ControlGallery` that displays a collection of standard components

- (improvement) Vulkan supporting drawing with index buffers

- (bug) `Example/HotReload.cpp`: inconsistent GLFW mouse capture behaviour as Win32

- (bug) `Test/ListLayoutTest.cpp`: nothing drawn by `LayerFrameTiled` within `ScaleFrame` under certain large scale values with OpenGL/Vulkan backend

- (improvement) handling device lost with Vulkan backend (supporting recreation of Vulkan Instance, Device, and device-dependent resources) (possibly simulating static initialization/destruction with respect to resource creation dependencies)

- (feature) adding stateful version for components like `TextView`, `TextEditor`, etc in the standard component library (similar to `Stateful::ImageView` and `ImageView`, with a derivation chain of states: `TextState`(text), `EditState`(caret, selection), `TemporaryEditState`(IME composition), `InputState`(focus, mouse, key) and using absolute timing with `Timer`)

- (feature) adding example `StateSharing`, where multiple views share the same state and each view can update the state that is synchronized with the others

- (feature) `TextEditor`: implementing undo/redo

- (improvement) `TextEditor`: supporting right-to-left/vertical text editing

- (feature) supporting loading images directly from a URL

- (improvement) implementing text layout engine for OpenGL/Vulkan backend (possibly with HarfBuzz and FreeType, or other existing 2D graphics libraries like Skia that draw texts)

- (improvement) implementing integer value region arithmetic for non-Win32 platforms (possibly with Clipper2, Boost.Geometry, etc)

- (feature) making *ViewDesign* installable by CMake

- (feature) setting up GitHub Actions for automatically checking commits and pull-requests

- (feature) `MirroringFrame::MirrorView`: possibly handling and redirecting mouse events to `MirroringFrame`

- (questionable) `ViewBase`/`Desktop`: allowing a view component to initiate `SetTrack()`

- (questionable) adding another size trait like `Bounded` for `TextView` and `MaxFrame` to replace the ambiguous `Relative` trait
