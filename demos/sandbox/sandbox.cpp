/*
 A sanbox application made with OpenGLSE.

 Miloslav Číž, 2014
 */

#include "../../openglse.hpp"

using namespace gl_se;

#define NUMBER_OF_TEXT_LINES 3

mesh_3d_static *room;
texture_2d room_texture;
picture_2d *text_lines[NUMBER_OF_TEXT_LINES];

static void render_scene()
  {
    unsigned int i;

    camera.handle_fps();   // handles the camera as in FPS games
    room->draw();

    for (i = 0; i < NUMBER_OF_TEXT_LINES; i++)
      text_lines[i]->draw();
  }

static void keyboard_function2(bool key_up, int key, int x, int y) // this function must be registered in order for camera.handle_fps() to work
  {
  }

static void keyboard_function(int key, int x, int y)
  {
    if (key == 'q')
      stop_rendering();
  }

int main(int argc, char **argv)

{
  unsigned int i;

  init_opengl(&argc,argv,800,600,render_scene,"OpenglSE intro");  // this must be done before anything else
  set_mouse_visibility(false);
  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);

  room_texture.load_ppm("room.ppm");

  text_lines[0] = make_text("q to quit");
  text_lines[1] = make_text("");
  text_lines[2] = make_text("");

  room = make_sharp_cuboid(30,30,30);              // makes a box that will represent the room
  room->flip_triangles();                          // flips the box inside out, as the camera will be inside it
  room->set_render_mode(RENDER_MODE_SHADED_PHONG); // turns on per-pixel shading instead of per-vertex
  room->set_texture(&room_texture);
  room->set_lighting_properties(2,5,0.5,1);

  render_loop();                 // starts the rendering loop

  for (i = 0; i < NUMBER_OF_TEXT_LINES; i++)
    delete text_lines[i];

  delete room;
  return 0;
}
