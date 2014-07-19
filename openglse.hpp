/**
  Simple OpenGL graphic engine.

  Miloslav Èíž, 2014
*/
#ifndef OPENGLSE_H
#define OPENGLSE_H

#define GLEW_STATIC
#define PI 3.1415926535897932384626
#define PI_DIVIDED_180 0.01745329251

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

char shader_vertex[] =
"#version 330                                                 \n"
"layout (location = 0) in vec3 position;                      \n"
"layout (location = 1) in vec2 texture_coordination;          \n"
"layout (location = 2) in vec3 normal;                        \n"
"                                                             \n"
"uniform mat4 perspective_matrix;                             \n"
"uniform mat4 world_matrix;                                   \n"
"uniform mat4 view_matrix;                                    \n"
"uniform vec3 light_direction;                                \n"
"uniform vec3 camera_position;                                \n"
"uniform float ambient_factor;     // material variable       \n"
"uniform float diffuse_factor;     // material variable       \n"
"uniform float specular_factor;    // material variable       \n"
"uniform float specular_exponent;  // material variable       \n"
"uniform uint render_mode;                                    \n"
"uniform float fog_distance;                                  \n"
"uniform float far_plane;          // far plane distance      \n"
"                                                             \n"
"out vec2 uv_coordination;                                    \n"
"out float final_intensity;      // computed from lighting    \n"
"out vec3 transformed_normal;    // normal after object transformation \n"
"out vec3 transformed_position;                               \n"
"out float fog_intensity;        // 0 = maximum fog           \n"
"                                                             \n"
"float diffuse_intensity;                                     \n"
"float specular_intensity;                                    \n"
"vec3 reflection_vector;                                      \n"
"vec3 direction_to_camera;                                    \n"
"                                                             \n"
"void main()                                                  \n"
"{                                                            \n"
"  transformed_position = (world_matrix * vec4(position,1.0)).xyz;                                     \n"
"  gl_Position = perspective_matrix * view_matrix * vec4(transformed_position,1.0);                    \n"
"  uv_coordination = texture_coordination;                                                             \n"
"  transformed_normal = (world_matrix * vec4(normal, 0.0)).xyz;                                        \n"
"                                                                                                      \n"
"  if (fog_distance > 0) // fog enabled                                                                \n"
"    fog_intensity = clamp(pow((1.0 - gl_Position.z / far_plane) / (1.0 - fog_distance),2),0.0,1.0);   \n"
"  else                                                                                                \n"
"    fog_intensity = 1.0;                                                                              \n"
"                                                                                                      \n"
"  if (render_mode == uint(0)) {                                                                       \n"
"    diffuse_intensity = 1.0;                                                                          \n"
"    specular_intensity = 1.0; }                                                                       \n"
"  else {                                                                                              \n"
"    diffuse_intensity = clamp(dot(normalize(transformed_normal),-1 * light_direction),0.0,1.0);       \n"
"    reflection_vector = normalize(reflect(light_direction,transformed_normal));                       \n"
"    direction_to_camera = normalize(camera_position - transformed_position);                          \n"
"    specular_intensity = clamp(dot(direction_to_camera,reflection_vector),0.0,1.0);                   \n"
"    specular_intensity = clamp(pow(specular_intensity,specular_exponent),0.0,1.0);                    \n"
"    final_intensity = ambient_factor + diffuse_factor * diffuse_intensity + specular_factor * specular_intensity; } \n"
"}                                                            \n";

char shader_fragment[] =
"#version 330                                                 \n"
"in vec2 uv_coordination;                                     \n"
"in float final_intensity;                                    \n"
"in vec3 transformed_normal;                                  \n"
"in vec3 transformed_position;                                \n"
"out vec4 FragColor;                                          \n"
"in float fog_intensity;         // 0 = maximum fog           \n"
"                                                             \n"
"uniform sampler2D texture_unit;                              \n"
"uniform bool use_texture;         // whether to use texture or just color \n"
"uniform vec3 mesh_color;                                     \n"
"uniform vec3 light_direction;                                \n"
"uniform vec3 light_color;                                    \n"
"uniform vec3 camera_position;                                \n"
"uniform float ambient_factor;     // material variable       \n"
"uniform float diffuse_factor;     // material variable       \n"
"uniform float specular_factor;    // material variable       \n"
"uniform float specular_exponent;  // material variable       \n"
"uniform vec3 transparent_color;                              \n"
"uniform bool transparency_enabled;                           \n"
"uniform uint render_mode;                                    \n"
"uniform float fog_distance;                                  \n"
"uniform vec3 background_color;  // viewport background color \n"
"uniform bool use_fog;                                        \n"
"                                                             \n"
"vec3 transparent_color_difference;   // helper variable      \n"
"vec3 reflection_vector;                                      \n"
"float diffuse_intensity;                                     \n"
"float specular_intensity;                                    \n"
"float helper_intensity;                                      \n"
"vec3 direction_to_camera;                                    \n"
"                                                             \n"
"void main()                                                  \n"
"{                                                            \n"
"  if (use_texture)                                           \n"
"    FragColor = texture2D(texture_unit,uv_coordination.xy);  \n"
"  else                                                       \n"
"    FragColor = vec4(mesh_color,1.0);                        \n"
"                                                             \n"
"  if (render_mode == uint(2)) {                                                                               \n"
"    diffuse_intensity = clamp(dot(normalize(transformed_normal),-1 * light_direction),0.0,1.0);               \n"
"    reflection_vector = normalize(reflect(light_direction,transformed_normal));                               \n"
"    direction_to_camera = normalize(camera_position - transformed_position);                                  \n"
"    specular_intensity = clamp(dot(direction_to_camera,reflection_vector),0.0,1.0);                           \n"
"    specular_intensity = clamp(pow(specular_intensity,specular_exponent),0.0,1.0);                            \n"
"    helper_intensity = ambient_factor + diffuse_factor * diffuse_intensity + specular_factor * specular_intensity; } \n"
"  else                                                       \n"
"    helper_intensity = final_intensity;                      \n"
"                                                             \n"
"  gl_FragDepth = gl_FragCoord.z;                             \n"
"                                                             \n"
"  if (transparency_enabled) {                                \n"
"    transparent_color_difference = FragColor.xyz - transparent_color; \n"
"                                                                      \n"
"    if (abs(transparent_color_difference[0]) < 0.1 && abs(transparent_color_difference[1]) < 0.1 && abs(transparent_color_difference[2]) < 0.1)  // set the z coordination \n"
"      gl_FragDepth = 1.1;     // transparent color           \n"
"    }                                                        \n"
"                                                             \n"
"  FragColor = FragColor * vec4(helper_intensity,helper_intensity,helper_intensity,1.0) * vec4(light_color,1.0); \n"
"                                                             \n"
"  if (use_fog)                                               \n"
"    FragColor = mix(vec4(background_color,1.0),FragColor,fog_intensity); // apply fog \n"
"}                                                            \n";

typedef enum       /// special key codes
  {
    SPECIAL_KEY_UP = 256,
    SPECIAL_KEY_RIGHT,
    SPECIAL_KEY_DOWN,
    SPECIAL_KEY_LEFT,
  } special_keys;

typedef enum
  {
    RENDER_MODE_NO_LIGHT = 0,    /// triangles are filled but not shaded
    RENDER_MODE_SHADED_GORAUD,   /// triangles are filled and Goraud shaded (faster than Phong)
    RENDER_MODE_SHADED_PHONG,    /// triangles are filled and Phong (per-pixel) shaded (slower but nicer)
    RENDER_MODE_WIREFRAME        /// Goraud shaded, drawn in wireframe
  } render_mode;

typedef enum
  {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_FORWARD,
    DIRECTION_BACKWARD
  } axis_direction;

typedef enum
  {
    ROTATION_XYZ,                 /// rotation around x first, then y and then z
    ROTATION_ZYX,                 /// rotation around z first, then y and then x
    ROTATION_ZXY,                 /// rotation around z first, then x and then y (standard for meshes)
    ROTATION_YXZ                  /// rotation around y first, then x and then z (standard for camera)
  } rotation_matrix_type;

typedef struct     /// a point in 3D space
  {
    float x;
    float y;
    float z;
  } point_3d;

typedef struct     /// a vertice in 3D space
  {
    point_3d position;
    float texture_coordination[2];
    point_3d normal;
  } vertex_3d;

typedef struct     /// a triangle in 3D space
  {
    unsigned int index1;
    unsigned int index2;
    unsigned int index3;
  } triangle_3d;

class texture_2d   /// represents a texture, it's x and y size must be power of 2
  {
    protected:
      unsigned int width;
      unsigned int height;
      unsigned char *data;  /// RGB color data
      bool transparency_enabled;
      unsigned char transparent_color[3];
      float transparent_color_float[3];

      GLuint to;            /// texture object handle

      void upload_texture_data();
        /**<
         Uploads the texture data to GPU.
         */

      unsigned int xy_to_linear(int x, int y);
        /**<
         Converts x,y coordinates to linear data offset.

         @param x x coordinations
         @param y Y coordinations
         */

    public:
      texture_2d();
        /**<
         Class constructor, initialises a new texture.
         */

      ~texture_2d();
        /**<
         Class destructor, frees all the memory.
         */

      void set_transparent_color(unsigned char red, unsigned char green, unsigned char blue);
        /**<
         Sets the color that will be transparent (if transparency is
         enabled).

         @param red amount of red in transparent color
         @param green amount of green in transparent color
         @param blue amount of blue in transparent color
         */

      void set_transparency(bool enabled);
        /**<
         Enabled or disabled transparency. If transparency is enabled,
         the transparent color will be rendered transparent.

         @param enabled if true, transparency will be enabled, otherwise
                disabled
         */

      bool transparency_is_enabled();
        /**<
         Checks if the texture has transparency enabled.

         @return true if transparency is enabled, false otherwise
         */

      void get_transparent_color(unsigned char *red, unsigned char *green, unsigned char *blue);
        /**<
         Gets the transparent color set for the texture.

         @param red in this variable the amount of red will be returned
         @param green in this variable the amount of green will be returned
         @param blue in this variable the amount of blue will be returned
         */

      void update();
        /**<
         Updates the texture (loads its data to the GPU).
         */

      void set_pixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
        /**<
         Sets the texture pixel color, to take effect, the update method must
         be called.

         @param x x coordination of the pixel
         @param y y coordination of the pixel
         @param red amount of red
         @param green amount of green
         @param blue amount of blue
         */

      void get_pixel(int x, int y, unsigned char *red, unsigned char *green, unsigned char *blue);
        /**<
         Gets the texture pixel color.

         @param x x coordination of the pixel
         @param y y coordination of the pixel
         @param red in this variable the amount of red will be returned
         @param green in this variable the amount of green will be returned
         @param blue in this variable the amount of blue will be returned
         */

      void get_transparent_color_float(float *red, float *green, float *blue);
        /**<
         Gets the transparent color set for the texture as three float
         values.

         @param red in this variable the amount of red will be returned
         @param green in this variable the amount of green will be returned
         @param blue in this variable the amount of blue will be returned
         */

      bool load_ppm(string filename);
        /**<
         Loads the texture from ppm file format.

         @param filename file to be loaded
         @return true if everything went OK, false otherwise
         */

      GLuint get_texture_object();
        /**<
         Returns the texture object handle;

         @return texture object handle
         */

      unsigned int get_width();
        /**<
         Gets the texture width;

         @param texture width in pixels
         */

      unsigned int get_height();
        /**<
         Gets the texture height;

         @param texture hright in pixels
         */
  };

