# Faidworks Input Viewer

A native gamepad input viewer built with SFML and SDL3. Displays controller inputs in real time with customizable button layouts, colors, and fonts.

## Building

Requires CMake 3.10+. Dependencies (SFML 3, SDL3) are fetched automatically via CMake FetchContent.

### Linux

```sh
chmod +x ./build.sh
./build.sh
```

### Windows

Run from a Visual Studio Developer Command Prompt:

```bat
build-win.bat
```

Produces a statically linked exe at `build_win\src\Release\faidworks-input-viewer.exe` with no external DLL dependencies.

## Usage

Press **Alt** to toggle the navigation bar and switch between tabs.

### Main

Displays controller inputs in real time. Buttons light up with their configured active color when pressed, and triggers show analog fill.

![Main tab](resources/display.png)

### Layout

Drag elements to reposition them. The layout is saved automatically on exit.

![Layout tab](resources/layout.png)

### Settings

Configure input mappings, active style (filled/pressed), background color, per-button colors, and font.

![Settings tab](resources/settings.png)

## Configuration

Settings are stored in `~/.config/input-viewer/settings.txt` (Linux), `~/Library/Application Support/input-viewer/` (macOS), or `%APPDATA%/input-viewer/` (Windows).
