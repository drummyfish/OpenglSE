/*
 Work in progress
 */

#include "../../openglse.hpp"

using namespace gl_se;

/* this function will be called in the main loop to render the scene
   repeatedly: */

mesh_3d_static *terrain, *water_frame_0, *water_frame_1, *skybox,
               *rock1, *rock2, *tree, *tree_low;

mesh_3d_static *rock1_instances[3];
mesh_3d_static *rock2_instances[5];

mesh_3d_animated *water;
texture_2d terrain_heightmap, terrain_texturemap, water_texture,
           grass_texture, sand_texture, sky_texture, rock_texture,
           tree_texture;

static void render_scene()

  {
    camera.handle_fps();

    rock1_instances[0]->draw();
    rock1_instances[1]->draw();
    rock1_instances[2]->draw();

    rock2_instances[0]->draw();
    rock2_instances[1]->draw();
    rock2_instances[2]->draw();
    rock2_instances[3]->draw();
    rock2_instances[4]->draw();

    tree_low->draw();

    terrain->draw();
    water->draw();
    skybox->draw();

    point_3d p;
    camera.get_position(&p);
   // cout << p.x << " " << p.y << " " << p.z << endl;
  }

static void keyboard_function2(bool key_up, int key, int x, int y)

  {
  }

static void keyboard_function(int key, int x, int y)

  {
    if (key == 'p')
      exit(0);
  }

void init_scene()

  {
    unsigned int i;

    terrain_heightmap.load_ppm("terrain_heightmap.ppm");
    terrain_texturemap.load_ppm("terrain_texturemap.ppm");
    water_texture.load_ppm("water.ppm");
    grass_texture.load_ppm("grass.ppm");
    sand_texture.load_ppm("sand.ppm");
    sky_texture.load_ppm("sky.ppm");
    rock_texture.load_ppm("rock.ppm");
    tree_texture.load_ppm("tree.ppm");
    tree_texture.set_transparency(true);
    tree_texture.set_transparent_color(255,0,0);

    tree_low = new mesh_3d_static();
    tree_low->load_obj("tree_low.obj");
    tree_low->set_texture(&tree_texture);

    tree = new mesh_3d_static();
    tree->load_obj("tree.obj");

    tree->set_scale(0.1);
    tree->set_texture(&tree_texture);

    terrain = make_terrain(50,50,10,100,100,&terrain_heightmap);
    terrain->texture_map_plane(DIRECTION_DOWN,7,7);
    terrain->texture_map_layer_mask(&terrain_texturemap);
    terrain->set_render_mode(RENDER_MODE_SHADED_PHONG);
    terrain->set_lighting_properties(0.3,0.7,0.3,1.5);
    terrain->set_texture(&grass_texture);
    terrain->set_texture2(&sand_texture);

    water_frame_0 = make_terrain(500,500,1,20,20,NULL);
    water_frame_0->texture_map_plane(DIRECTION_DOWN,20,20);

    water_frame_1 = new mesh_3d_static(water_frame_0);
    water_frame_1->update();

    skybox = make_sphere(200,15,15);
    skybox->texture_map_plane(DIRECTION_FORWARD,3,3);
    skybox->flip_triangles();
    skybox->set_render_mode(RENDER_MODE_NO_LIGHT);
    skybox->set_texture(&sky_texture);

    camera.set_skybox(skybox);

    for (i = 0; i < water_frame_0->vertex_count(); i++)
      {
        water_frame_0->vertices[i].position.y += (rand() % 100) * 0.02;
        water_frame_1->vertices[i].position.y += (rand() % 100) * 0.02;
        water_frame_1->vertices[i].texture_coordination[0] += (rand() % 10 - 5) * 0.01;
        water_frame_1->vertices[i].texture_coordination[1] += (rand() % 10 - 5) * 0.01;
      }

    water_frame_0->update();
    water_frame_1->update();

    water = new mesh_3d_animated();

    water->add_frame(water_frame_0,2000);
    water->set_texture(&water_texture);
    water->add_frame(water_frame_1,2000);
    water->set_position(0,1,0);
    water->set_render_mode(RENDER_MODE_SHADED_PHONG);
    water->set_lighting_properties(0.4,0.2,0.9,5);
    water->update();

    water->set_playing(true);


    rock1 = new mesh_3d_static();
    rock1->load_obj("rock1.obj");

    for (i = 0; i < 3; i++)
      {
        rock1_instances[i] = new mesh_3d_static();
        rock1_instances[i]->set_render_mode(RENDER_MODE_SHADED_PHONG);
        rock1_instances[i]->set_texture(&rock_texture);
        rock1_instances[i]->make_instance_of(rock1);
        rock1_instances[i]->set_lighting_properties(0.6,0.6,0.3,1.2);
      }

    rock1_instances[0]->set_position(16,0,3);
    rock1_instances[0]->set_scale(0.3);
    rock1_instances[0]->set_rotation(0,10,3);

    rock1_instances[1]->set_position(5,0,22);
    rock1_instances[1]->set_scale(0.2);
    rock1_instances[1]->set_rotation(5,90,-3);

    rock1_instances[2]->set_position(-28,0,-14);
    rock1_instances[2]->set_scale(0.25);

    rock2 = new mesh_3d_static();
    rock2->load_obj("rock2.obj");

    for (i = 0; i < 5; i++)
      {
        rock2_instances[i] = new mesh_3d_static();
        rock2_instances[i]->set_render_mode(RENDER_MODE_SHADED_PHONG);
        rock2_instances[i]->set_texture(&rock_texture);
        rock2_instances[i]->make_instance_of(rock2);
        rock2_instances[i]->set_lighting_properties(0.6,0.6,0.3,1.2);
        rock2_instances[i]->set_scale(0.08);
      }


    rock2_instances[0]->set_position(14,2.5,-14);
    rock2_instances[1]->set_position(2,8.5,4);
    rock2_instances[1]->set_scale(0.12);

    rock2_instances[2]->set_position(-11,5,-11);
    rock2_instances[2]->set_rotation(0,20,0);
    rock2_instances[2]->set_scale(0.1);

    rock2_instances[2]->set_position(-2,9,-5);
    rock2_instances[2]->set_scale(0.2);

    rock2_instances[3]->set_position(-7,2,20);
    rock2_instances[3]->set_rotation(0,122,0);
    rock2_instances[3]->set_scale(0.15);

    rock2_instances[4]->set_position(-15,2,21);
    rock2_instances[4]->set_rotation(0,5,0);
    rock2_instances[4]->set_scale(0.12);
  }

int main(int argc, char **argv)

{
  unsigned int i;

  init_opengl(&argc,argv,800,600,render_scene,"OpenglSE intro");

  init_scene();

  set_perspective(110,0.01,500);

  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);



  camera.set_position(0,17,0);

  camera.rotation_speed = 0.025;

  render_loop();
  return 0;
}
