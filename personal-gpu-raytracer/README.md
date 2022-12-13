# GLSL GPU Raytracer

Overview:

In this project, I am writing a GPU raytracer, written with OpenGL (in GLSL). I use a fullscreen quad (2 triangles that cover the full screen) that
I pass as a VAO into a vertex shader, which only passes the info immediately into a fragment shader for each uv coordinate pair representing a pixel
in the screen. For each uv coordinate, I "shoot" a ray from the digital camera position through this coordinate point and trace it (as of right now)
through the list of object primitives (currently only spheres) defined by their multivariable representations (not polygonal meshse) to find intersections.
I then use Phong lighting and shadows to determine the color of that pixel, and trace the reflected ray to iterate this process 3 times (GLSL doesn't support
recursion). Thus, I am tracing all pixels. Also, lighting doesn't include an exception for a point-light existing in the middle of the scene.

After this geometry rendering process, I have implemented 3 filtering functions: luma grayscale (per pixel), inverse (per
pixel), and a 5-wide box blur. Instead of the geometry and Phong rendering going directly to the
screen, they are rendered to a framebuffer that stores depth and stencil information in a renderbuffer and color in a texture. After the
phong lighting, I pass the texture through the filter shaders onto a fullscreen quad, which then is rendered to the screen (default framebuffer).

IN ORDER TO CHANGE THE DEFAULT FRAMEBUFFER, CHANGE "m_defaultFBO" UNDER "CONSTANTS" IN "realtime.h".

More recently, I have implemented camera-moving functionality:
 - W Key: pushes the camera in (in the direction of the look vector)
 - S Key: pulls the camera out (away from the direction of the look vector)
 - A Key: pushes the camera left (in a direction perpendicular to the look and up vectors)
 - D Key: pushes the camera right (opposite the A Key direction)
 - Spacebar: pulls the camera up, along the worldspace vector (0,1,0)
 - Ctrl/Command (depending on OS): pulls the camera down, along the worlspace vector (0,-1,0)
 - Mouse movement: 
    - left-right movement: rotates about the (0,1,0) axis according to Rodrigues' formula, which also updates
        the look vector
    - up-down movement: rotates camera about axis perpendicular to up and look vectors
 
Every time a key is pressed, the above-specified rendering process happens repeatedly - for translations, the vector-distance
traveled is normalized and then multiplied by 5 divided by the amount of time the key has been pressed to guarantee movement 
happens at 5 worldspace units per second. For rotations, the speed has been altered arbitrarily (This can be modified by changing the
constant m_rotationSpeed under "CONSTANTS" in realtime.h).

Run instructions:
If running from QT, make sure to specify the run directory as the project directory and specify parameters inside program.
Else, just run the compiled project and specify parameters inside program.

Future work:
I would like to implement this on cones, cubes, and cylinders as well!

Known Bugs:
If a point light in the XML is placed in the middle of the scene, shadows might get weird. Tbh, I haven't tested it.

Acknowledgements:
This project is a culmination of my learning in CSCI 1230: Introduction to Computer Graphics at Brown University, taught by Prof. Daniel Ritchie in Fall of 2022. I
have learned so much and the class was honestly magical. I'm not submitting this code repo itself anywhere, but it will be factored into my final project.
