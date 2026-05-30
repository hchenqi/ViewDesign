# Building ViewDesign

Tools to install:

(required)
- CMake: https://cmake.org/download/

(optional)
- Ninja: https://ninja-build.org/ (recommended CMake generator for faster build)
- vcpkg: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started (recommended C++ package manager for automatically installing dependent libraries)
- Visual Studio Code: https://code.visualstudio.com/ (recommended code editor)
  - CMake Tools (extension integrating CMake in VS Code)

This library can be configured and built using CMake presets:
- Clone or download the source code of this repository, open the repository root folder
- Create a copy of `CMakeUserPresets.example.json` and rename it to `CMakeUserPresets.json`
- (with CMake Tools extension in VS Code or Visual Studio):
  - Select the preset named with `VS2026 Windows MSVC x64 Debug (Backend: Win32-DirectX)` (or others)
  - Click `Build All Projects`
- (on the command line):
  - Run `cmake --preset vs2026-windows-msvc-x64-debug-win32-directx` (or choose other presets)
  - Run `cmake --build build/vs2026-windows-msvc-x64-debug-win32-directx`

Possible base presets are specified in `CMakePresets.json` and can be custom inherited in `CMakeUserPresets.json` as the example.

*ViewDesign* can be directly included in a CMake project with `add_subdirectory(path_to_ViewDesign_root)` and `target_link_libraries(AppName PRIVATE ViewDesign)`. The CMake cache variable `VIEWDESIGN_BACKEND` needs to be specified as one of the supported backends, either in the command line as `-DVIEWDESIGN_BACKEND=Win32-DirectX` or in the presets: (using `Win32-DirectX` backend for example)

```json
  "cacheVariables": {
    "VIEWDESIGN_BACKEND": "Win32-DirectX"
  }
```

Other CMake cache variables like `CMAKE_CXX_STANDARD`, `CMAKE_CXX_COMPILER`, `CMAKE_BUILD_TYPE`, etc are expected to be specified as well.

## Compiler

The following compilers can be used to build *ViewDesign*:

*Windows Target*

- MSVC: https://visualstudio.microsoft.com/downloads/
- Mingw-w64:
  - Install msys2: https://www.msys2.org/
  - Using G++ (Target x64):
    - Install from msys2: `pacman -S mingw-w64-ucrt-x86_64-gcc`
    - Install from msys2: `pacman -S mingw-w64-ucrt-x86_64-gdb` (optional for debugging)
    - Add `C:\msys64\ucrt64\bin` to path (or your installation directory)
  - Using G++ (Target x86):
    - Install from msys2: `pacman -S mingw-w64-i686-gcc`
    - Add `C:\msys64\mingw32\bin` to path (or your installation directory)
  - Using Clang/LLVM (only Target x64 supported):
    - Install from msys2: `pacman -S mingw-w64-clang-x86_64-clang`
    - Add `C:\msys64\clang64\bin` to path (or your installation directory)
- Mingw-w64 (Linux host):
  - Install G++: (Debian / Ubuntu) `sudo apt install g++-mingw-w64-x86-64`

> Mingw-w64 might use an older version of Windows SDK that doesn't include the header `icu.h`. And there might be build or installation issues with ICU.

> Mingw might not find the entry point provided by the library at link time. In this case, add the following link options for the targets:
> ```cmake
> if(MINGW)
>     target_link_options(${TARGET_NAME} PRIVATE -Wl,--whole-archive $<TARGET_LINKER_FILE:ViewDesign> -Wl,--no-whole-archive)
> endif()
> ```

*Linux Target*

- G++: (Ubuntu) `sudo apt install g++`

## Backend

The following backends can be selected for for configuring and building *ViewDesign*:
- `Win32-DirectX` (Windows)
- `Win32-OpenGL` (Windows)
- `Win32-Vulkan` (Windows)
- `GLFW-OpenGL` (Windows, Linux)
- `GLFW-Vulkan` (Windows, Linux)

Additional platform packages might be required for a backend. These packages can be installed from source, with vcpkg or the system package manager globally, or via the provided vcpkg manifest `vcpkg.json`.

> Backend / Platform:
> 
> Every backend has its implementation of a set of interfaces like windowing (Win32 / GLFW) and rendering (DirectX / OpenGL / Vulkan). For each set of interfaces there might be multiple backends available but only one backend is to be chosen at configure time.
>
> Platform here does not strictly mean the operating system, but rather packages that are available. A platform provides helper functions under its own namespace for backends and other platform-specific modules to use. All platforms that are visible in the current development environment will be eagerly included, though they might not be actually used.

## Platform

The following platform packages will be searched and included automatically.

### Win32 SDK (Already included in the compiler tool chain targeting Windows)

### OpenGL (Already available on major operating systems)

### ICU (International Components for Unicode, https://icu.unicode.org/)

> ICU is built with C++17 by default, which doesn't include some functions related with `std::u16string`, causing linker errors. It can be built and installed from [its source](https://github.com/unicode-org/icu/releases) with C++ standard specified as C++23:
> (current directory at `icu/source`)
> - Linux:
>   - Run `CXXFLAGS=-std=c++23 ./configure;` `make -j$(nproc);` `sudo make install;`
> - Windows:
>   - Run `msbuild allinone/allinone.sln /p:OverrideLanguageStandard=stdcpp23 /p:Configuration=Release /p:Platform=x64` (within Developer Command Prompt from Visual Studio)
> Set `ICU_ROOT` environment variable to `icu/` for cmake `find_package(ICU)` to be able to locate the built files. The `ICU_ROOT` can also be specified directly as a cmake cache variable like in `CMakePresets.json`.

### GLFW (https://www.glfw.org/)

> GLFW installed by `sudo apt install libglfw3-dev` (Ubuntu) might be in a lower version which doesn't include some definitions. It can be built and installed from [its source](https://github.com/glfw/glfw/releases) with CMake:
> (current directory at `glfw/`)
> - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
> - `cmake --build build -j$(nproc)`
> - `sudo cmake --install build`
>
> It is still recommended to use vcpkg manifest `vcpkg.json` for automatically installing GLFW and glad together.

> On Linux (WSL2), GLFW- backends currently don't work well with Wayland. Set environment variables by `export XDG_SESSION_TYPE=x11` or `export WAYLAND_DISPLAY=` before running the program to let GLFW choose x11 instead of Wayland.

### glad (https://glad.dav1d.de/)

> The easiest way to install glad is through vcpkg. The normal way requires generating and unpacking the source files manually.

### Vulkan (https://vulkan.lunarg.com/)

### stb (https://github.com/nothings/stb)

> The header-only library *stb* provides an image loader that is used for OpenGL and Vulkan backends.
