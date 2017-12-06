# Kjøttbasaren

A simple 3D scene of Bergen, Norway and the Kjøttbasaren with free and automatic navigation. Starting takes some time due to the large terrain file.

Developed by Sören Henning and Zach Litzinger

## Features

* 3D Terrain of the city Bergen
* Model of the Kjøttbasaren building
* Manual navigaton in x, y and z direction
* Automatic navigation using composite Bezier curves
* Horizontal and vertical rotation
* Directional sunlight (on/off switch)
* Headlight on top of the camera, modelled as a point light source with decreasing intensity (on/off switch)
* Perspective and orthographic projection
* Per fragment shading with various effects (fog, black and white, linocut (similiar to toon), noise, vignette, normal highlighting, terrain colors per height)
* Adjustable mix of material properties and textures
* Objects imported as .obj files
* Textures and material properties imported from .mtl files
* Skybox included

## Navigation

### Mouse

Dragging with left mouse button: Rotating

Dragging with middle mouse button: Zooming in and out

Dragging with right mouse button: Move forward, right, left and backwards

### Keyboard

`w`, `s`: Move forward and backwards (in viewing direction)

`a`, `d`: Move left and right

`c`, `space`: Move up and down

`m`, `n`: Decrease or increase field of view

`b`: Toggle projection between orthographic and perspective

`r`: Reset camera

`v`: Toggle automatic camera movement

`k`: Change texture intensity (0% - 100%)

`i`: Print info about camera status

`y`: Toggle sunlight

`x` :Toggle headlight

`1`: Switch to realistic shading (*per fragment illumination*)

`2`: Switch to foggy shading (*per fragment illumination*)

`3`: Switch to black and white shading (*per fragment illumination*)

`4`: Switch to linocut shading (*per fragment illumination*)

`5`: Switch to noise shading (*per fragment illumination*)

`6`: Switch to vignette shading (*per fragment illumination*)

`7`: Switch to normal highlight shading (face normals are mapped to colors, useful for debugging) (*per fragment illumination*)

`8`: Switch to terrain height colors shading (*per fragment illumination*)

`l`: Reload shaders

`p`: Set drawing to wireframes

`o`: Set drawing to solid faces

`q`: Quit

## Building it
 * Visual Studio 2017 solution included
 * **Important:** Project has to be compiled for **x64** to load all objects properly
 * FreeGLUT and Glew for Windows included
 * Terrain Importer (transform to .obj file) located in `terrain-importer/main.kt`
