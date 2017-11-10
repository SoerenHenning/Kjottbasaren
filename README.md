# Computer Graphics Project

Developed by Sören Henning and Zach Litzinger

## Features

* Moving in x, y and z direction
* Horizontal and vertical rotation
* Direction sunlight
* Headlight on top of the camera, modelled as a point light source with decreasing intensity
* Perspective and orthographic projection (no depth information in orthographic projection due to fact how orthographic projection is)
* Per fragment, per vertex and black and white shading

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

`v`: Toggle automatic rotation of models

`k`: Change texture intensity (0% - 100%)

`i`: Print info about camera status

`y`: Toggle sunlight

`x` :Toggle headlight

`1`: Switch to realistic shading (*per vertex illumination*)

`2`: Switch to foggy shading (*per vertex illumination*)

`3`: Switch to black and white shading (*per vertex illumination*)

`4`: Switch to linocut shading (*per vertex illumination*)

`5`: Switch to noise shading (*per vertex illumination*)

`6`: Switch to vignette shading (*per vertex illumination*)

`7`: Switch to normal highlight shading (useful for debugging) (*per vertex illumination*)

`l`: Reload shaders

`p`: Set drawing to wireframes

`o`: Set drawing to solid faces

`q`: Quit