class mesh_3d      /// a 3D mesh made of triangles
  {
    protected:
      GLuint vbo;          /// the mesh's virtual buffer object handle
      GLuint ibo;          /// the mesh's index buffer object handle
      point_3d position;
      point_3d rotation;   /// the object's rotation in angles, x, y and z are roll, pitch and yaw
      point_3d scale;
      bool use_fog;
      texture_2d *texture; /// the texture associated with the mesh
      unsigned int color[3];   /// RGB mesh color that's used when the mesh doesn't have any texture
      float color_float[3];    /// mesh RGB color in range <0,1>
      render_mode mesh_render_mode;         /// determines how the model will be rendered
      mesh_3d *instance_parent;             /// if this object is an instance of another mesh, this points to it

      float material_ambient_intensity;     /// in range <0,1>, affects how much ambient light is reflected
      float material_diffuse_intensity;     /// in range <0,1>, affects how much diffuse light is reflected
      float material_specular_intensity;    /// in range <0,1>, affects how much specular light is reflected
      float material_specular_exponent;

      float translation_matrix[4][4];
      float rotation_matrix[4][4];
      float scale_matrix[4][4];
      float transformation_matrix[4][4];  /// translation + rotation + scale

      void update_transformation_matrix();
        /**<
          Computes and updates the object's transformation matrix out of
          translation, rotation and scale matrices.
        */

    public:
      vector<vertex_3d> vertices;
      vector<triangle_3d> triangles;

      mesh_3d();
        /**<
         Class constructor.
        */

      ~mesh_3d();
        /**<
         Class destructor, frees all the object's memory.
         */

      bool load_obj(string filename);
        /**<
         Loads the mesh from obj file format.

         @param filename file to be loaded
         @return true if everything went OK, false otherwise
         */

      void set_render_mode(render_mode mode);
        /**<
         Sets the render mode for the mesh (determines how the mesh will
         be rendered).

         @param mode mode to be set
         */

      void flip_triangles();
        /**<
         Flips the vertex triangles and normals (so that they're facing
         the other way).
         */

      void set_use_fog(bool enable);
        /**<
         Sets whether the mesh should be affected by fog (if it's turned
         on) when being rendered;

         @param enable if true, the mesh will be affected by fog,
                otherwise not
         */

      void make_instance_of(mesh_3d *what);
        /**<
         Makes this mesh an instance of another mesh (that means that
         this mesh will use the other meshe's vertex data stored in GPU
         memory.) The vertex data of this mesh will be deleted by
         calling this method.

         @param what mesh of which this mesh will become an instance
         */

      void get_vbo_ibo(GLuint *vbo, GLuint *ibo);
        /**<
         Gets the VBO (vertex buffer object) and IBO (index buffer object)
         handles.

         @param vbo in this vriable VBO handle will be returned
         @param ibo in this vriable IBO handle will be returned
         */

      unsigned int vertex_count();
        /**<
         Gets the mesh vertex count.

         @return number of vertices of the mesh
         */

      unsigned int triangle_count();
        /**<
         Gets the mesh triangle count.

         @return number of triangles of the mesh
         */

      render_mode get_render_mode();
        /**<
         Returns the render mode set for the mesh.

         @return current render mode
         */

      void clear();
        /**<
         Restores the mesh to the state as if it was just initialised.
         */

      void set_color(unsigned char red, unsigned char green, unsigned char blue);
        /**<
         Sets the mesh's color (which will be used if no texture is specified
         for the mesh).

         @param red amount of red
         @param green amount of green
         @param blue amount of blue
         */

      void texture_map_plane(axis_direction direction, float plane_width, float plane_height);
        /**<
         Maps the texture coordinations of the vertices using planar mapping.

         @param direction direction of the plane normal
         @param plane_width width of the plane
         @param plena_height height of the plane
         */

      void set_lighting_properties(float ambient, float diffuse, float specular, float specular_exponent);
        /**<
         Sets the lighting properties that affect how the light is
         reflected by the mesh surface.

         @param ambient factor in range <0,1>, says how much ambient light is reflected
         @param diffuse factor in range <0,1>, says how much diffuse light is reflected
         @param specular factor in range <0,1>, says how much specular light is reflected
         @param specular_exponent defines a size of specular reflections
         */

      void set_texture(texture_2d *texture);
        /**<
         Sets given texture to be used with this mesh.

         @param texture texture to be set for the mesh
        */

      void set_position(float x, float y, float z);
        /**<
         Sets the mesh position in the world space.

         @param x new x position
         @param y new y position
         @param z new z position
        */

      void set_rotation(float x, float y, float z);
        /**<
         Sets the mesh rotation in the world space. The parameter
         values are in degrees.

         @param x new x rotation along x axis (roll)
         @param y new y rotation along y axis (pitch)
         @param z new z rotation along z axis (yaw)
        */

      void set_scale(float x, float y, float z);
        /**<
         Sets the mesh scale.

         @param x new scale in x direction
         @param y new scale in y direction
         @param z new scale in z direction
        */

      void get_position(point_3d *point);
        /**<
         Gets the object position.

         @param point in this variable the object position will be returned;
        */

      void get_rotation(point_3d *point);
        /**<
         Gets the object rotation.

         @param point in this variable the object rotation will be returned;
        */

      void get_scale(point_3d *point);
        /**<
         Gets the object scale.

         @param point in this variable the object scale will be returned;
        */

      void get_bounding_box(float *x0, float *y0, float *z0, float *x1, float *y1, float *z1);
        /**<
         Returns the model bounding box (in model space).

         @param x0 x coordination of the first point
         @param y0 y coordination of the first point
         @param z0 z coordination of the first point
         @param x1 x coordination of the second point
         @param y1 y coordination of the second point
         @param z1 z coordination of the second point
         */

      void update();
        /**<
         This should be called if the models vertices or triangles have
         changed to reupload the model to the GPU. There's no need to
         call this method when position, rotation or scale of the model
         have been changed. This method doesn nothing if the mesh is an
         instance of another mesh.
        */

      void draw();
        /**<
         Draws the model.
        */

      void smooth_normals();
        /**<
         Makes the mesh normals by computing a normal for each triangle
         and averaging them so the mesh will appear smooth.
         */

      void print_data();
        /**<
         Debugging purposes method, prints the model's vertices and
         triangles to stdout.
         */

      void add_vertex(float x, float y, float z, float texture_u, float texture_v, float normal_x, float normal_y, float normal_z);
        /**<
         Adds a new verice to the mesh.

         @param x x position of the vertice
         @param y y position of the vertice
         @param z z position of the vertice
         @param texture_u texture u coordination
         @param texture_v texture v coordination
         @param normal_x normal vector x, the normal will be automatically normalized
         @param normal_y normal vector y
         @param normal_z normal vector z
        */

      void add_triangle(unsigned int index1, unsigned int index2, unsigned int index3);
        /**<
         Adds a new triangle indices to the mesh.

         @param index1 first index of the triangle
         @param index2 second index of the triangle
         @param index3 third index of the triangle
        */

      void apply_matrix(float matrix[4][4]);
        /**<
         Applies a transformation matrix to all the vertices of the
         mesh. The multiplication is done in order point * matrix (so
         the point is a row vector, not column). Normals are also
         transformed.

         @param matrix transformation matrix to be applies
        */
  };

void draw_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  /**<
   Draws a pixel at given position in frame buffer.

   @param x x position in pixels
   @param y y position in pixels
   @param r amount of red
   @param g amount of green
   @param b amount of blue
   @param a amount of alpha
  */

void init_opengl(int *argc_pointer, char** argv, unsigned int window_width, unsigned int window_height, void (*draw_function)(void), const char *window_title);
  /**<
   Initialises OpenGL, should be before any other function call from
   this library.

   @param argc_pointer pointer to program argc
   @param argv program argv
   @param window_width rendering window width in pixels
   @param window_height rendering window height in pixels
   @param draw_function pointer to a function that will be called
          to render each frame, this function shouldn't call
          glutSwapBuffers or glClear, they will be called
          automatically
   @param window_title window title
  */

void set_perspective(float fov_degrees, float near, float far);
  /**<
   Sets the perspective for the rendering.

   @param fov field of view in degrees
   @param near near distance
   @param far far distance
  */

void register_keyboard_function(void (*function)(int key, int x, int y));
  /**<
   Registers given function to be called when key press (character or special)
   one occurs.

   @param function function to be registered (key is either the ASCII key code
          or a special_keys code, x and y are the mouse coordinations at the
          time when the key press occured)
  */

void register_advanced_keyboard_function(void (*function)(bool key_up, int key, int x, int y));
  /**<
   Registers given function to be called when key down or key up event
   occurs.

   @param function function to be registered (key is either the ASCII key code
          or a special_keys code, x and y are the mouse coordinations at the
          time when the key press occured, key_up indicates if the event was
          key press or key release)
   */

int get_time();
  /**<
   Gets the current time count.

   @return number of millisecends since the init function has been called
   */

void normalize_vector(point_3d *vector);
  /**<
   Normalizes given vector (so that it's size is equal to 1).

   @param vector vector to be normalized
   */

int get_frame_time_difference();
  /**<
   Gets the time difference of this frame and the previous one.

   @return time difference in milliseconds since the last frame
   */

float vector_length(point_3d vector);
  /**<
   Calculates a vector length.

   @param vector vector whose length will be returned
   @return vector length
   */

mesh_3d *make_cuboid(float side_x, float side_y, float side_z);
  /**<
   Makes a cuboid (box) mesh.

   @param side_x length of the x side
   @param side_y length of the y side
   @param side_z length of the z side
   @return instance of the cuboid mesh
   */

mesh_3d *make_sharp_cuboid(float side_x, float side_y, float side_z);
  /**<
   Makes sharp-edged cuboid (box) mesh. It consists of 6 faces that don't
   share vertices, so that the edges can be sharp and can have independent
   texture mapping, which is especially good for skyboxes etc.

   @param side_x length of the x side
   @param side_y length of the y side
   @param side_z length of the z side
   @return instance of the cuboid mesh
   */

mesh_3d *make_plane(float width, float height, unsigned int resolution_x, unsigned int resolution_y);
  /**<
   Makes a plane mesh.

   @param width plane width
   @param height plane height
   @param resolution_x number of squares in x direction
   @param resolution_y number of squares in y direction
   @return instance of the plane mesh
   */

mesh_3d *make_cone(float radius, float height, unsigned int sides);
  /**<
   Makes a cone mesh.

   @param radius cone base radius
   @param height cone height
   @param sides number of sides
   @return instance of the cone mesh
   */

mesh_3d *make_cylinder(float radius, float height, unsigned int sides);
  /**<
   Makes a cylinder mesh.

   @param radius cylinder base radius
   @param height cylinder height
   @param sides number of sides
   @return instance of the cylinder mesh
   */

mesh_3d *make_sphere(float radius, unsigned int height_segments, unsigned int sides);
  /**<
   Makes a sphere mesh.

   @param radius sphere radius
   @param height_segments vertical resolution
   @param sides number of sides of the sphere
   @return instance of the sphere mesh
   */

mesh_3d *make_terrain(float size_x, float size_y, float height, unsigned int resolution_x, unsigned int resolution_y, texture_2d *heightmap);
  /**<
   Makes a terrain mesh based on heightmap stored as image in texture.

   @param size_x x size of the mesh
   @param size_y y size of the mesh
   @param height terrain amplitude
   @param resolution_x x resolution of the terrain
   @param resolution_y y resolution of the terrain
   @param heightmap heightmap image, only red component is taken into account
   @return generated terrain mesh
   */

void set_global_light(point_3d direction, unsigned char red, unsigned char green, unsigned char blue);
  /**<
   Sets the global directional light parameters that will be used in
   shading process.

   @param direction light direction
   @param red amount of red in the light intensity
   @param green amount of green in the light intensity
   @param blue amount of blue in the light intensity
   */

void set_background_color(unsigned char red, unsigned char green, unsigned char blue);
  /**<
   Sets the background (and fog) color for the viewport.

   @param red amount of red
   @param green amount of green
   @param blue amount of blue
   */

void set_fog(float distance);
  /**<
   Sets the fog distance. The fog color is determined by the background
   color.

   @param distance distance of the fog measured from the far plane,
          negative value or zero turns the fog off
   */

