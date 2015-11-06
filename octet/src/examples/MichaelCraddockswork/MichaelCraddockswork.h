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
	material *wall, *floor ,*target, *bullet, *faketar, *block;

	mat4t worldcoord;
	bool isKeyDown;
	string content;
	enum {
		BULLET = 0, FLOOR = 1, TARGET = 2, FAKETARGET = 3, num_sound_sources = 8
	};
	ALuint Hit;
	ALuint Shot;
	ALuint Faketarget;
	unsigned cur_source;
	ALuint sources[num_sound_sources];
	ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }

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

	// in this section we add a mesh and a rigid body
	void add_mesh(mat4t_in coord, mesh *shape, material *mat, bool is_dynamic = true){
		
		scene_node *node = new scene_node();
		node->access_nodeToParent() = coord;
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, shape, mat));
		nodes.push_back(node);

		btMatrix3x3 matrix(get_btMatrix3x3(coord));
		btVector3 pos(get_btVector3(coord[3].xyz()));

		btCollisionShape *collisionshape = shape->get_bullet_shape();

		if (collisionshape){

			btTransform transform(matrix, pos);
			btDefaultMotionState *motionstate = new btDefaultMotionState(transform);
			btScalar mass = is_dynamic ? 1.0f : 0.0f;
			btVector3 inertiaTensor;

			collisionshape->calculateLocalInertia(mass, inertiaTensor);

			btRigidBody *rigid_body = new btRigidBody(mass, motionstate, collisionshape, inertiaTensor);
			world->addRigidBody(rigid_body);
			rigid_bodies.push_back(rigid_body);
			rigid_body->setUserPointer(node);

		}
	}
	void add_bullet(mat4t_in modelToWorld, vec3_in size, material *mat, int index, bool is_dynamic = true) {

		btMatrix3x3 matrix(get_btMatrix3x3(modelToWorld));
		btVector3 pos(get_btVector3(modelToWorld[3].xyz()));

		btCollisionShape *shape = new btBoxShape(get_btVector3(size));

		btTransform transform(matrix, pos);

		btDefaultMotionState *motionState = new btDefaultMotionState(transform);
		btScalar mass = is_dynamic ? 1.0f : 0.0f;
		btVector3 inertiaTensor;

		shape->calculateLocalInertia(mass, inertiaTensor);

		btRigidBody * rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
		world->addRigidBody(rigid_body);
		rigid_bodies.push_back(rigid_body);
		// this is where we make the bullet follow the camera so that it shoots where the camera is looking.
		vec3 dir(0, 0, -1);
		quat rot = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().toQuaternion();
		dir = dir * (mat4t)rot;
		rigid_body->applyCentralForce(get_btVector3(dir * 2500));
		rigid_body->setUserIndex(index);


		mesh_box *box = new mesh_box(size);
		scene_node *node = new scene_node(modelToWorld, atom_);
		nodes.push_back(node);

		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, box, mat));
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
				pos += vec3(0, -1, 0);
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
				add_mesh(worldcoord, box, target, true);
				rigid_bodies.back()->setUserIndex(TARGET);
				rigid_bodies.back()->setFriction(0);
				rigid_bodies.back()->setRestitution(0);
				worldcoord.loadIdentity();
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case'F': worldcoord.translate(pos);
				add_mesh(worldcoord, box, block, false);
				rigid_bodies.back()->setFriction(0);
				rigid_bodies.back()->setRestitution(0);
				worldcoord.loadIdentity();
				x += 1;
				pos += vec3(1, 0, 0);
				break;
			case'N': worldcoord.translate(pos);
				add_mesh(worldcoord, box, faketar ,false);
				rigid_bodies.back()->setUserIndex(FAKETARGET);
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
		modeltoworld.translate(15, -12, 30);
		modeltoworld.rotateY((float)-x*2.0f);
		if (vy / 2 - y < 40 && vy / 2 - y > -40)
			modeltoworld.rotateX(vy / 2 - y);
		else if (vy / 2 - y >= 40)
			modeltoworld.rotateX(40);
		else if (vy / 2 - y <= 40)
			modeltoworld.rotateX(-40);

		app_scene->get_camera_instance(0)->get_node()->access_nodeToParent() = modeltoworld;
		if (is_key_down(key_lmb)){
			// This checks if the mouse button has been pressed down
			if (isKeyDown == true){
			}
			//This creates the box within the first frame and registers that the key is down
			else{
				ALuint source = get_sound_source();
				alSourcei(source, AL_BUFFER, Shot);
				alSourcePlay(source);
				modeltoworld.translate(0, 0, -1);
				add_bullet(modeltoworld, vec3(0.1f), bullet, BULLET);
				isKeyDown = true;
			}
		}
		else {
			isKeyDown = false;
		}
	}
  
    void app_init() {

	  app_scene = new visual_scene();
	  app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->get_node();
	  box = new mesh_box(0.5f);
	  wall = new material(vec4(1, 0, 0, 1));
	  floor = new material(vec4(0, 1, 0, 1));
	  target = new material(vec4(1, 1, 0, 1));
	  bullet = new material(vec4(1, 0, 0, 1));
	  faketar = new material(vec4(1, 0, 0, 1));
	  block = new material(vec4(1, 0, 1, 1));
	  isKeyDown = false;
	  loadlevel();
    }

  
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  int numManifolds = world->getDispatcher()->getNumManifolds();
	  //if (true) printf("------new physics step--------\n");
	  for (int i = 0; i < numManifolds; i++){
		  btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		  const btCollisionObject* rgd1 = contactManifold->getBody0();
		  const btCollisionObject* rgd2 = contactManifold->getBody1();
		  int objA = contactManifold->getBody0()->getUserIndex();
		  int objB = contactManifold->getBody1()->getUserIndex();
		  //here we check the collisions and print them to make sure they work
		  if (objA == TARGET && objB == BULLET){
			  printf("A bullet has hit a target\n");
		  }
		  if (objA == FAKETARGET && objB == BULLET){
			  printf("A bullet has hit a fake target\n");
		  }
		  //if the bullet collides with the target play sound
		  if (objA == TARGET && objB == BULLET){

			  for (int i = 0; i < rigid_bodies.size(); ++i){
				  if (rigid_bodies[i] == rgd1)
				  {
					  ALuint source = get_sound_source();
					  alSourcei(source, AL_BUFFER, Hit);
					  alSourcePlay(source);				
				  }
			  }
		  }
		  if (objA == BULLET && objB == TARGET){

			  for (int i = 0; i < rigid_bodies.size(); ++i){
				  if (rigid_bodies[i] == rgd2){
					  ALuint source = get_sound_source();
					  alSourcei(source, AL_BUFFER, Hit);
					  alSourcePlay(source);
				  }
			  }
		  }
		  if (objA == FAKETARGET && objB == BULLET){

			  for (int i = 0; i < rigid_bodies.size(); ++i){
				  if (rigid_bodies[i] == rgd1)
				  {
					  ALuint source = get_sound_source();
					  alSourcei(source, AL_BUFFER, Faketarget);
					  alSourcePlay(source);
				  }
			  }
		  }
		  if (objA == BULLET && objB == FAKETARGET){

			  for (int i = 0; i < rigid_bodies.size(); ++i){
				  if (rigid_bodies[i] == rgd2){
					  ALuint source = get_sound_source();
					  alSourcei(source, AL_BUFFER, Faketarget);
					  alSourcePlay(source);
				  }
			  }
		  }
	  }


	  world->stepSimulation(1.0f / 30);
	  for (unsigned i = 0; i != rigid_bodies.size(); ++i) {
		  btRigidBody *rigid_body = rigid_bodies[i];
		  btQuaternion btq = rigid_body->getOrientation();
		  btVector3 pos = rigid_body->getCenterOfMassPosition();
		  quat q(btq[0], btq[1], btq[2], btq[3]);
		  mat4t modelToWorld = q;
		  modelToWorld[3] = vec4(pos[0], pos[1], pos[2], 1);
		  nodes[i]->access_nodeToParent() = modelToWorld;
	  }
	  Hit = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/Michaels work sounds/Hit.wav");
	  Shot = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/Michaels work sounds/Shot.wav");
	  Faketarget = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/Michaels work sounds/Faketarget.wav");
	  cur_source = 0;
	  alGenSources(num_sound_sources, sources);

      app_scene->update(1.0f/30);
	  Camera();

      // draw the scene
      app_scene->render((float)vx / vy);
	  
    }
  };
}
