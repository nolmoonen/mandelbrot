![alt text](example.png?raw=true)

## Building
* Clone [GLFW 3.3.2](https://github.com/glfw/glfw/releases/tag/3.3.2) source code into directory `external/glfw-3.3.2`.
* Copy [stb/stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) into directory `external/stb`. 
**Note: this is a single header.** 
*   Clone [linmath.h](https://github.com/datenwolf/linmath.h) into directory `external/linmath`.
*   Clone [glad v0.1.33](https://github.com/Dav1dde/glad/releases/tag/v0.1.33) into directory `external/glad-0.1.33`.
* Build using CMake.

## Features
* Zoom in to the Mandelbrot fractal.
* Computations are done on a separate thread to keep the window responsive.

## Controls
* Click and hold with LMB to make a selection.
    * Release LMB to zoom in.
    * Click with RMB to cancel selection.
* Press backspace to zoom out.
* Press escape to close the application.

## Todo
* Preemption to compute thread in the case of resizing, zooming in/out, closing application.
* Saving texture as bitmap.
* Full screen mode (for saving 1920x1080 textures).
* Color schemes.
* Make logging thread-safe.