// global variables:

unsigned int global_window_width, global_window_height;
void (*user_render_function)(void) = NULL;                         /// pointer to user specified loop render function
void (*user_keyboard_function)(int key, int x, int y) = NULL;      /// pointer to user specified keypress function
void (*user_advanced_keyboard_function)(bool key_up, int key, int x, int y) = NULL;
float global_fov, global_near, global_far;                         /// perspective parameters
float global_fog_distance;                                         /// at what distance from the far plane in view space the fog begins

unsigned char global_background_color[3];                          /// background color of the viewport
bool global_keyboard_state[512];                                   /// keeps the keyboard state (each ASCII character + special keys) for the advanced keyboard function
int global_previous_frame_time = 0;                                /// keep the time of the previous frame

point_3d global_light_direction;                                   /// global directional light direction vector
unsigned char global_light_color[3];                               /// global directional light RGB intensity

GLuint perspective_matrix_location;                                /// perspective matrix location
GLuint world_matrix_location;                                      /// world matrix location
GLuint view_matrix_location;                                       /// view matrix location
GLuint texture_unit_location;                                      /// texture matrix location
GLuint use_texture_location;
GLuint mesh_color_location;
GLuint camera_position_location;
GLuint light_direction_location;
GLuint light_color_location;
GLuint ambient_factor_location;
GLuint diffuse_factor_location;
GLuint specular_factor_location;
GLuint specular_exponent_location;
GLuint transparent_color_location;
GLuint transparency_enabled_location;
GLuint render_mode_location;
GLuint fog_distance_location;
GLuint use_fog_location;
GLuint far_plane_location;
GLuint background_color_location;

struct camera_struct                                                            /// represents a camera
{
  point_3d position;                   /// camera position
  point_3d rotation;                   /// camera rotation (around x, y and z axis in degrees)
  point_3d direction_forward_vector;   /// camera direction (forward) vector
  point_3d direction_left_vector;
  point_3d direction_up_vector;
  mesh_3d *skybox = NULL;              /// skybox, follows the camera movement (but not its rotation)

  float translation_matrix[4][4];
  float rotation_matrix[4][4];
  float transformation_matrix[4][4];   /// translation + rotation

  float movement_speed = 0.01;         /// camera movement speed (distance per millisecond) used by camera handling function
  float rotation_speed = 0.1;          /// camera rotation speed (angles per millisecond)
  bool limit_x_rotation = true;        /// this disallows camera handling function to rotate around x more than allowed (classic FPS behaviour)

  int key_go_forward = SPECIAL_KEY_UP;
  int key_go_backward = SPECIAL_KEY_DOWN;
  int key_go_left = SPECIAL_KEY_LEFT;
  int key_go_right = SPECIAL_KEY_RIGHT;
  int key_go_up = 'q';
  int key_go_down = 'e';
  int key_rotate_y_cw = 'd';
  int key_rotate_y_ccw = 'a';
  int key_rotate_x_cw = 'w';
  int key_rotate_x_ccw = 's';
  int key_rotate_z_cw = 'y';
  int key_rotate_z_ccw = 'x';

  void set_position(float x, float y, float z);

    /**<
     Sets the camera position to given point.

     @param x new camera x
     @param y new camera y
     @param z new camera z
    */

  void get_direction(point_3d *direction);
    /**<
     Gets the camera direction vector;

     @param direction in this variable the unit camera direction vector
            will be returned
     */

  void set_skybox(mesh_3d *what);
    /**<
     Sets a skybox, which will follow the camera movement (but not its
     rotation).

     @param what skybox to be set
     */

  void handle_fps();
    /**<
     Handles the camera depending on the current keyboard state and camera
     settings (speed, key mapping etc.). This is intended to be called inside
     the render function and only if advanced keyboard function is registered.
     The camera behaves the same way as in FPS.
     */

  void move(float dx, float dy, float dz);
    /**<
     Moves the camera relatively to its current position.

     @param dx by how much to move the camera in x
     @param dy by how much to move the camera in y
     @param dz by how much to move the camera in z
     */

  void rotate(float dx_angles, float dy_angles, float dz_angles);
    /**<
     Rotates the camera relatively to its current rotation.

     @param dx_angles by how many angles to rotate the camera around x
     @param dy_angles by how many angles to rotate the camera around y
     @param dz_angles by how many angles to rotate the camera around z
     */

  void set_rotation(float x, float y, float z);
    /**<
     Sets the camera rotation to given point.

     @param x new camera rotation around x
     @param y new camera rotation around y
     @param z new camera rotation around z
    */

  void get_position(point_3d *position);
    /**<
     Gets the current camera position.

     @param position in this variable the camera position will be returned
    */

  void get_rotation(point_3d *rotation);
    /**<
     Gets the current camera rotation.

     @param rotation in this variable the camera rotation in degrees will
          be returned
    */

  void go(axis_direction direction, float distance);
    /**<
     Moves the camera in direction depending on its rotation.

     @param direction direction in which the camera will move from its
            perspective
     @param distance how far the camera should go
     */

} camera;

//======================================================================
// private function definitions:
//======================================================================

void print_matrix(float matrix[4][4])
  /**<
    For debugging purposes, prints given matrix.

    @matrix matrix to be printed
  */

{
  unsigned int i,j;

  for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
        cout << matrix[i][j] << " ";

      cout << endl;
    }

  cout << endl;
}

//----------------------------------------------------------------------

unsigned int texture_2d::xy_to_linear(int x, int y)

{
  return (y * this->width + x) * 3;
}

//----------------------------------------------------------------------

int convert_glut_key(int glut_key)
  /**<
   Converts the GLUT key code to the library key code.
   */

{
  switch (glut_key)
    {
      case GLUT_KEY_UP: return SPECIAL_KEY_UP; break;
      case GLUT_KEY_RIGHT: return SPECIAL_KEY_RIGHT; break;
      case GLUT_KEY_DOWN: return SPECIAL_KEY_DOWN; break;
      case GLUT_KEY_LEFT: return SPECIAL_KEY_LEFT; break;
    }

  return 0;
}

//----------------------------------------------------------------------

void parse_obj_line(string line,float data[4][3])
  /**<
    Parses the data contained in one line of obj file format
    (e.g. "v 1.5 3 4.2" or "f 1/2 3/5 4/6 1/20").

    @param data in this variable the parsed data will be returned, the
           first index represents the element number (i.e. x, y, z for
           a vertex or one of the triangle indices) and the second index
           represents one of up to 3 shashed values (if the values are
           in format a/b/3), if any of the values is not present, -1.0
           is inserted.
   */

{
  line = line.substr(line.find_first_of(' '));  // get rid of the first characters

  unsigned int i,j;
  size_t position;
  bool do_break;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 3; j++)
      data[i][j] = -1.0;

  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 3; j++)
        {
          do_break = false;

          try
            {
              if (line.length() >= 1)
                data[i][j] = stof(line,&position);

              if (line[position] != '/')
                do_break = true;

              if (position + 1 <= line.length())
                line = line.substr(position + 1);
              else
                return;

              if (do_break)
                break;
            }
          catch (exception& e)
            {
            }
        }
    }
}

//----------------------------------------------------------------------

float angle_to_0_360(float angle)
  /**<
   Converts a value in angles so that it is in range <0,360>.

   @param angle angle to be converted
   @return corresponding angle in <0,360> range
   */

{
  if (angle > 360)
    {
      while (angle > 360)
        angle -= 360;

      return angle;
    }
  else if (angle < 0)
    {
      while (angle < 0)
        angle += 360;

      return angle;
    }

  return angle;
}

//----------------------------------------------------------------------

void multiply_matrices(float matrix_a[4][4], float matrix_b[4][4], float matrix_result[4][4])

{
  unsigned int i,j;

  for (j = 0; j < 4; j++)
    for (i = 0; i < 4; i++)
      matrix_result[i][j] =
        matrix_b[0][j] * matrix_a[i][0] +
        matrix_b[1][j] * matrix_a[i][1] +
        matrix_b[2][j] * matrix_a[i][2] +
        matrix_b[3][j] * matrix_a[i][3];
}

//----------------------------------------------------------------------

void cross_product(point_3d vector_a, point_3d vector_b, point_3d *point_result)

{
  point_result->x = vector_a.y * vector_b.z - vector_a.z * vector_b.y;
  point_result->y = vector_a.z * vector_b.x - vector_a.x * vector_b.z;
  point_result->z = vector_a.x * vector_b.y - vector_a.y * vector_b.x;
}

//----------------------------------------------------------------------

void multiply_vector_matrix(float vector[4], float matrix[4][4], float vector_result[4])

{
  unsigned int i;

  for (i = 0; i < 4; i++)
    vector_result[i] = vector[0] * matrix[i][0] + vector[1] * matrix[i][1] +
                       vector[2] * matrix[i][2] + vector[3] * matrix[i][3];
}

//----------------------------------------------------------------------

void loop_function()

  /**<
    This function is internally registered as a render function and calls
    the user render function.
  */

{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  user_render_function();
  global_previous_frame_time = glutGet(GLUT_ELAPSED_TIME);
  glutSwapBuffers();
}

//----------------------------------------------------------------------

void make_perspective_matrix(float fov_degrees, float near_plane, float far_plane, float matrix[4][4])

{
  float aspect_ratio = global_window_width / ((float) global_window_height);
  float range = near_plane - far_plane;
  float tan_half_fov = tanf(fov_degrees / 2.0 * PI_DIVIDED_180);

  matrix[0][0] = 1.0f / (tan_half_fov * aspect_ratio) ; matrix[1][0] = 0.0f; matrix[2][0] = 0.0f; matrix[3][0] = 0.0f;
  matrix[0][1] = 0.0f; matrix[1][1] = 1.0f / tan_half_fov; matrix[2][1] = 0.0f; matrix[3][1] = 0.0f;
  matrix[0][2] = 0.0f; matrix[1][2] = 0.0f; matrix[2][2] = (-1 * near_plane - far_plane) / (float) range; matrix[3][2] = 1.0;
  matrix[0][3] = 0.0f; matrix[1][3] = 0.0f; matrix[2][3] = 2.0f * far_plane * near_plane / (float) range; matrix[3][3] = 0.0f;
}

//----------------------------------------------------------------------

void make_rotation_matrix(float degrees_x, float degrees_y, float degrees_z, rotation_matrix_type type, float matrix[4][4])

