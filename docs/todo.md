# To Do

- (improvement) implementing `DrawOn` for `RoundedRectangle` and `Ellipse` with Vulkan backend (possibly adding a new pipeline for dealing with arcs)

- (improvement) completing the documentation of the standard component library

- (feature) adding example `MapView` that displays an infinitely large, scrollable and scalable map with multi-level tiled layers

- (feature) adding example `ViewEmbedding` that renders view components within 3D scenes

- (feature) adding more components like `Slider`, `ColorPicker`, `Scrollbar`, `Chart`, `TableLayout`, `Menu`, etc in the standard component library

- (improvement) `Example/vulkan/SceneEmbedding.cpp`: adding a Vulkan `Texture` constructor supporting image depth for offscreen 3D rendering

- (improvement) Vulkan supporting drawing with index buffers

- (bug) `Example/HotReload.cpp`: inconsistent GLFW mouse capture behaviour as Win32

- (bug) `Test/ListLayoutTest.cpp`: nothing drawn by `LayerFrameTiled` within `ScaleFrame` under certain large scale values with OpenGL/Vulkan backend

- (improvement) handling device lost with Vulkan backend (supporting recreation of Vulkan Instance, Device, and device-dependent resources) (possibly simulating static initialization/destruction with respect to resource creation dependencies)

- (feature) adding stateful version for components like `TextView`, `TextEditor`, etc in the standard component library (possibly deriving the normal `TextEditor` from `Stateful::TextEditor`, with a derivation chain of states: `TextState`(text), `EditState`(caret, selection), `TemporaryEditState`(IME composition), `InputState`(focus, mouse, key) and using absolute timing with `Timer`)

- (feature) adding example `StateSharing`, where multiple views share the same state and each view can update the state that is synchronized with the others

- (feature) `TextEditor`: implementing undo/redo

- (improvement) `TextEditor`: supporting right-to-left/vertical text editing

- (improvement) supporting image loading for OpenGL/Vulkan backend (possibly with stb)

- (improvement) implementing text layout engine for OpenGL/Vulkan backend (possibly with HarfBuzz and FreeType)

- (improvement) implementing integer value region arithmetic for non-Win32 platforms (possibly with Clipper2, Boost.Geometry, etc)

- (feature) making *ViewDesign* installable by CMake

- (feature) setting up GitHub Actions for automatically checking commits and pull-requests

- (feature) `MirroringFrame::MirrorView`: possibly handling and redirecting mouse events to `MirroringFrame`

- (questionable) `ViewBase`/`Desktop`: allowing a view component to initiate `SetTrack()`
