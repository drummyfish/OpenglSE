implemented features:

- multiplatform C++ 3D engine based on OpenGL
- extremely easy to use (single C++ header file)
- basic keyboard input handling
- advanced input handling (keyboard up and down events)
- possibility to automatically generate mesh normals
- 3D triangle mesh rendering, supports normals and texture coordinates for each vertex
- per-mesh rendering modes: no shading (good for skyboxes etc.), Goraud (per-vertex) shading, Phong (per-pixel) shading, wireframe
- basic ligting (one global directional light)
- simple material variables (have impact on lighting of the mesh)
- basic primitive generation: box, sphere, plane, cyllinder, cone
- texture loading from PPM file format
- interface to modify the texture pixel data
- 3D mesh loading from OBJ file format
- mesh and texture saving to OBJ and PPM
- basic camera handling (with just one function call) implemented
- simple texturing (one texture coordinate for each vertex)
- simple texture transparency (each pixel can be either fully transparent or fully visible)
- simple camera management (parametrised perspective, possibility to rotate and move the camera forward/backward etc.)
- simple time measurement
- terrain generation based on provided heightmap image
- fog
- FPS measurement
- basic automatic texture mapping
- skybox support
- possibility to turn off fog for specific objects (e.g. sky box)
- instancing (sharing mesh data on GPU)
- texture layering (mesh with 2 textures, vertices have specified weight to blend between tham, for example for terrain)
- basic mesh operations (vertex and mesh merging, decreasing polygon count etc.)
- basic key-frame vertex-blend based animations (possibility to load each keyframe from one OBJ file), interpolating or just switching between the frames
- very simple shadows (blobs underneath objects)

to-do:
- mouse input handling
- LOD (multiple meshes being switched depending on the distance)
- text rendering
- 2D image rendering
- billboarding (2D sprites)
- generating volumetric meshes from 3D arrays of data
- simple collision detection
- simple physics

instalation:
- install GLEW and FREEGLUT:
  - Fedora: yum install glew-devel freeglut-devel
- include openglse.hpp in your sourcecode and use namespace gl_se
- link with GCC:
  - on Windows add these flags: -lfreeglut -lglew32s -lopengl32
  - on Linux add these flags: -lfreeglut -lglew32s -lopengl32
