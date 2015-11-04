////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include <fstream>
#include <sstream>
namespace octet {

  class MichaelCraddockswork : public app {
  
    ref<visual_scene> app_scene;
	btDefaultCollisionConfiguration config;       /// setup for the world
	btCollisionDispatcher *dispatcher;            /// handler for collisions between objects
	btDbvtBroadphase *broadphase;                 /// handler for broadphase (rough) collision
	btSequentialImpulseConstraintSolver *solver;  /// handler to resolve collisions
	btDiscreteDynamicsWorld *world;

	string content;
  public:
   
    MichaelCraddockswork(int argc, char **argv) : app(argc, argv) {
    }
	void loadlevel(){
		
		std::fstream levelfile;
		std::stringstream filename;
		filename << "testlevel" << ".txt";
		levelfile.open(filename.str().c_str(), std::ios::in);
		if (!levelfile.is_open()){
			printf("The file isn't here, try again.\n");
		}
		else{

			printf("file is open\n");
			levelfile.seekg(0, levelfile.end);
			int length = levelfile.tellg();
			levelfile.seekg(0, levelfile.beg);

			std::stringstream file;
			file.str(std::string());
			file << levelfile.rdbuf();
			levelfile.close();

			content = file.str().c_str();
			printf("%s \n", content.c_str());
			Buildlevel();
		}

		}
	void Buildlevel(){


	}

	void Camera(){

		int vx = 0,vy = 0;
		int x, y;
		get_viewport_size(vx, vy);
		get_mouse_pos(x, y);

		mat4t modeltoworld;

		modeltoworld.loadIdentity();
		modeltoworld.translate(-25.0f, 12.5f, -5.0f);
		modeltoworld.rotateY((float)-x*2.0f);
		if (vy / 2 - y < 40 && vy / 2 - y > -40)
			modeltoworld.rotateX(vy / 2 - y);
		else if (vy / 2 - y >= 40)
			modeltoworld.rotateX(40);
		else if (vy / 2 - y <= 40)
			modeltoworld.rotateX(-40);

		app_scene->get_camera_instance(0)->get_node()->access_nodeToParent() = modeltoworld;
	}
  
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  material *red = new material(vec4(1, 0, 0, 1));
	  mesh_box *box = new mesh_box(vec3(4));
	  scene_node *node = new scene_node();
	  app_scene->add_child(node);
	  app_scene->add_mesh_instance(new mesh_instance(node, box, red));
	  loadlevel();
	
    }

  
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
	  scene_node *node = app_scene->get_mesh_instance(0)->get_node();
	  node->rotate(1, vec3(1, 0, 0));
	  node->rotate(1, vec3(0, 1, 0));
	  Camera();
    }
  };
}
