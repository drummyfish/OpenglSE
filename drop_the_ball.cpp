/**
  Example application for the OpenglSE library.

  work in progress

  Miloslav Číž, 2014
*/

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "openglse.hpp"

using namespace std;
using namespace gl_se;

mesh_3d_static *ball, *terrain, *skybox;

int counter = 0;
int last_time = 0;

static void render_scene()

  {
    ball->draw();

    skybox->draw();
        terrain->draw();
    camera.handle_fps();


    if (counter <= 0)
      {
        counter = 255;
        cout << "FPS: " << get_fps() << endl;
      }
    else
      counter--;
  }

static void keyboard_function2(bool key_up, int key, int x, int y)

  {
    // this function must be registered in order for camera.handle_fps to work
  }

static void keyboard_function(int key, int x, int y)
  {
    if (key == 'q')
      {
        point_3d camera_position;

        camera.get_position(&camera_position);
        ball->set_position(camera_position.x,camera_position.y,camera_position.z);
      }
  }

int main(int argc, char** argv)

  {
    init_opengl(&argc,argv,800,600,render_scene,"drop the ball");

    texture_2d grass,sand,terrain_heightmap,terrain_mask,sky;
    grass.load_ppm("grass.ppm");
    sand.load_ppm("sand.ppm");
    terrain_heightmap.load_ppm("terrain.ppm");
    sky.load_ppm("skybox.ppm");
    sky.load_ppm("sky.ppm");
    terrain_mask.load_ppm("mask.ppm");

    terrain = make_terrain(100,100,20,30,30,&terrain_heightmap);
    terrain->texture_map_layer_mask(&terrain_mask);
    terrain->set_texture(&grass);
    terrain->set_texture2(&sand);
    terrain->set_lighting_properties(0.2,1.0,0.75,1);
    terrain->texture_map_plane(DIRECTION_DOWN,10,10);
    terrain->set_render_mode(RENDER_MODE_SHADED_PHONG);
    terrain->set_position(0,-7,0);

    skybox = make_sphere(1.0,10,10);
    skybox->set_scale(100);
    skybox->texture_map_plane(DIRECTION_LEFT,1.0,1.0);
    skybox->set_texture(&sky);
    skybox->set_color(255,0,20);
    skybox->flip_triangles();
    skybox->set_render_mode(RENDER_MODE_NO_LIGHT);

    ball = make_sphere(2,10,10);
    ball->set_render_mode(RENDER_MODE_SHADED_PHONG);
    ball->set_color(200,50,150);
    ball->set_lighting_properties(0.3,1.0,0.9,500);
    ball->set_position(0,10,0);

    point_3d light_direction;
    light_direction.x = 1;
    light_direction.y = -2;
    light_direction.z = 5;

    set_global_light(light_direction,250,225,145);
    set_fog(45);
    camera.set_position(0,10,-10);
    camera.set_skybox(skybox);          // skybox will follow the camera

    register_advanced_keyboard_function(keyboard_function2);
    register_keyboard_function(keyboard_function);

    set_background_color(93,144,209);

    glutMainLoop();

    delete skybox;
    delete ball;
    delete terrain;

    return 0;
  }

//----------------------------------------------------------------------
