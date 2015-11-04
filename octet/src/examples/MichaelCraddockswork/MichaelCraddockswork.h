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

	dynarray<btRigidBody*> rigid_bodies;
	dynarray<scene_node*> nodes;
	mesh_box *box;
	material *wall, *floor ,*target;

	mat4t worldcoord;
	//scene_node *cam;

	string content;
  public:
   
    MichaelCraddockswork(int argc, char **argv) : app(argc, argv) {

		dispatcher = new btCollisionDispatcher(&config);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
    }
	~MichaelCraddockswork(){
		delete world;
		delete dispatcher;
		delete solver;
		delete broadphase;
	}

	/*void newscene(){
		app_scene->reset();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->set_far_plane(1000);
		cam = app_scene->get_camera_instance(0)->get_node();
		cam->translate(vec3(24, -24, 50));
		Camera();
	} */
	// in this section we add a mesh and a rigid body
	void add_mesh(mat4t_in coord, mesh *shape, material *mat, bool is_dynamic){
		
		scene_node *node = new scene_node();
		node->access_nodeToParent() = coord;
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, shape, mat));

		btMatrix3x3 matrix(get_btMatrix3x3(coord));
		btVector3 pos(get_btVector3(coord[3].xyz()));

		btCollisionShape *collisionshape = shape->get_bullet_shape();

		if (collisionshape){

			btTransform transform(matrix, pos);
			btDefaultMotionState *motionstate = new btDefaultMotionState(transform);
			btScalar mass = is_dynamic ? 0.5f : 0.5f;
			btVector3 inertiaTensor;

			collisionshape->calculateLocalInertia(mass, inertiaTensor);

			btRigidBody *rigid_body = new btRigidBody(mass, motionstate, collisionshape, inertiaTensor);
			world->addRigidBody(rigid_body);
			rigid_bodies.push_back(rigid_body);
			rigid_body->setUserPointer(node);

		}
		
		

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

		vec3 pos = vec3(0, 0, 0);
		int x = 0;
		for (int i = 0; i < content.size(); i++){

			scene_node *node = new scene_node;
			char c = content[i];
			switch (c)
			{
			case '\n': pos -= (vec3(x, 0, 0));
				x = 0;
				pos += vec3(1, 0, 0);
				break;
			case' ':
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case'-': worldcoord.translate(pos);
				add_mesh(worldcoord, box, floor, false);
				rigid_bodies.back()->setFriction(0);
				rigid_bodies.back()->setRestitution(0);
				worldcoord.loadIdentity();
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case'X': worldcoord.translate(pos);
				add_mesh(worldcoord, box, target, false);
				rigid_bodies.back()->setFriction(0);
				rigid_bodies.back()->setRestitution(0);
				worldcoord.loadIdentity();
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case'|': worldcoord.translate(pos);
				add_mesh(worldcoord, box, wall, false);
				rigid_bodies.back()->setFriction(0);
				rigid_bodies.back()->setRestitution(0);
				worldcoord.loadIdentity();
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			default: break;
			}
		}

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

	  app_scene = new visual_scene();
	  app_scene->create_default_camera_and_lights();
	  box = new mesh_box(0.5f);
	  wall = new material(vec4(1, 0, 0, 1));
	  floor = new material(vec4(0, 1, 0, 1));
	  target = new material(vec4(1, 1, 0, 1));
	  loadlevel();
	  Camera();
	  
	  
	
    }

  
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
	  
    }
  };
}
