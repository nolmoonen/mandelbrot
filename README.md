![alt text](example.png?raw=true)

The fractal generation and visualization was created using [this](https://www.codingame.com/playgrounds/2358/how-to-plot-the-mandelbrot-set/mandelbrot-set) tutorial.

For Vulkan, [this](https://vulkan-tutorial.com/) tutorial was used.

## Building
* Clone [GLFW](https://github.com/glfw/glfw) in directory `glfw`.
* Install [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).
* Build using CMake.

## Todo
* Save texture when zooming (invalidate stack when resizing).
* Deal with recreating texture while still resizing.
* Increasing maximum iterations over time.