{
  degrees_x *= PI_DIVIDED_180;  // convert to radians
  degrees_y *= PI_DIVIDED_180;
  degrees_z *= PI_DIVIDED_180;

  float sin_x = sin(degrees_x);
  float cos_x = cos(degrees_x);
  float sin_y = sin(degrees_y);
  float cos_y = cos(degrees_y);
  float sin_z = sin(degrees_z);
  float cos_z = cos(degrees_z);

  switch (type)
    {
      case ROTATION_XYZ:

        matrix[0][0] = cos_z * cos_y;
        matrix[1][0] = -1 * cos_y * sin_z;
        matrix[2][0] = sin_y;
        matrix[3][0] = 0;

        matrix[0][1] = sin_x * sin_y * cos_z + cos_x * sin_z;
        matrix[1][1] = -1 * sin_x * sin_y * sin_z + cos_x * cos_z;
        matrix[2][1] = -1 * sin_x * cos_y;
        matrix[3][1] = 0;

        matrix[0][2] = -1 * cos_x * sin_y * cos_z + sin_x * sin_z;
        matrix[1][2] = cos_x * sin_y * sin_z + sin_x * cos_z;
        matrix[2][2] = cos_x * cos_y;
        matrix[3][2] = 0;

        matrix[0][3] = 0;
        matrix[1][3] = 0;
        matrix[2][3] = 0;
        matrix[3][3] = 1;
        break;

      case ROTATION_ZYX:

        matrix[0][0] = cos_z * cos_y;
        matrix[1][0] = cos_z * sin_y * sin_x - sin_z * cos_x;
        matrix[2][0] = cos_z * sin_y * cos_x + sin_z * sin_x;
        matrix[3][0] = 0;

        matrix[0][1] = sin_z * cos_y;
        matrix[1][1] = sin_z * sin_y * sin_x + cos_z * cos_x;
        matrix[2][1] = sin_z * sin_y * cos_x - cos_z * sin_x;
        matrix[3][1] = 0;

        matrix[0][2] = -1 * sin_y;
        matrix[1][2] = cos_y * sin_x;
        matrix[2][2] = cos_y * cos_x;
        matrix[3][2] = 0;

        matrix[0][3] = 0;
        matrix[1][3] = 0;
        matrix[2][3] = 0;
        matrix[3][3] = 1;
        break;

      case ROTATION_ZXY:

        matrix[0][0] = cos_z * cos_y - sin_z * sin_x * sin_y;
        matrix[1][0] = -1 * sin_z * cos_x;
        matrix[2][0] = cos_z * sin_y + sin_z * sin_x * cos_y;
        matrix[3][0] = 0;

        matrix[0][1] = sin_z * cos_y + cos_z * sin_x * sin_y;
        matrix[1][1] = cos_z * cos_x;
        matrix[2][1] = sin_z * sin_y - cos_z * sin_x * cos_y;
        matrix[3][1] = 0;

        matrix[0][2] = -1 * cos_x * sin_y;
        matrix[1][2] = sin_x;
        matrix[2][2] = cos_x * cos_y;
        matrix[3][2] = 0;

        matrix[0][3] = 0;
        matrix[1][3] = 0;
        matrix[2][3] = 0;
        matrix[3][3] = 1;
        break;

      case ROTATION_YXZ:

        matrix[0][0] = cos_y * cos_z + sin_y * sin_x * sin_z;
        matrix[1][0] = -1 * cos_y * sin_z + sin_y * sin_x * cos_z;
        matrix[2][0] = sin_y * cos_x;
        matrix[3][0] = 0;

        matrix[0][1] = cos_x * sin_z;
        matrix[1][1] = cos_x * cos_z;
        matrix[2][1] = -1 * sin_x;
        matrix[3][1] = 0;

        matrix[0][2] = -1 * sin_y * cos_z + cos_y * sin_x * sin_z;
        matrix[1][2] = sin_y * sin_z + cos_y * sin_x * cos_z;
        matrix[2][2] = cos_y * cos_x;
        matrix[3][2] = 0;

        matrix[0][3] = 0;
        matrix[1][3] = 0;
        matrix[2][3] = 0;
        matrix[3][3] = 1;
        break;
    }

  unsigned int i,j;

  for (j = 0; j < 4; j++)        // get rid of negative zeroes, they mess up the shaders
    for (i = 0; i < 4; i++)
      if (matrix[i][j] == -0)
        matrix[i][j] = 0;
}

//----------------------------------------------------------------------

void make_translation_matrix(float x, float y, float z, float matrix[4][4])

{
  matrix[0][0] = 1;
  matrix[1][0] = 0;
  matrix[2][0] = 0;
  matrix[3][0] = 0;

  matrix[0][1] = 0;
  matrix[1][1] = 1;
  matrix[2][1] = 0;
  matrix[3][1] = 0;

  matrix[0][2] = 0;
  matrix[1][2] = 0;
  matrix[2][2] = 1;
  matrix[3][2] = 0;

  matrix[0][3] = x;
  matrix[1][3] = y;
  matrix[2][3] = z;
  matrix[3][3] = 1;
}

//----------------------------------------------------------------------

void make_scale_matrix(float x, float y, float z, float matrix[4][4])

{
  matrix[0][0] = x;
  matrix[1][0] = 0;
  matrix[2][0] = 0;
  matrix[3][0] = 0;

  matrix[0][1] = 0;
  matrix[1][1] = y;
  matrix[2][1] = 0;
  matrix[3][1] = 0;

  matrix[0][2] = 0;
  matrix[1][2] = 0;
  matrix[2][2] = z;
  matrix[3][2] = 0;

  matrix[0][3] = 0;
  matrix[1][3] = 0;
  matrix[2][3] = 0;
  matrix[3][3] = 1;
}

//----------------------------------------------------------------------

void make_identity_matrix(float matrix[4][4])

{
  unsigned int i,j;

  for (j = 0; j < 4; j++)
    for (i = 0; i < 4; i++)
      if (i == j)
        matrix[i][j] = 1;
      else
        matrix[i][j] = 0;
}

//----------------------------------------------------------------------

bool add_shader(GLuint shader_program, const char* shader_text, GLenum shader_type)

{
  GLuint shader_object = glCreateShader(shader_type);

  const GLchar* p[1];
  p[0] = shader_text;

  GLint lengths[1];
  lengths[0]= strlen(shader_text);

  glShaderSource(shader_object,1,p,lengths);
  glCompileShader(shader_object);

  GLint success;

  glGetShaderiv(shader_object,GL_COMPILE_STATUS,&success);

  if (!success)
    {
      GLchar log[1024];
      glGetShaderInfoLog(shader_object,sizeof(log),NULL,log);
      cerr << "Shader compile log: " << log << endl;
      return false;
    }

  glAttachShader(shader_program,shader_object);

  return true;
}

//----------------------------------------------------------------------

bool compile_shaders()

{
  GLuint shader_program = glCreateProgram();

  if (shader_program == 0)
    {
      cerr << "ERROR: could not create a shader program." << endl;
      return false;
    }

  if (!add_shader(shader_program,shader_vertex,GL_VERTEX_SHADER))
    {
      cerr << "ERROR: could not add a vertex shader program." << endl;
      return false;
    }

  if (!add_shader(shader_program,shader_fragment,GL_FRAGMENT_SHADER))
    {
      cerr << "ERROR: could not add a fragment shader program." << endl;
      return false;
    }

  GLint success = 0;

  glLinkProgram(shader_program);

  glGetProgramiv(shader_program,GL_LINK_STATUS,&success);

  if (success == 0)
    {
      cerr << "ERROR: could not link the shader program." << endl;
      return false;
    }

  glValidateProgram(shader_program);

  glGetProgramiv(shader_program,GL_VALIDATE_STATUS,&success);

  if (!success)
    {
      cerr << "ERROR: the shader program is invalid." << endl;
      return false;
    }

  glUseProgram(shader_program);

  perspective_matrix_location = glGetUniformLocation(shader_program,"perspective_matrix");
  world_matrix_location = glGetUniformLocation(shader_program,"world_matrix");
  view_matrix_location = glGetUniformLocation(shader_program,"view_matrix");
  texture_unit_location = glGetUniformLocation(shader_program,"texture_unit");
  use_texture_location = glGetUniformLocation(shader_program,"use_texture");
  mesh_color_location = glGetUniformLocation(shader_program,"mesh_color");
  light_direction_location = glGetUniformLocation(shader_program,"light_direction");
  light_color_location = glGetUniformLocation(shader_program,"light_color");
  ambient_factor_location = glGetUniformLocation(shader_program,"ambient_factor");
  diffuse_factor_location = glGetUniformLocation(shader_program,"diffuse_factor");
  specular_factor_location = glGetUniformLocation(shader_program,"specular_factor");
  camera_position_location = glGetUniformLocation(shader_program,"camera_position");
  specular_exponent_location = glGetUniformLocation(shader_program,"specular_exponent");
  transparent_color_location = glGetUniformLocation(shader_program,"transparent_color");
  transparency_enabled_location = glGetUniformLocation(shader_program,"transparency_enabled");
  render_mode_location = glGetUniformLocation(shader_program,"render_mode");
  fog_distance_location = glGetUniformLocation(shader_program,"fog_distance");
  far_plane_location = glGetUniformLocation(shader_program,"far_plane");
  background_color_location = glGetUniformLocation(shader_program,"background_color");
  use_fog_location = glGetUniformLocation(shader_program,"use_fog");

  return true;
}

//----------------------------------------------------------------------

void helper_special_function(int key, int x, int y)

{
  key = convert_glut_key(key);

  if (user_advanced_keyboard_function != NULL)
    {
      if (!global_keyboard_state[key])
        {
          global_keyboard_state[key] = true;
          user_advanced_keyboard_function(true,key,x,y);
        }
    }

  if (user_keyboard_function != NULL)
    {
      user_keyboard_function(key,x,y);
    }
}

//----------------------------------------------------------------------

void helper_keyboard_function(unsigned char key, int x, int y)

{
  if (user_advanced_keyboard_function != NULL)
    {
      if (!global_keyboard_state[key])
        {
          global_keyboard_state[key] = true;
          user_advanced_keyboard_function(true,key,x,y);
        }
    }

  if (user_keyboard_function != NULL)
    user_keyboard_function(key,x,y);
}

//----------------------------------------------------------------------

void helper_advanced_special_keyboard_function(int key, int x, int y)

{
  key = convert_glut_key(key);

  if (user_advanced_keyboard_function != NULL && global_keyboard_state[key])
    {
      global_keyboard_state[key] = false;
      user_advanced_keyboard_function(false,key,x,y);
    }
}

//----------------------------------------------------------------------

void helper_advanced_keyboard_function(unsigned char key, int x, int y)

{
  if (user_advanced_keyboard_function != NULL && global_keyboard_state[key])
    {
      global_keyboard_state[key] = false;
      user_advanced_keyboard_function(false,key,x,y);
    }
}

//----------------------------------------------------------------------

void mesh_3d::update_transformation_matrix()

{
  float helper_matrix[4][4];

  multiply_matrices(this->translation_matrix,this->rotation_matrix,helper_matrix);
  multiply_matrices(helper_matrix,this->scale_matrix,this->transformation_matrix);
}

//----------------------------------------------------------------------

void update_view_matrix()

  /**<
    Updates the view (camera) matrix in the shader.
  */

{
  multiply_matrices(camera.rotation_matrix,camera.translation_matrix,camera.transformation_matrix);
  glUniformMatrix4fv(view_matrix_location,1,GL_TRUE,(const GLfloat *)camera.transformation_matrix);
}

//----------------------------------------------------------------------

void texture_2d::upload_texture_data()

{
  glBindTexture(GL_TEXTURE_2D,this->to);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,this->data);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

//----------------------------------------------------------------------

void reshape_function(int width, int height)

{
  global_window_height = height;
  global_window_width = width;
  glViewport(0,0,width,height);
  set_perspective(global_fov,global_near,global_far);
}

//======================================================================
// public function definitions:
//======================================================================

void set_global_light(point_3d direction, unsigned char red, unsigned char green, unsigned char blue)

{
  float helper_color[3],helper_direction[3];

  global_light_direction.x = direction.x;
  global_light_direction.y = direction.y;
  global_light_direction.z = direction.z;

  normalize_vector(&global_light_direction);

  global_light_color[0] = red;
  global_light_color[1] = green;
  global_light_color[2] = blue;

  helper_color[0] = red / 255.0;
  helper_color[1] = green / 255.0;
  helper_color[2] = blue / 255.0;

  helper_direction[0] = global_light_direction.x;
  helper_direction[1] = global_light_direction.y;
  helper_direction[2] = global_light_direction.z;

  glUniform3fv(light_color_location,1,(const GLfloat *) helper_color);
  glUniform3fv(light_direction_location,1,(const GLfloat *) helper_direction);
}

//----------------------------------------------------------------------

void camera_struct::go(axis_direction direction, float distance)

