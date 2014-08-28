/*
 A sanbox application made with OpenGLSE.

 Miloslav Číž, 2014
 */

#include "../../openglse.hpp"

using namespace gl_se;

#define NUMBER_OF_TEXT_LINES 3
#define WRITE_FPS_EACH_FRAMES 50
#define ROOM_SIZE 30

mesh_3d_static *room, *cow, *sphere;
texture_2d room_texture;
picture_2d *text_lines[NUMBER_OF_TEXT_LINES];

int counter = WRITE_FPS_EACH_FRAMES;
float room_size_half = ROOM_SIZE / 2.0 - 0.5;

static void render_scene()
  {
    unsigned int i;
    point_3d camera_position;

    camera.handle_fps();   // handles the camera as in FPS games

    camera.get_position(&camera_position);     // limit the camera movement to only within the room
    camera_position.x = clamp(camera_position.x,-1 * room_size_half,room_size_half);
    camera_position.y = clamp(camera_position.y,-1 * room_size_half,room_size_half);
    camera_position.z = clamp(camera_position.z,-1 * room_size_half,room_size_half);
    camera.set_position(camera_position.x,camera_position.y,camera_position.z);

    room->draw();          // draw the objects
    cow->draw();
    sphere->draw();

    for (i = 0; i < NUMBER_OF_TEXT_LINES; i++)   // draw the text
      text_lines[i]->draw();

    if (counter <= 0)      // once every few frames write out the FPS
      {
        counter = WRITE_FPS_EACH_FRAMES;
        cout << "FPS: " << get_fps() << endl;
      }

    counter--;
  }

static void keyboard_function2(bool key_up, int key, int x, int y) // this function must be registered in order for camera.handle_fps() to work
  {
  }

static void keyboard_function(int key, int x, int y)
  {
    point_3d camera_position, camera_direction;

    switch (key)
      {
        case 'q':  // quit
          stop_rendering();
          break;

        case 's':
          camera.get_position(&camera_position);
          camera.get_direction(&camera_direction);
          sphere->set_position(camera_position.x + camera_direction.x * 3.5,camera_position.y + camera_direction.y * 3.5,camera_position.z + camera_direction.z * 3.5);
          break;
      }
  }

int main(int argc, char **argv)

{
  unsigned int i;

  init_opengl(&argc,argv,800,600,render_scene,"OpenglSE intro");  // this must be done before anything else
  set_mouse_visibility(false);                                    // hides the mouse cursor
  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);

  room_texture.load_ppm("room.ppm");

  text_lines[0] = make_text("q to quit");
  text_lines[1] = make_text("s to move the sphere");
  text_lines[2] = make_text("");

  for (i = 0; i < NUMBER_OF_TEXT_LINES; i++)                // set the text line positions
    text_lines[i]->set_position(0.02,i * 0.2);

  room = make_sharp_cuboid(ROOM_SIZE,ROOM_SIZE,ROOM_SIZE);  // makes a box that will represent the room
  room->flip_triangles();                                   // flips the box inside out, as the camera will be inside it
  room->set_render_mode(RENDER_MODE_SHADED_PHONG);          // turns on per-pixel shading instead of per-vertex
  room->set_texture(&room_texture);
  room->set_lighting_properties(2,5,0.5,1);

  cow = new mesh_3d_static();                               // load the cow model from a file
  cow->load_obj("cow.obj");

  if (cow == NULL)
    cerr << "error: cow model couldn't be loaded" << endl;

  cow->smooth_normals();
  cow->set_scale(0.75);
  cow->set_render_mode(RENDER_MODE_SHADED_PHONG);
  cow->set_color(255,0,0);

  sphere = make_sphere(2.0,10,10);
  sphere->set_position(1,-1,5);
  sphere->set_render_mode(RENDER_MODE_SHADED_PHONG);
  sphere->set_color(0,255,0);

  camera.set_position(3,2,2);

  render_loop();                 // starts the rendering loop

  for (i = 0; i < NUMBER_OF_TEXT_LINES; i++)
    delete text_lines[i];

  delete room;
  delete sphere;
  delete cow;

  return 0;
}
