implemented features:

- multiplatform C++ 3D engine based on OpenGL
- extremely easy to use (single C++ header file)
- basic keyboard input handling
- advanced input handling (keyboard up and down events)
- possibility to automatically generate mesh normals
- 3D triangle mesh rendering, supports normals and texture coordinates for each vertex
- per-mesh rendering modes: no shading, Goraud (per-vertex) shading, Phong (per-pixel) shading, wireframe
- basic ligting (one global directional light)
- simple material variables (have impact on lighting of the mesh)
- basic primitive generating: box, sphere, plane, cyllinder, cone
- texture loading from PPM file format
- interface to modify the texture pixel data
- 3D mesh loading from OBJ file format
- basic camera handling (with just one function call) implemented
- simple texturing (one texture coordinate for each vertex)
- simple texture transparency (each pixel can be either fully transparent or fully visible)
- simple camera management (parametrised perspective, possibility to rotate and move the camera forward/backward etc.)
- simple time measurement

to-do:
- basic key-frame vertex-blend based animations (possibility to load each keyframe from one OBJ file)
- terrain generation based on provided heightmap image
- mouse input handling
- LOD (multiple meshes being switched depending on the distance)
- fog
- text rendering
- 2D image rendering
- billboarding (2D sprites)
- simple colision detection
- instancing (sharing mesh data on GPU)
- nearby vertices merging