{
  switch (direction)
    {
      case DIRECTION_FORWARD:
        this->move(camera.direction_forward_vector.x * distance,
                   camera.direction_forward_vector.y * distance,
                   camera.direction_forward_vector.z * distance);
        break;

      case DIRECTION_BACKWARD:
        this->go(DIRECTION_FORWARD,-1 * distance);
        break;

      case DIRECTION_LEFT:
        this->move(camera.direction_left_vector.x * distance,
                   camera.direction_left_vector.y * distance,
                   camera.direction_left_vector.z * distance);
        break;

      case DIRECTION_RIGHT:
        this->go(DIRECTION_LEFT,-1 * distance);
        break;


      case DIRECTION_UP:
        this->move(camera.direction_up_vector.x * distance,
                   camera.direction_up_vector.y * distance,
                   camera.direction_up_vector.z * distance);
        break;

      case DIRECTION_DOWN:
        this->go(DIRECTION_UP,-1 * distance);
        break;

      default:
        break;
    }
}

//----------------------------------------------------------------------

render_mode mesh_3d::get_render_mode()

{
  return this->mesh_render_mode;
}

//----------------------------------------------------------------------

void mesh_3d::set_lighting_properties(float ambient, float diffuse, float specular, float specular_exponent)

{
  if (ambient > 1.0)
    ambient = 1.0;
  else if (ambient < 0.0)
    ambient = 0.0;

  if (diffuse > 1.0)
    diffuse = 1.0;
  else if (diffuse < 0.0)
    diffuse = 0.0;

  if (specular > 1.0)
    specular = 1.0;
  else if (specular < 0.0)
    specular = 0.0;

  this->material_ambient_intensity = ambient;
  this->material_diffuse_intensity = diffuse;
  this->material_specular_intensity = specular;
  this->material_specular_exponent = specular_exponent;
}

//----------------------------------------------------------------------

bool texture_2d::transparency_is_enabled()

{
  return this->transparency_enabled;
}

//----------------------------------------------------------------------

void texture_2d::get_transparent_color(unsigned char *red, unsigned char *green, unsigned char *blue)

{
  *red = this->transparent_color[0];
  *green = this->transparent_color[1];
  *blue = this->transparent_color[2];
}

//----------------------------------------------------------------------

void texture_2d::get_transparent_color_float(float *red, float *green, float *blue)

{
  *red = this->transparent_color_float[0];
  *green = this->transparent_color_float[1];
  *blue = this->transparent_color_float[2];
}

//----------------------------------------------------------------------

mesh_3d *make_cuboid(float side_x, float side_y, float side_z)

{
  mesh_3d *result = new mesh_3d;

  double half_x, half_y, half_z;

  half_x = side_x / 2.0;
  half_y = side_y / 2.0;
  half_z = side_z / 2.0;

              //     x           y           z             u    v      normal   #
  result->add_vertex(-1 * half_x,-1 * half_y,half_z,       0,   0,  -1,-1,-1);  // 0
  result->add_vertex(half_x,     -1 * half_y,half_z,       1,   0,   1,-1,-1);  // 1
  result->add_vertex(-1 * half_x,half_y,     half_z,       0,   1,  -1, 1,-1);  // 2
  result->add_vertex(half_x,     half_y,     half_z,       1,   1,   1, 1,-1);  // 3

  result->add_vertex(-1 * half_x,-1 * half_y,-1 * half_z,  1,   1,  -1,-1, 1);  // 4
  result->add_vertex(half_x,     -1 * half_y,-1 * half_z,  0,   1,   1,-1, 1);  // 5
  result->add_vertex(-1 * half_x,half_y,     -1 * half_z,  1,   0,  -1, 1, 1);  // 6
  result->add_vertex(half_x,     half_y,     -1 * half_z,  0,   0,   1, 1, 1);  // 7

  result->add_triangle(0,1,2);
  result->add_triangle(1,3,2);
  result->add_triangle(3,5,7);
  result->add_triangle(3,1,5);
  result->add_triangle(5,4,6);
  result->add_triangle(6,7,5);
  result->add_triangle(6,4,2);
  result->add_triangle(4,0,2);
  result->add_triangle(6,2,3);
  result->add_triangle(6,3,7);
  result->add_triangle(1,0,4);
  result->add_triangle(5,1,4);

  result->update();

  return result;
}

//----------------------------------------------------------------------

mesh_3d *make_sharp_cuboid(float side_x, float side_y, float side_z)

{
  mesh_3d *result = new mesh_3d;

  double half_x, half_y, half_z;

  half_x = side_x / 2.0;
  half_y = side_y / 2.0;
  half_z = side_z / 2.0;

              //     x           y           z             u    v      normal   #
  // back:
  result->add_vertex(-1 * half_x,-1 * half_y,half_z,       0.75, 0.5,0,  0,-1);  // 0
  result->add_vertex(half_x,     -1 * half_y,half_z,       0.5,0.5,  0,  0,-1);  // 1
  result->add_vertex(-1 * half_x,half_y,     half_z,       0.75, 0,  0,  0,-1);  // 2
  result->add_vertex(half_x,     half_y,     half_z,       0.5,0,    0,  0,-1);  // 3

  result->add_triangle(0,1,2);
  result->add_triangle(1,3,2);

  // front:
  result->add_vertex(-1 * half_x,-1 * half_y,-1 * half_z,  0,   0.5, 0, 0, 1);  // 4
  result->add_vertex(half_x,     -1 * half_y,-1 * half_z,  0.25,0.5, 0, 0, 1);  // 5
  result->add_vertex(-1 * half_x,half_y,     -1 * half_z,  0,   0,   0, 0, 1);  // 6
  result->add_vertex(half_x,     half_y,     -1 * half_z,  0.25,0,   0, 0, 1);  // 7

  result->add_triangle(5,4,6);
  result->add_triangle(6,7,5);

  // left:
  result->add_vertex(-1 * half_x,-1 * half_y,-1 * half_z,  1,   0.5,-1, 0, 0);  // 8
  result->add_vertex(-1 * half_x,-1 * half_y,half_z,       0.75,0.5,-1, 0, 0);  // 9
  result->add_vertex(-1 * half_x,half_y,     -1 * half_z,  1,   0  ,-1, 0, 0);  // 10
  result->add_vertex(-1 * half_x,half_y,          half_z,  0.75,0,  -1, 0, 0);  // 11

  result->add_triangle(8,9,10);
  result->add_triangle(11,10,9);

  // right:
  result->add_vertex(half_x,    -1 * half_y,-1 * half_z,   0.25,0.5, 1, 0, 0);  // 12
  result->add_vertex(half_x,    -1 * half_y,half_z,        0.5, 0.5, 1, 0, 0);  // 13
  result->add_vertex(half_x,     half_y,     -1 * half_z,  0.25,0,   1, 0, 0);  // 14
  result->add_vertex(half_x,     half_y,          half_z,  0.5, 0,   1, 0, 0);  // 15

  result->add_triangle(12,14,13);
  result->add_triangle(15,13,14);

  // top:
  result->add_vertex(-1 * half_x,half_y,    -1 * half_z,   0,   1,   0, 1, 0);  // 16
  result->add_vertex(half_x,     half_y,    -1 * half_z,   0.25,1,   0, 1, 0);  // 17
  result->add_vertex(-1 * half_x,half_y,         half_z,   0,   0.5, 0, 1, 0);  // 18
  result->add_vertex(half_x,     half_y,         half_z,   0.25,0.5, 0, 1, 0);  // 19

  result->add_triangle(18,17,16);
  result->add_triangle(18,19,17);

  // bottom:
  result->add_vertex(-1 * half_x,-1 * half_y,-1 * half_z,  0.25,0.5, 0,-1, 0);  // 20
  result->add_vertex(half_x,     -1 * half_y,-1 * half_z,  0.5, 0.5, 0,-1, 0);  // 21
  result->add_vertex(-1 * half_x,-1 * half_y,     half_z,  0.25,1,   0,-1, 0);  // 22
  result->add_vertex(half_x,     -1 * half_y,     half_z,  0.5, 1,   0,-1, 0);  // 23

  result->add_triangle(22,20,21);
  result->add_triangle(23,22,21);

  result->update();

  return result;
}

//----------------------------------------------------------------------

mesh_3d *make_plane(float width, float height, unsigned int resolution_x, unsigned int resolution_y)

{
  mesh_3d *result = new mesh_3d;

  unsigned int i,j;
  float step_x,step_y,half_width,half_height;

  half_width = width / 2.0;
  half_height = height / 2.0;

  step_x = width / (float) resolution_x;
  step_y = height / (float) resolution_y;

  for (j = 0; j < resolution_y + 1; j++)
    for (i = 0; i < resolution_x + 1; i++)
      {
        result->add_vertex(step_x * i - half_width,step_y * j - half_height,0,i / (float) resolution_x,j / (float) resolution_y,0,0,-1);
      }

  for (i = 0; i < result->vertices.size() - resolution_x - 2; i++)
    {
      if (i % (resolution_x + 1) != 0 && (i - i / (resolution_x + 1)) % resolution_x == 0)
        continue;

      result->add_triangle(i,i + resolution_x + 1,i + 1);
      result->add_triangle(i + resolution_x + 2,i + 1,i + resolution_x + 1);
    }

  result->update();

  return result;
}

//----------------------------------------------------------------------

unsigned int texture_2d::get_width()

{
  return this->width;
}

//----------------------------------------------------------------------

unsigned int texture_2d::get_height()

{
  return this->height;
}

//----------------------------------------------------------------------

mesh_3d *make_terrain(float size_x, float size_y, float height, unsigned int resolution_x, unsigned int resolution_y, texture_2d *heightmap)

{
  mesh_3d *result;
  float matrix[4][4];
  unsigned int i,index;
  unsigned char r,g,b;
  int x,y;
  int indexes[4];

  result = make_plane(size_x,size_y,resolution_x,resolution_y);

  make_rotation_matrix(-90,0,0,ROTATION_XYZ,matrix);
  result->apply_matrix(matrix);

  index = 0;
  int countdown = -1;

  // change the topology of the plane to look better:

  for (index = 0; index < result->triangles.size(); index += 4)
    {
      indexes[0] = result->triangles[index].index1;
      indexes[1] = result->triangles[index].index2;
      indexes[2] = result->triangles[index].index3;
      indexes[3] = result->triangles[index + 1].index1;

      result->triangles[index].index2 = indexes[3];
      result->triangles[index + 1].index2 = indexes[0];

      if (resolution_x % 2 == 0)
        {
          if (countdown < 0)
            {
              countdown = resolution_x * 2;
              index += 2;
            }

          countdown -= 4;
        }
    }

  // set the height for each vertex:

  if (heightmap != NULL)
    for (i = 0; i < result->vertices.size(); i++)
      {
        x = ((result->vertices[i].position.x + size_x / 2.0) / size_x) * (heightmap->get_width() - 1);
        y = ((result->vertices[i].position.z + size_y / 2.0) / size_y) * (heightmap->get_height() - 1);

        heightmap->get_pixel(x,y,&r,&g,&b);

        result->vertices[i].position.y += r / 255.0 * height;
      }

  result->texture_map_plane(DIRECTION_DOWN,1.0,1.0);
  result->smooth_normals();

  result->update();

  return result;
}

//----------------------------------------------------------------------

mesh_3d *make_cone(float radius, float height, unsigned int sides)

{
  mesh_3d *result = new mesh_3d;

  result->add_vertex(0,0,0,0,0,0,-1,0);         // bottom
  result->add_vertex(0,height,0,0.5,1,0,1,0);   // top

  result->add_vertex(radius,0,0,0,0,1,0,0);   // first circle vertex

  unsigned int i;
  float helper_x,helper_z;
  float texture_step = 1.0 / (float) sides;
  float angle = 0;
  float angle_step = 2 * PI / (float) sides;

  for (i = 0; i < sides; i++)
    {
      angle += angle_step;
      helper_x = cos(angle) * radius;
      helper_z = sin(angle) * radius;

      if (i < sides - 1)
        {
          result->add_vertex(helper_x,0,helper_z,(i + 1) * texture_step,0,helper_x,0,helper_z);
          result->add_triangle(0,i + 2,i + 3);
          result->add_triangle(1,i + 3,i + 2);
        }
      else
        {
          result->add_triangle(0,i + 2,2);
          result->add_triangle(1,2,i + 2);
        }
    }

  result->update();

  return result;
}

