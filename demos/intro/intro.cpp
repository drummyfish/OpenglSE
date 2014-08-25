/*
 Work in progress
 */

#include "../../openglse.hpp"

using namespace gl_se;

/* this function will be called in the main loop to render the scene
   repeatedly: */

#define NUMBER_OF_TREES 40
#define TERRAIN_HEIGHT 10
#define TERRAIN_TILE_FACTOR 10

mesh_3d_static *terrain, *water_frame_0, *water_frame_1, *skybox,
               *rock1, *rock2, *tree, *tree_low, *sun, *water_static;

mesh_3d_static *rock1_instances[3];
mesh_3d_static *rock2_instances[5];

mesh_3d_animated *water;
texture_2d terrain_heightmap, terrain_texturemap, water_texture,
           grass_texture, sand_texture, sky_texture, rock_texture,
           tree_texture;

float rendering_started_at;

keyframe_interpolator     // camera interpolators:
  i_x,                    // position x
  i_y,                    // position y
  i_z,                    // position z
  i_r_x,                  // rotation around x
  i_r_y;                  // rotation around y

mesh_3d_lod *trees[NUMBER_OF_TREES];


void setup_camera_keyframes()
  {                  // time   // value
    i_x.add_keyframe(  0,           114,     INTERPOLATION_SINE);
    i_y.add_keyframe(  0,           6,       INTERPOLATION_SINE);
    i_z.add_keyframe(  0,           61,      INTERPOLATION_SINE);
    i_r_x.add_keyframe(0,           0,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(0,           230,     INTERPOLATION_SINE);

    i_x.add_keyframe(  7000,        36,      INTERPOLATION_SINE);
    i_y.add_keyframe(  6000,        5.5,     INTERPOLATION_SINE);
    i_z.add_keyframe(  7500,        1,       INTERPOLATION_SINE);
    i_r_x.add_keyframe(7200,        0,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(6000,        240,     INTERPOLATION_SINE);

    i_x.add_keyframe(  12500,       36,      INTERPOLATION_CONSTANT);
    i_y.add_keyframe(  12100,       10,      INTERPOLATION_CONSTANT);
    i_z.add_keyframe(  12000,       1,       INTERPOLATION_CONSTANT);
    i_r_x.add_keyframe(12200,       2,       INTERPOLATION_CONSTANT);
    i_r_y.add_keyframe(12000,       270,     INTERPOLATION_CONSTANT);

    i_x.add_keyframe(  13000,       9,       INTERPOLATION_SINE);
    i_y.add_keyframe(  13000,       16,      INTERPOLATION_SINE);
    i_z.add_keyframe(  13000,       -23,     INTERPOLATION_SINE);
    i_r_x.add_keyframe(13000,       35,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(13000,       341,     INTERPOLATION_SINE);

    i_x.add_keyframe(  15500,       -18,     INTERPOLATION_SINE);
    i_y.add_keyframe(  15100,       9,       INTERPOLATION_SINE);
    i_z.add_keyframe(  14600,       -20,     INTERPOLATION_SINE);
    i_r_x.add_keyframe(14700,       11,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(15200,       375,     INTERPOLATION_SINE);

    i_x.add_keyframe(  17700,       -25,     INTERPOLATION_LINEAR);
    i_y.add_keyframe(  18500,       3,       INTERPOLATION_LINEAR);
    i_z.add_keyframe(  18100,       -1,      INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(18005,       -10,     INTERPOLATION_SINE);
    i_r_y.add_keyframe(17800,       440,     INTERPOLATION_SINE);

    i_x.add_keyframe(  20500,       -23,     INTERPOLATION_LINEAR);
    i_y.add_keyframe(  20100,       5,       INTERPOLATION_LINEAR);
    i_z.add_keyframe(  19700,       14,      INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(20100,       2,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(20200,       460,     INTERPOLATION_SINE);

    i_x.add_keyframe(  22100,       -18,     INTERPOLATION_LINEAR);
    i_y.add_keyframe(  21500,       11,      INTERPOLATION_LINEAR);
    i_z.add_keyframe(  22100,       38,      INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(22000,       11,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(22000,       500,     INTERPOLATION_SINE);

    i_x.add_keyframe(  25500,       18,      INTERPOLATION_CONSTANT);
    i_y.add_keyframe(  24900,       17,      INTERPOLATION_CONSTANT);
    i_z.add_keyframe(  25100,       37,      INTERPOLATION_CONSTANT);
    i_r_x.add_keyframe(25000,       29,      INTERPOLATION_CONSTANT);
    i_r_y.add_keyframe(25100,       550,     INTERPOLATION_CONSTANT);

    i_x.add_keyframe(  26000,       14,      INTERPOLATION_SINE);
    i_y.add_keyframe(  26000,       5,       INTERPOLATION_SINE);
    i_z.add_keyframe(  26000,       11,      INTERPOLATION_SINE);
    i_r_x.add_keyframe(26000,       7,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(26000,       321,     INTERPOLATION_SINE);

    i_x.add_keyframe(  33000,       14,      INTERPOLATION_CONSTANT);
    i_y.add_keyframe(  33000,       5,       INTERPOLATION_CONSTANT);
    i_z.add_keyframe(  33000,       11,      INTERPOLATION_CONSTANT);
    i_r_x.add_keyframe(33000,       7,       INTERPOLATION_CONSTANT);
    i_r_y.add_keyframe(33000,       270,     INTERPOLATION_CONSTANT);

    i_x.add_keyframe(  34000,       10,      INTERPOLATION_LINEAR);
    i_y.add_keyframe(  34000,       7,       INTERPOLATION_LINEAR);
    i_z.add_keyframe(  34000,       12,      INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(34000,       15,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(34000,       196,     INTERPOLATION_SINE);

    i_x.add_keyframe(  38000,       8,       INTERPOLATION_LINEAR);
    i_y.add_keyframe(  38000,       9,       INTERPOLATION_LINEAR);
    i_z.add_keyframe(  38000,       2.4,     INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(38000,       -5,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(38000,       259,     INTERPOLATION_SINE);

    i_x.add_keyframe(  42000,       0,       INTERPOLATION_LINEAR);
    i_y.add_keyframe(  42000,       11,      INTERPOLATION_LINEAR);
    i_z.add_keyframe(  42000,       4.1,     INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(42000,       0,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(42000,       280,     INTERPOLATION_SINE);

    i_x.add_keyframe(  46000,       -11,     INTERPOLATION_LINEAR);
    i_y.add_keyframe(  46000,       15,      INTERPOLATION_LINEAR);
    i_z.add_keyframe(  46000,       8,       INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(46000,       46,      INTERPOLATION_SINE);
    i_r_y.add_keyframe(46000,       168,     INTERPOLATION_SINE);

    i_x.add_keyframe(  50000,       -15,     INTERPOLATION_CONSTANT);
    i_y.add_keyframe(  50000,       27,      INTERPOLATION_CONSTANT);
    i_z.add_keyframe(  50000,       5,       INTERPOLATION_CONSTANT);
    i_r_x.add_keyframe(50000,       82,      INTERPOLATION_CONSTANT);
    i_r_y.add_keyframe(50000,       108,     INTERPOLATION_CONSTANT);

    i_x.add_keyframe(  52000,       -6,      INTERPOLATION_LINEAR);
    i_y.add_keyframe(  52000,       4.1,     INTERPOLATION_LINEAR);
    i_z.add_keyframe(  52000,       35.48,   INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(52000,       0,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(52000,       218,     INTERPOLATION_SINE);

    i_x.add_keyframe(  62000,       -130,    INTERPOLATION_LINEAR);
    i_y.add_keyframe(  62000,       8,       INTERPOLATION_LINEAR);
    i_z.add_keyframe(  62000,       -127,    INTERPOLATION_LINEAR);
    i_r_x.add_keyframe(62000,       0,       INTERPOLATION_SINE);
    i_r_y.add_keyframe(62000,       218,     INTERPOLATION_SINE);
  }

static void render_scene()

  {
    float parameter;
    parameter = get_time() - rendering_started_at;

    if (parameter >= 62000)  // end of intro
      stop_rendering();

    // get the camera position and rotation from interpolators:
    camera.set_position(i_x.get_value(parameter),i_y.get_value(parameter),i_z.get_value(parameter));
    camera.set_rotation(i_r_x.get_value(parameter),i_r_y.get_value(parameter),0);

    unsigned int i;

    for (i = 0; i < NUMBER_OF_TREES; i++)
      {
        trees[i]->draw();
      }

    rock1_instances[0]->draw();
    rock1_instances[1]->draw();
    rock1_instances[2]->draw();

    rock2_instances[0]->draw();
    rock2_instances[1]->draw();
    rock2_instances[2]->draw();
    rock2_instances[3]->draw();
    rock2_instances[4]->draw();

    sun->draw();

    terrain->draw();
    water->draw();
    water_static->draw();
    skybox->draw();

    point_3d p;
    point_3d r;
    camera.get_position(&p);
    camera.get_rotation(&r);
  }

void destroy_scene()
  {
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

    srand(1000);

    // load textures:
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

    // make the sun:
    sun = make_sphere(50,10,10);
    sun->set_render_mode(RENDER_MODE_NO_LIGHT);
    sun->set_color(250,250,220);
    sun->set_position(-400,100,-400);

    // make the terrain:
    terrain = make_terrain(50,50,TERRAIN_HEIGHT,100,100,&terrain_heightmap);
    terrain->texture_map_plane(DIRECTION_DOWN,TERRAIN_TILE_FACTOR,TERRAIN_TILE_FACTOR);
    terrain->texture_map_layer_mask(&terrain_texturemap);
    terrain->set_lighting_properties(0.3,0.7,0.3,1.5);
    terrain->set_texture(&grass_texture);
    terrain->set_texture2(&sand_texture);

    // make the water animation of two frames:
    water_frame_0 = make_terrain(300,300,1,18,18,NULL);
    water_frame_0->texture_map_plane(DIRECTION_DOWN,20,20);

    water_frame_1 = new mesh_3d_static(water_frame_0);
    water_frame_1->update();

    for (i = 0; i < water_frame_0->vertex_count(); i++)    // deform the surface randomly so the water will move a little
      {
        water_frame_0->vertices[i].position.y += (rand() % 100) * 0.03;
        water_frame_1->vertices[i].position.y += (rand() % 100) * 0.03;
        water_frame_1->vertices[i].texture_coordination[0] += (rand() % 10 - 5) * 0.02;
        water_frame_1->vertices[i].texture_coordination[1] += (rand() % 10 - 5) * 0.02;
      }

    water_frame_0->update();
    water_frame_1->update();

    water = new mesh_3d_animated();

    water->add_frame(water_frame_0,2000);
    water->set_texture(&water_texture);
    water->add_frame(water_frame_1,2000);
    water->set_position(0,1,0);
    water->set_render_mode(RENDER_MODE_SHADED_GORAUD);
    water->set_lighting_properties(0.6,0.2,0.9,5);
    water->update();             // upload the animation data to GPU

    water->set_playing(true);    // play the animation

    water_static = make_plane(1000,1000,10,10);
    water_static->set_rotation(-90,0,0);
    water_static->set_position(0,-0.2,0);
    water_static->texture_map_plane(DIRECTION_FORWARD,50,50);
    water_static->set_texture(&water_texture);
    water_static->set_lighting_properties(0.6,0.2,0.9,5);

    // make the skybox:
    skybox = make_sphere(800,15,15);
    skybox->texture_map_plane(DIRECTION_FORWARD,3,3);
    skybox->flip_triangles();    // flip the sphere inside out as the camera will be inside
    skybox->set_render_mode(RENDER_MODE_NO_LIGHT);  // no shading for the sky
    skybox->set_texture(&sky_texture);

    camera.set_skybox(skybox);   // the skybox will follow camera movement now

    // make rocks:
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

    // make trees:
    tree_low = new mesh_3d_static();
    tree_low->load_obj("tree_low.obj");
    tree_low->set_texture(&tree_texture);

    tree = new mesh_3d_static();
    tree->load_obj("tree.obj");

    tree->set_scale(0.1);
    tree->set_texture(&tree_texture);

    float x0,y0,z0,x1,y1,z1,island_width,island_height,position_x,position_y,height;
    unsigned char r,g,b;
    terrain->get_bounding_box(&x0,&y0,&z0,&x1,&y1,&z1);

    island_width = x1 - x0;
    island_height = z1 - z0;

    for (i = 0; i < NUMBER_OF_TREES; i++)       // make trees
      {
        position_x = ((rand() % 100) * 0.01 - 0.5) * 0.8 + 0.5;
        position_y = ((rand() % 100) * 0.01 - 0.5) * 0.8 + 0.5;

        terrain->add_shadow((1 - position_x) * TERRAIN_TILE_FACTOR,(1 - position_y) * TERRAIN_TILE_FACTOR,0.25,-0.1);

        terrain_heightmap.get_pixel(position_x * terrain_heightmap.get_width(),position_y * terrain_heightmap.get_height(),&r,&g,&b);
        height = r / 255.0 * TERRAIN_HEIGHT - 0.7;

        position_x *= island_width;             // transform from normalised to world coords
        position_y *= island_height;

        position_x -= island_width / 2.0;
        position_y -= island_height / 2.0;

        trees[i] = new mesh_3d_lod(true,true);
        trees[i]->add_detail_mesh(tree,10);
        trees[i]->add_detail_mesh(tree_low,100);
        trees[i]->set_lighting_properties(0.4,0.3,0.0,1);
        trees[i]->set_scale(0.1 + (rand() % 10) * 0.005);
        trees[i]->set_rotation(rand() % 100 * 0.15,rand() % 1000 * 0.360,rand() % 10 * 0.15);

        trees[i]->set_position(position_x,height,position_y);
      }
  }

int main(int argc, char **argv)

{
  unsigned int i;

  init_opengl(&argc,argv,800,600,render_scene,"OpenglSE intro");

  go_fullscreen();

  init_scene();
  setup_camera_keyframes();

  set_mouse_visibility(false);

  set_perspective(110,0.01,1000);

  register_keyboard_function(keyboard_function);
  register_advanced_keyboard_function(keyboard_function2);

  camera.set_position(0,17,0);

  camera.rotation_speed = 0.025;

  rendering_started_at = get_time();
  render_loop();
  destroy_scene();
  return 0;
}
