/*
 Obj model viewer with OpenGLSE.

 usage: run the program either without one or two parameters, the first
        one is obj model to be loaded, the second one is texture in ppm
        format

 Miloslav Číž, 2014
 */

#include "../../openglse.hpp"
#include <cstdlib>

using namespace gl_se;

mesh_3d_static model;
texture_2d model_texture;

static void render_scene()
  {
    camera.handle_fps();   // handles the camera as in FPS games
    model.draw();
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
  init_opengl(&argc,argv,800,600,render_scene,"model viewer");    // this must be done before anything else
  set_mouse_visibility(false);                                    // hides the mouse cursor
  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);

  if (argc >= 2)  // model specified
    {
      string model_filename;
      model_filename = argv[1];

      if (!model.load_obj(model_filename))
        {
          cerr << "error: couldn't load the model" << endl;
          exit(1);
        }

      model.smooth_normals();
      model.set_render_mode(RENDER_MODE_SHADED_PHONG);    // turns on per-pixel shading
      model.set_lighting_properties(3,6,6,1.5);
      model.scale_to_size(5);

      if (argc >= 3)  // texture specified
        {
          string texture_filename;
          texture_filename = argv[2];

          if (!model_texture.load_ppm(texture_filename))
            {
              cerr << "error: couldn't load the texture" << endl;
              exit(1);
            }

          model.set_texture(&model_texture);
        }
    }

  camera.rotation_speed = 0.04;
  camera.set_position(3.2,3.7,-2);
  camera.set_rotation(41,288,0);

  set_background_color(145,236,242);

  render_loop();     // starts the rendering loop

  return 0;
}