//----------------------------------------------------------------------

mesh_3d *make_sphere(float radius, unsigned int height_segments, unsigned int sides)

{
  mesh_3d *result = new mesh_3d;

  result->add_vertex(0,-1 * radius,0,0.5,0.5,0,-1,0);  // bottom vertex
  result->add_vertex(0,radius,0,0.5,0.5,0,1,0);        // top vertex

  unsigned int i,j,first_row_index;
  float angle_step;
  float scale_factor;
  float x_coordination,y_coordination,z_coordination;

  angle_step = 360.0 / (float) sides * PI_DIVIDED_180;

  for (j = 1; j < height_segments; j++)
    {
      y_coordination = -1 * cos(j / (float) height_segments * PI) * radius;
      scale_factor = sin(j / (float) height_segments * PI);

      for (i = 0; i < sides; i++)
        {
          x_coordination = sin(i * angle_step) * scale_factor;
          z_coordination = cos(i * angle_step) * scale_factor;

          result->add_vertex(x_coordination * radius,y_coordination,z_coordination * radius,x_coordination,z_coordination,x_coordination,y_coordination,z_coordination);

          first_row_index = (j - 1) * sides + 2;

          if (j == 1)  // bottom of the sphere
            {
              if (i < sides - 1)
                result->add_triangle(0,i + 3,i + 2);
              else
                result->add_triangle(0,2,i + 2);
            }
          else if (j == height_segments - 1)  // top of the sphere
            {
              if (i < sides - 1)
                result->add_triangle(1,first_row_index + i,first_row_index + i + 1);
              else
                result->add_triangle(1,first_row_index + i,first_row_index);
            }

          if (j < height_segments - 1)
            {
              if (i < sides - 1)
                {
                  result->add_triangle(first_row_index + i,first_row_index + i + 1,first_row_index + i + 1 + sides);
                  result->add_triangle(first_row_index + i,first_row_index + i + 1 + sides,first_row_index + i + sides);
                }
              else
                {
                  result->add_triangle(first_row_index + i,first_row_index,first_row_index + sides);
                  result->add_triangle(first_row_index + i,first_row_index + sides,first_row_index + i + sides);
                }
            }
        }
    }

  result->update();

  return result;
}

//----------------------------------------------------------------------

mesh_3d *make_cylinder(float radius, float height, unsigned int sides)

{
  mesh_3d *result = new mesh_3d;

  result->add_vertex(0,0,0,0,0,0,-1,0);
  result->add_vertex(0,height,0,0,0,0,1,0);

  result->add_vertex(radius,0,0,0,0,1,0,0);
  result->add_vertex(radius,height,0,0,1,1,0,0);

  unsigned int i;
  float helper_x,helper_z;
  float angle = 0;
  float texture_step = 1 / (float) sides;
  float angle_step = 2 * PI / (float) sides;

  for (i = 0; i < sides; i++)
    {
      angle += angle_step;
      helper_x = cos(angle) * radius;
      helper_z = sin(angle) * radius;

      if (i < sides - 1)
        {
          result->add_vertex(helper_x,0,helper_z,(i + 1) * texture_step,0,helper_x,0,helper_z);
          result->add_vertex(helper_x,height,helper_z,(i + 1) * texture_step,1,helper_x,0,helper_z);

          result->add_triangle(0,2 * i + 2,2 * i + 4);
          result->add_triangle(1,2 * i + 5,2 * i + 3);

          result->add_triangle(2 * (i + 1),2 * (i + 1) + 1,2 * (i + 1) + 2);
          result->add_triangle(2 * (i + 1) + 1,2 * (i + 1) + 3,2 * (i + 1) + 2);
        }
      else
        {
          result->add_triangle(0,2 * i + 2,2);
          result->add_triangle(1,3,2 * i + 3);

          result->add_triangle(2 * (i + 1),2 * (i + 1) + 1,2);
          result->add_triangle(2 * (i + 1) + 1,3,2);
        }
    }

  result->update();

  return result;
}

//----------------------------------------------------------------------

void mesh_3d::apply_matrix(float matrix[4][4])

{
  unsigned int i;
  float helper_vector[4],result_vector[4];

  for (i = 0; i < this->vertices.size(); i++)
    {
      helper_vector[0] = this->vertices[i].position.x;
      helper_vector[1] = this->vertices[i].position.y;
      helper_vector[2] = this->vertices[i].position.z;
      helper_vector[3] = 1.0;

      multiply_vector_matrix(helper_vector,matrix,result_vector);

      this->vertices[i].position.x = result_vector[0];
      this->vertices[i].position.y = result_vector[1];
      this->vertices[i].position.z = result_vector[2];

      helper_vector[0] = this->vertices[i].normal.x;
      helper_vector[1] = this->vertices[i].normal.y;
      helper_vector[2] = this->vertices[i].normal.z;
      helper_vector[3] = 0.0;

      multiply_vector_matrix(helper_vector,matrix,result_vector);

      this->vertices[i].normal.x = result_vector[0];
      this->vertices[i].normal.y = result_vector[1];
      this->vertices[i].normal.z = result_vector[2];
    }
}

//----------------------------------------------------------------------

texture_2d::texture_2d()

{
  this->width = 0;
  this->height = 0;
  this->data = NULL;
  this->transparency_enabled = false;
  this->set_transparent_color(0,0,0);
  glGenTextures(1,&this->to);
}

//----------------------------------------------------------------------

void texture_2d::set_transparent_color(unsigned char red, unsigned char green, unsigned char blue)

{
  this->transparent_color[0] = red;
  this->transparent_color[1] = green;
  this->transparent_color[2] = blue;

  this->transparent_color_float[0] = red / 255.0;
  this->transparent_color_float[1] = green / 255.0;
  this->transparent_color_float[2] = blue / 255.0;
}

//----------------------------------------------------------------------

void texture_2d::set_transparency(bool enabled)

{
  this->transparency_enabled = enabled;
}

//----------------------------------------------------------------------

GLuint texture_2d::get_texture_object()

{
  return this->to;
}

//----------------------------------------------------------------------

bool texture_2d::load_ppm(string filename)

{
  char buffer[16];
  FILE *file_handle;
  int character, rgb_component;

  file_handle = fopen(filename.c_str(),"rb");

  if (!file_handle)
    return false;

  if (!fgets(buffer,sizeof(buffer),file_handle))   // file format
    return false;

  character = getc(file_handle);                   // skip comments

  while (character == '#')
    {
      while (getc(file_handle) != '\n');
      character = getc(file_handle);
    }

  ungetc(character,file_handle);

  // image size:

  if (fscanf(file_handle,"%d %d",(int *) &this->width,(int *) &this->height) != 2)
    {
      fclose(file_handle);
      return false;
    }

  // rgb component:

  if (fscanf(file_handle,"%d",&rgb_component) != 1)
    {
      fclose(file_handle);
      return false;
    }

  if (rgb_component != 255)  // check the depth
    {
      fclose(file_handle);
      return false;
    }

  while (fgetc(file_handle) != '\n');

  this->data = (unsigned char *) malloc(this->width * this->height * sizeof(unsigned char) * 3);

  if (!this->data)
    {
      fclose(file_handle);
      return false;
    }

  //read pixel data:

  if (fread(this->data,3 * this->width,this->height,file_handle) != this->height)
    {
      fclose(file_handle);
      return false;
    }

  fclose(file_handle);

  // upload the data to GPU:

  this->upload_texture_data();

  return true;
}

//----------------------------------------------------------------------

void mesh_3d::smooth_normals()

{
  vector<point_3d> triangle_normals;  // normals for each triangle

  unsigned int i,j,helper_index;
  point_3d vector0,vector1,vector2;

  for (i = 0; i < this->triangles.size(); i++)
    {
      vector0 = this->vertices[this->triangles[i].index1].position;
      vector1 = vector0;

      helper_index = this->triangles[i].index2;

      vector0.x -= this->vertices[helper_index].position.x;
      vector0.y -= this->vertices[helper_index].position.y;
      vector0.z -= this->vertices[helper_index].position.z;

      helper_index = this->triangles[i].index3;

      vector1.x -= this->vertices[helper_index].position.x;
      vector1.y -= this->vertices[helper_index].position.y;
      vector1.z -= this->vertices[helper_index].position.z;

      cross_product(vector0,vector1,&vector2);

      normalize_vector(&vector2);

      triangle_normals.push_back(vector2);
    }

  unsigned int number_of_triangles;
  point_3d normal_sum;

  for (j = 0; j < this->vertices.size(); j++)  // find all coresponding triangles for each vector and make an average normal of them
    {
      normal_sum.x = 0;
      normal_sum.y = 0;
      normal_sum.z = 0;
      number_of_triangles = 0;

      for (i = 0; i < this->triangles.size(); i++)
        if (this->triangles[i].index1 == j ||
            this->triangles[i].index2 == j ||
            this->triangles[i].index3 == j)
          {
            normal_sum.x += triangle_normals[i].x;
            normal_sum.y += triangle_normals[i].y;
            normal_sum.z += triangle_normals[i].z;
            number_of_triangles++;
          }

      if (number_of_triangles != 0)
        {
          normal_sum.x = normal_sum.x / (float) number_of_triangles;
          normal_sum.y = normal_sum.y / (float) number_of_triangles;
          normal_sum.z = normal_sum.z / (float) number_of_triangles;
        }
      else
        normal_sum.x = 1.0;

      this->vertices[j].normal = normal_sum;
    }

  this->update();
}

//----------------------------------------------------------------------

bool mesh_3d::load_obj(string filename)

{
  ifstream obj_file(filename.c_str());
  string line;
  float obj_line_data[4][3];

  if (!obj_file.is_open())
    return false;

  this->clear();

  while (getline(obj_file,line) )
    {
      switch (line[0])
        {
          case 'v':
            parse_obj_line(line,obj_line_data);
            this->add_vertex(obj_line_data[0][0],obj_line_data[1][0],obj_line_data[2][0],0,0,1,0,0);
            break;
          case 'f':
            parse_obj_line(line,obj_line_data);

            if (obj_line_data[3][0] < 0.0)   // 3 vertex face
              this->add_triangle(floor(obj_line_data[0][0]) - 1,floor(obj_line_data[1][0]) - 1,roundf(obj_line_data[2][0]) - 1);
            else                             // 4 vertex face
              {
                this->add_triangle(floor(obj_line_data[0][0]) - 1,floor(obj_line_data[1][0]) - 1,roundf(obj_line_data[2][0]) - 1);
                this->add_triangle(floor(obj_line_data[0][0]) - 1,floor(obj_line_data[2][0]) - 1,roundf(obj_line_data[3][0]) - 1);
              }

            break;
        }
    }

  obj_file.close();

  this->update();

  return true;
}

//----------------------------------------------------------------------

void camera_struct::set_position(float x, float y, float z)

{
  if (this->skybox != NULL)
    skybox->set_position(x,y,z);

  this->position.x = x;
  this->position.y = y;
  this->position.z = z;

  make_translation_matrix(-1 * x,-1 * y,-1 * z,camera.translation_matrix);

  glUniform3fv(camera_position_location,1,(const GLfloat *) &camera.position); // update the camera position in the shader

  update_view_matrix();
}

//----------------------------------------------------------------------

void camera_struct::move(float dx, float dy, float dz)

{
  point_3d current_position;

  this->get_position(&current_position);
  this->set_position(current_position.x + dx,current_position.y + dy,current_position.z + dz);
}

//----------------------------------------------------------------------

void camera_struct::rotate(float dx_angles, float dy_angles, float dz_angles)

{
  point_3d current_rotation;

  this->get_rotation(&current_rotation);
  this->set_rotation(current_rotation.x + dx_angles,current_rotation.y + dy_angles,current_rotation.z + dz_angles);
}

