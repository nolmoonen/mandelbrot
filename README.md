![alt text](example.png?raw=true)

The fractal generation and visualization was created using [this](https://www.codingame.com/playgrounds/2358/how-to-plot-the-mandelbrot-set/mandelbrot-set) tutorial.

For Vulkan, [this](https://vulkan-tutorial.com/) tutorial was used.

## Building
* Clone [GLFW 3.3.2](https://github.com/glfw/glfw/releases/tag/3.3.2) source code into directory `external/glfw-3.3.2`.
* Install [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
* Build using CMake.

## Features
* Vulkan-powered window.
* Computations are done on a seperate thread to keep the window responsive.

## Controls
* Click and hold with LMB to make a selection.
    * Release to zoom in.
    * Click with RMB to cancel selection.
* Press backspace to zoom out.

## Todo
* Preemption to compute thread in the case of resizing, zooming in/out, closing application.
* Saving texture as bitmap.
* Full screen mode (for saving 1920x1080 textures).
* Color schemes.
