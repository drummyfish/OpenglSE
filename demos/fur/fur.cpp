/*
 Fur rendering with OpenGLSE.

 usage: run the program either without parameters or with one parameter
        specifying the path to obj file to be rendered with fur

 Miloslav Číž, 2014
 */

#include "../../openglse.hpp"
#include <cstdlib>

using namespace gl_se;

#define LAYERS 50
#define DENSITY 20         // the less the more dense
#define TEXTURE_RESOLUTION 256

mesh_3d_static *model;
texture_2d fur_texture;

static void render_scene()
  {
    camera.handle_fps();   // handles the camera as in FPS games
    model->draw();
  }

static void keyboard_function2(bool key_up, int key, int x, int y) // this function must be registered in order for camera.handle_fps() to work
  {
  }

static void keyboard_function(int key, int x, int y)
  {
    if (key == 'q') // quit
      stop_rendering();
  }

int main(int argc, char **argv)

{
  unsigned int i,j;
  mesh_3d_static *original_model;
  float scale_matrix[4][4];
  float scale;

  init_opengl(&argc,argv,800,600,render_scene,"fur rendering");   // this must be done before anything else
  set_mouse_visibility(false);                                    // hides the mouse cursor
  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);

  fur_texture.initialise(TEXTURE_RESOLUTION,TEXTURE_RESOLUTION);

  for (j = 0; j < TEXTURE_RESOLUTION; j++)          // make the fur layer texture
    for (i = 0; i < TEXTURE_RESOLUTION; i++)
      {
        if (rand() % DENSITY == 0)
          {
            fur_texture.set_pixel(i,j,255,210,210);          // a signle hair
            fur_texture.set_pixel(i + 1,j + 1,150,150,150);  // shadow
          }
      }

  fur_texture.set_transparent_color(255,255,255);
  fur_texture.set_transparency(true);
  fur_texture.update();

  // make the mesh:

  if (argc == 2)  // model specified
    {
      string filename;
      filename = argv[1];
      original_model = new mesh_3d_static();
      original_model->load_obj(filename);
      original_model->smooth_normals();
      original_model->texture_map_plane(DIRECTION_FORWARD,3,3);
      original_model->update();

      float width,height,depth;

      original_model->get_size(&width,&height,&depth);

      // scale the model to size cca 5:
      original_model->scale_to_size(5,true);
    }
  else   // default model - a sphere
    {
      original_model = make_sphere(2,15,15);
    }

  scale = 1.01;
  make_scale_matrix(scale,scale,scale,scale_matrix);

  model = new mesh_3d_static();
  model->set_texture(&fur_texture);
  model->set_render_mode(RENDER_MODE_SHADED_PHONG);
  model->set_lighting_properties(0.2,0.6,0.6,2);

  for (i = 0; i < LAYERS; i++)
    {
      model->merge(original_model);
      original_model->apply_matrix(scale_matrix);
    }

  delete original_model;

  camera.rotation_speed = 0.04;
  camera.set_position(3.2,3.7,-2);
  camera.set_rotation(41,288,0);

  render_loop();                 // starts the rendering loop

  delete model;

  return 0;
}