//----------------------------------------------------------------------

void camera_struct::get_direction(point_3d *direction)

{
  direction->x = this->direction_forward_vector.x;
  direction->y = this->direction_forward_vector.y;
  direction->z = this->direction_forward_vector.z;
}

//----------------------------------------------------------------------

void camera_struct::set_rotation(float x, float y, float z)

{
  this->rotation.x = angle_to_0_360(x);
  this->rotation.y = angle_to_0_360(y);
  this->rotation.z = angle_to_0_360(z);

  make_rotation_matrix(camera.rotation.x,camera.rotation.y,camera.rotation.z,ROTATION_YXZ,camera.rotation_matrix);
  update_view_matrix();

  float vector_rotation_matrix[4][4];
  make_rotation_matrix(-1 * camera.rotation.x,-1 * camera.rotation.y,-1 * camera.rotation.z,ROTATION_ZXY,vector_rotation_matrix); // for the camera vector, we have to use reversed order of transformations

  float helper_vector[4];
  float result_vector[4];

  helper_vector[0] = 0;     // initial camera direction
  helper_vector[1] = 0;
  helper_vector[2] = 1;
  helper_vector[3] = 0;

  multiply_vector_matrix(helper_vector,vector_rotation_matrix,result_vector);

  this->direction_forward_vector.x = result_vector[0];
  this->direction_forward_vector.y = result_vector[1];
  this->direction_forward_vector.z = result_vector[2];

  normalize_vector(&camera.direction_forward_vector);

  helper_vector[0] = -1;
  helper_vector[1] = 0;
  helper_vector[2] = 0;
  helper_vector[3] = 0;

  multiply_vector_matrix(helper_vector,vector_rotation_matrix,result_vector);

  this->direction_left_vector.x = result_vector[0];
  this->direction_left_vector.y = result_vector[1];
  this->direction_left_vector.z = result_vector[2];

  helper_vector[0] = 0;
  helper_vector[1] = 1;
  helper_vector[2] = 0;
  helper_vector[3] = 0;

  multiply_vector_matrix(helper_vector,vector_rotation_matrix,result_vector);

  this->direction_up_vector.x = result_vector[0];
  this->direction_up_vector.y = result_vector[1];
  this->direction_up_vector.z = result_vector[2];
}

//----------------------------------------------------------------------

void camera_struct::get_position(point_3d *position)

{
  position->x = this->position.x;
  position->y = this->position.y;
  position->z = this->position.z;
}

//----------------------------------------------------------------------

void camera_struct::get_rotation(point_3d *rotation)

{
  rotation->x = this->rotation.x;
  rotation->y = this->rotation.y;
  rotation->z = this->rotation.z;
}

//----------------------------------------------------------------------

int get_time()

{
  return glutGet(GLUT_ELAPSED_TIME);
}

//----------------------------------------------------------------------

void mesh_3d::texture_map_plane(axis_direction direction, float plane_width, float plane_height)

{
  unsigned int i;
  float width,height,depth;
  float x0,y0,z0,x1,y1,z1;
  float u_coordination,v_coordination;

  this->get_bounding_box(&x0,&y0,&z0,&x1,&y1,&z1);
  width = x0 - x1;
  width = width < 0 ? -1 * width : width;
  height = y0 - y1;
  height = height < 0 ? -1 * height : height;
  depth = z0 - z1;
  depth = depth < 0 ? -1 * depth : depth;

  for (i = 0; i < this->vertices.size(); i++)
    {
      switch (direction)
        {
          case DIRECTION_LEFT:
            u_coordination = ((this->vertices[i].position.z - z0) / depth) * plane_width;
            v_coordination = ((this->vertices[i].position.y - y0) / height) * plane_height;
            break;

          case DIRECTION_RIGHT:
            u_coordination = (1.0 - (this->vertices[i].position.z - z0) / depth) * plane_width;
            v_coordination = (1.0 - (this->vertices[i].position.y - y0) / height) * plane_height;
            break;

          case DIRECTION_FORWARD:
            u_coordination = ((this->vertices[i].position.x - x0) / width) * plane_width;
            v_coordination = ((this->vertices[i].position.y - y0) / height) * plane_height;
            break;

          case DIRECTION_BACKWARD:
            u_coordination = (1.0 - (this->vertices[i].position.x - x0) / width) * plane_width;
            v_coordination = (1.0 - (this->vertices[i].position.y - y0) / height) * plane_height;
            break;

          case DIRECTION_UP:
            u_coordination = ((this->vertices[i].position.x - x0) / width) * plane_width;
            v_coordination = ((this->vertices[i].position.z - z0) / depth) * plane_height;
            break;

          case DIRECTION_DOWN:
          default:
            u_coordination = (1.0 - (this->vertices[i].position.x - x0) / width) * plane_width;
            v_coordination = (1.0 - (this->vertices[i].position.z - z0) / depth) * plane_height;
            break;
        }

      this->vertices[i].texture_coordination[0] = u_coordination;
      this->vertices[i].texture_coordination[1] = v_coordination;
    }

  this->update();
}

//----------------------------------------------------------------------

void mesh_3d::get_position(point_3d *point)

{
  point->x = this->position.x;
  point->y = this->position.y;
  point->z = this->position.z;
}

//----------------------------------------------------------------------

void mesh_3d::get_rotation(point_3d *point)

{
  point->x = this->rotation.x;
  point->y = this->rotation.y;
  point->z = this->rotation.z;
}

//----------------------------------------------------------------------

void mesh_3d::get_scale(point_3d *point)

{
  point->x = this->scale.x;
  point->y = this->scale.y;
  point->z = this->scale.z;
}

//----------------------------------------------------------------------

void mesh_3d::set_position(float x, float y, float z)

{
  this->position.x = x;
  this->position.y = y;
  this->position.z = z;

  make_translation_matrix(x,y,z,this->translation_matrix);

  this->update_transformation_matrix();
}

//----------------------------------------------------------------------

void mesh_3d::clear()

{
  this->vertices.clear();
  this->triangles.clear();
}

//----------------------------------------------------------------------

void mesh_3d::make_instance_of(mesh_3d *what)

{
  GLuint what_vbo,what_ibo;

  if (this->vbo != 0)
    glDeleteBuffers(1,&this->vbo);

  if (this->ibo != 0)
    glDeleteBuffers(1,&this->ibo);

  what->get_vbo_ibo(&what_vbo,&what_ibo);

  this->vbo = what_vbo;
  this->ibo = what_ibo;

  this->instance_parent = what;
}

//----------------------------------------------------------------------

void mesh_3d::set_rotation(float x, float y, float z)

{
  this->rotation.x = angle_to_0_360(x);
  this->rotation.y = angle_to_0_360(y);
  this->rotation.z = angle_to_0_360(z);

  make_rotation_matrix(this->rotation.x,this->rotation.y,this->rotation.z,ROTATION_ZXY,this->rotation_matrix);

  this->update_transformation_matrix();
}

//----------------------------------------------------------------------

void mesh_3d::set_scale(float x, float y, float z)

{
  this->scale.x = x;
  this->scale.y = y;
  this->scale.z = z;

  make_scale_matrix(x,y,z,this->scale_matrix);

  this->update_transformation_matrix();
}

//----------------------------------------------------------------------

void register_keyboard_function(void (*function)(int key, int x, int y))

{
  user_keyboard_function = function;
}

//----------------------------------------------------------------------

void register_advanced_keyboard_function(void (*function)(bool key_up, int key, int x, int y))

{
  user_advanced_keyboard_function = function;
}

//----------------------------------------------------------------------

void set_perspective(float fov_degrees, float near_plane, float far_plane)

{
  float matrix[4][4];

  global_fov = fov_degrees;
  global_near = near_plane;
  global_far = far_plane;

  make_perspective_matrix(fov_degrees,near_plane,far_plane,matrix);
  glUniformMatrix4fv(perspective_matrix_location,1,GL_TRUE,(const GLfloat *)matrix);
  glUniform1f(far_plane_location,(GLfloat) far_plane);

  set_fog(global_fog_distance);        // fog uniform must be also updated
}

//----------------------------------------------------------------------

void mesh_3d::get_vbo_ibo(GLuint *vbo, GLuint *ibo)

{
  *vbo = this->vbo;
  *ibo = this->ibo;
}

//----------------------------------------------------------------------

mesh_3d::mesh_3d()

{
  this->vbo = 0;
  this->ibo = 0;
  this->texture = NULL;
  this->instance_parent = NULL;

  this->set_color(255,255,255);

  this->set_lighting_properties(0.2,0.5,0.8,100);

  this->set_position(0,0,0);
  this->set_rotation(0,0,0);
  this->set_scale(1,1,1);
  this->set_render_mode(RENDER_MODE_SHADED_GORAUD);
  this->use_fog = true;
}

//----------------------------------------------------------------------

void mesh_3d::set_texture(texture_2d *texture)

{
  this->texture = texture;
}

//----------------------------------------------------------------------

void mesh_3d::update()

{
  if (this->instance_parent != NULL)
    return;

  if (this->vbo == 0)
    glGenBuffers(1,&this->vbo);

  if (this->vbo == 0)
    cerr << "ERROR: VBO couldn't be allocated for the mesh.";

  glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
  glBufferData(GL_ARRAY_BUFFER,this->vertices.size() * sizeof(vertex_3d),&this->vertices[0],GL_STATIC_DRAW);

  if (this->ibo == 0)
    glGenBuffers(1,&this->ibo);

  if (this->ibo == 0)
    cerr << "ERROR: IBO couldn't be allocated for the mesh.";

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,this->triangles.size() * sizeof(triangle_3d),&this->triangles[0],GL_STATIC_DRAW);
}

//----------------------------------------------------------------------

void mesh_3d::set_color(unsigned char red, unsigned char green, unsigned char blue)

{
  this->color[0] = red;
  this->color[1] = green;
  this->color[2] = blue;

  this->color_float[0] = this->color[0] / 255.0f;
  this->color_float[1] = this->color[1] / 255.0f;
  this->color_float[2] = this->color[2] / 255.0f;
}

//----------------------------------------------------------------------

void camera_struct::handle_fps()

{
  float distance_to_go, angle_to_rotate;
  int time_difference;

  if (user_advanced_keyboard_function == NULL)    // this function must be registered
    return;

  time_difference = get_frame_time_difference();

  distance_to_go = time_difference * camera.movement_speed;
  angle_to_rotate = time_difference * camera.rotation_speed;

  if (global_keyboard_state[camera.key_go_forward])
    this->go(DIRECTION_FORWARD,distance_to_go);
  else if (global_keyboard_state[camera.key_go_backward])
    this->go(DIRECTION_BACKWARD,distance_to_go);

  if (global_keyboard_state[camera.key_go_left])
    this->go(DIRECTION_LEFT,distance_to_go);
  else if (global_keyboard_state[camera.key_go_right])
    this->go(DIRECTION_RIGHT,distance_to_go);

  if (global_keyboard_state[camera.key_go_up])
    this->go(DIRECTION_UP,distance_to_go);
  else if (global_keyboard_state[camera.key_go_down])
    this->go(DIRECTION_DOWN,distance_to_go);

  if (global_keyboard_state[camera.key_rotate_x_cw])
    this->rotate(-1 *angle_to_rotate,0,0);
  else if (global_keyboard_state[camera.key_rotate_x_ccw])
    this->rotate(angle_to_rotate,0,0);

  if (this->limit_x_rotation)
    {
      point_3d camera_rotation;
      this->get_rotation(&camera_rotation);

      if (camera_rotation.x < 270.0 && camera_rotation.x > 180.0)
        this->set_rotation(270.0,camera_rotation.y,camera_rotation.z);
      else if (camera_rotation.x > 90 && camera_rotation.x < 270.0)
        this->set_rotation(90.0,camera_rotation.y,camera_rotation.z);
    }

  if (global_keyboard_state[camera.key_rotate_y_cw])
    this->rotate(0,angle_to_rotate,0);
  else if (global_keyboard_state[camera.key_rotate_y_ccw])
    this->rotate(0,-1 * angle_to_rotate,0);

  if (global_keyboard_state[camera.key_rotate_z_cw])
    this->rotate(0,0,angle_to_rotate);
  else if (global_keyboard_state[camera.key_rotate_z_ccw])
    this->rotate(0,0,-1 * angle_to_rotate);
}

