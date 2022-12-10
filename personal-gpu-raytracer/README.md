# Projects 5 & 6: Lights, Camera & Action!

Overview:

In this project (Lights, Camera, Action!), I created a 3D rendering engine which converts .xml files describing scenes into 2D representations
of the 3D scenes. However, instead of using multivariable calculus to determine what an object primitive is, I generate the object
primitives as meshes of triangles using some trigonometry.

In this project, I utilize the OpenGL API to handle the rasterization and clipping. I wrote a vertex shader, in which object primitive
normals are converted to world space per each object, that passes positions into a fragment shader where Phong lighting is applied to them.

For each type of object, I store a VAO with a VBO that has been set up with normals and positions. These VBOs are reinitialized when the
parameter1 and parameter2 settings are changed, and then paintGL is called.

Within paintGL, I pass the array of light information into the shaders along with other scene data, then go through each shape, binding 
the appropriate VAO, and render each shape with its specific VAO and material color information. 3 different types of lights can be
rendered: directional, point (with light attenuation), and spot (with light attenuation and penumbra). Up to 8 lights can be supported per scene.

Prior to this process, I have calculated the camera position and view and perspective matrices, which are updated when the screen is resized
or the near or far plane are modified.

After this geometry rendering process, I have implemented 3 filtering functions: luma grayscale (per pixel) ("Per Pixel Filter"), inverse (per
pixel) ("Extra Credit 1"), and a 5-wide box blur ("Kernel-Based Filter"). Instead of the geometry and Phong rendering going directly to the
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

Known Bugs:
None

Extra Credit:
 - Extra per-pixel filter implemented (inverse)

Acknowledgements:

Thank you again to the TAs - this one was the least treacherous project since 2D projects, but Ed was still a great resouce!