//----------------------------------------------------------------------

void mesh_3d::draw()

{
  unsigned int number_of_triangles;

  if (this->vertices.size() == 0 || this->triangles.size() == 0)
    return;

  float transparent_color[3];

  if (this->texture != NULL)
    this->texture->get_transparent_color_float(transparent_color,transparent_color + 1,transparent_color + 2);

  glUniformMatrix4fv(world_matrix_location,1,GL_TRUE,(const GLfloat *) this->transformation_matrix); // load this model's transformation matrix
  glUniform1ui(use_texture_location,this->texture == NULL ? 0 : 1);
  glUniform1ui(use_fog_location,this->use_fog ? 1 : 0);
  glUniform1ui(render_mode_location,(GLuint) this->mesh_render_mode);
  glUniform1ui(transparency_enabled_location,this->texture != NULL && this->texture->transparency_is_enabled() ? 1 : 0);
  glUniform3fv(transparent_color_location,1,(const GLfloat *) transparent_color);
  glUniform3fv(mesh_color_location,1,(const GLfloat *) this->color_float);
  glUniform1f(ambient_factor_location,(GLfloat) this->material_ambient_intensity);
  glUniform1f(diffuse_factor_location,(GLfloat) this->material_diffuse_intensity);
  glUniform1f(specular_factor_location,(GLfloat) this->material_specular_intensity);
  glUniform1f(specular_exponent_location,(GLfloat) this->material_specular_exponent);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  if (this->texture != NULL)
    {
      glActiveTexture(GL_TEXTURE0);      // set the active texture unit to 0
      glBindTexture(GL_TEXTURE_2D,this->texture->get_texture_object());
    }

  glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(vertex_3d),0);                   // position
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(vertex_3d),(const GLvoid*) 12);  // texture coordination
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(vertex_3d),(const GLvoid*) 20);  // normal

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ibo);

  switch (this->mesh_render_mode)
    {
      case RENDER_MODE_SHADED_GORAUD:
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        break;

      case RENDER_MODE_WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        break;

      default:
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        break;
    }

  if (this->instance_parent == NULL)
    number_of_triangles = this->triangles.size();
  else
    number_of_triangles = this->instance_parent->triangle_count();

  glDrawElements(GL_TRIANGLES,number_of_triangles * 3,GL_UNSIGNED_INT,0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

//----------------------------------------------------------------------

void mesh_3d::set_use_fog(bool enable)

{
  this->use_fog = enable;
}

//----------------------------------------------------------------------

void texture_2d::set_pixel(int x, int y, unsigned char red, unsigned char green, unsigned char blue)

{
  unsigned int index;

  if (x < 0 || x >= (int) this->width || y < 0 || y >= (int) this->height)
    return;

  index = this->xy_to_linear(x,y);

  this->data[index] = red;
  this->data[index + 1] = green;
  this->data[index + 2] = blue;
}

//----------------------------------------------------------------------

void set_fog(float distance)

{
  float fog_distance_for_shader;   // distance from the viewer and normalized
  global_fog_distance = distance;
  fog_distance_for_shader = (global_far - distance) / global_far;

  if (distance <= 0.0)
    fog_distance_for_shader = -1.0;   // disables the fog

  glUniform1f(fog_distance_location,(GLfloat) fog_distance_for_shader);
}

//----------------------------------------------------------------------

unsigned int mesh_3d::vertex_count()

{
  return this->vertices.size();
}

//----------------------------------------------------------------------

unsigned int mesh_3d::triangle_count()

{
  return this->triangles.size();
}

//----------------------------------------------------------------------

void texture_2d::get_pixel(int x, int y, unsigned char *red, unsigned char *green, unsigned char *blue)

{
  unsigned int index;

  if (x < 0 || x >= (int) this->width || y < 0 || y >= (int) this->height)
    {
      *red = 0;
      *green = 0;
      *blue = 0;
      return;
    }

  index = this->xy_to_linear(x,y);

  *red = this->data[index];
  *green = this->data[index + 1];
  *blue = this->data[index + 2];
}

//----------------------------------------------------------------------

void mesh_3d::flip_triangles()

{
  unsigned int i, helper_index;

  for (i = 0; i < this->vertices.size(); i++)
    {
      this->vertices[i].normal.x *= -1;
      this->vertices[i].normal.y *= -1;
      this->vertices[i].normal.z *= -1;
    }

  for (i = 0; i < this->triangles.size(); i++)   // change the clockwiseness of each triangle
    {
      helper_index = this->triangles[i].index1;
      this->triangles[i].index1 = this->triangles[i].index2;
      this->triangles[i].index2 = helper_index;
    }

  this->update();
}

//----------------------------------------------------------------------

void camera_struct::set_skybox(mesh_3d *what)

{
  this->skybox = what;

  this->set_position(this->position.x,this->position.y,this->position.z);   // aligns the skybox with the camera
}

//----------------------------------------------------------------------

void mesh_3d::get_bounding_box(float *x0, float *y0, float *z0, float *x1, float *y1, float *z1)

{
  unsigned int i;

  *x0 = numeric_limits<float>::max();
  *y0 = numeric_limits<float>::max();
  *z0 = numeric_limits<float>::max();
  *x1 = numeric_limits<float>::min();
  *y1 = numeric_limits<float>::min();
  *z1 = numeric_limits<float>::min();

  for (i = 0; i < this->vertices.size(); i++)
    {
      if (this->vertices[i].position.x < *x0)
        *x0 = this->vertices[i].position.x;

      if (this->vertices[i].position.y < *y0)
        *y0 = this->vertices[i].position.y;

      if (this->vertices[i].position.z < *z0)
        *z0 = this->vertices[i].position.z;

      if (this->vertices[i].position.x > *x1)
        *x1 = this->vertices[i].position.x;

      if (this->vertices[i].position.y > *y1)
        *y1 = this->vertices[i].position.y;

      if (this->vertices[i].position.z > *z1)
        *z1 = this->vertices[i].position.z;
    }
}

//----------------------------------------------------------------------

int get_frame_time_difference()

{
  return glutGet(GLUT_ELAPSED_TIME) - global_previous_frame_time;
}

//----------------------------------------------------------------------

void mesh_3d::print_data()

{
  unsigned int i;

  cout << "vertices (index: position; uv; normal):" << endl;

  for (i = 0; i < this->vertices.size(); i++)
    cout << i << ": " << this->vertices[i].position.x << ", " <<
                         this->vertices[i].position.y << ", " <<
                         this->vertices[i].position.z << "; " <<
                         this->vertices[i].texture_coordination[0] << ", " <<
                         this->vertices[i].texture_coordination[1] << "; " <<
                         this->vertices[i].normal.x << "; " <<
                         this->vertices[i].normal.y << "; " <<
                         this->vertices[i].normal.z << endl;

  cout << endl;
  cout << "triangles (index: indices):" << endl;

  for (i = 0; i < this->triangles.size(); i++)
    cout << i << ": " << this->triangles[i].index1 << ", " <<
                         this->triangles[i].index2 << ", " <<
                         this->triangles[i].index3 << endl << endl;
}

//----------------------------------------------------------------------

void set_background_color(unsigned char red, unsigned char green, unsigned char blue)

{
  float helper_array[3];

  global_background_color[0] = red;
  global_background_color[1] = green;
  global_background_color[2] = blue;

  helper_array[0] = red / 255.0;
  helper_array[1] = green / 255.0;
  helper_array[2] = blue / 255.0;

  glUniform3fv(background_color_location,1,(const GLfloat *) helper_array);

  glClearColor(helper_array[0],helper_array[1],helper_array[2],1.0);
}

//----------------------------------------------------------------------

void mesh_3d::add_vertex(float x, float y, float z, float texture_u, float texture_v, float normal_x, float normal_y, float normal_z)

{
  vertex_3d vertex;

  vertex.position.x = x;
  vertex.position.y = y;
  vertex.position.z = z;

  vertex.texture_coordination[0] = texture_u;
  vertex.texture_coordination[1] = texture_v;

  vertex.normal.x = normal_x;
  vertex.normal.y = normal_y;
  vertex.normal.z = normal_z;

  normalize_vector(&vertex.normal);

  this->vertices.push_back(vertex);
}

//----------------------------------------------------------------------

void mesh_3d::set_render_mode(render_mode mode)

{
  this->mesh_render_mode = mode;
}

//----------------------------------------------------------------------

void mesh_3d::add_triangle(unsigned int index1, unsigned int index2, unsigned int index3)

{
  triangle_3d triangle;

  triangle.index1 = index1;
  triangle.index2 = index2;
  triangle.index3 = index3;

  this->triangles.push_back(triangle);
}

//----------------------------------------------------------------------

void draw_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)

{
  unsigned char bytes[4];

  bytes[0] = r;
  bytes[1] = g;
  bytes[2] = b;
  bytes[3] = a;

  glRasterPos2d(x / (double) global_window_width,y / (double) global_window_height);

  glDrawPixels(1,1,GL_RGBA,GL_UNSIGNED_BYTE,bytes);
}

//----------------------------------------------------------------------

mesh_3d::~mesh_3d()

{
  if (this->vbo != 0)
    glDeleteBuffers(1,&this->vbo);

  if (this->ibo != 0)
    glDeleteBuffers(1,&this->ibo);
}

//----------------------------------------------------------------------

void texture_2d::update()

{
  this->upload_texture_data();
}

//----------------------------------------------------------------------

texture_2d::~texture_2d()

{
  if (this->data != NULL)
    free(this->data);

  glDeleteTextures(1,&this->to);
}

//----------------------------------------------------------------------

void normalize_vector(point_3d *vector)

{
  float length = vector_length(*vector);

  vector->x = vector->x / length;
  vector->y = vector->y / length;
  vector->z = vector->z / length;
}

//----------------------------------------------------------------------

float vector_length(point_3d vector)

{
  return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

//----------------------------------------------------------------------

void init_opengl(int *argc_pointer, char** argv, unsigned int window_width, unsigned int window_height, void (*draw_function)(void), const char *window_title)

{
  global_window_width = window_width;
  global_window_height = window_height;
  global_fov = 95;
  global_near = 0.05;
  global_far = 100;

  user_render_function = draw_function;
  glutInit(argc_pointer,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutInitWindowSize(window_width,window_height);
  glutInitWindowPosition(1,1);
  glutCreateWindow(window_title);
  glutDisplayFunc(loop_function);
  glutIdleFunc(loop_function);
  glutKeyboardFunc(helper_keyboard_function);
  glutKeyboardUpFunc(helper_advanced_keyboard_function);
  glutSpecialUpFunc(helper_advanced_special_keyboard_function);
  glutSpecialFunc(helper_special_function);
  glutReshapeFunc(reshape_function);
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glewInit();
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  compile_shaders();
  set_perspective(global_fov,global_near,global_far);
  camera.set_position(0,0,0);
  camera.set_rotation(0,0,0);
  glUniform1i(texture_unit_location,0);   // we'll always be using the unit 0

  point_3d light_direction;

  light_direction.x = 1;
  light_direction.y = -1;
  light_direction.z = 1;

  set_global_light(light_direction,255,255,255);
  set_background_color(0,0,0);

  unsigned int i;

  for (i = 0; i < 512; i++)
    global_keyboard_state[i] = false;
}

//----------------------------------------------------------------------

#endif
