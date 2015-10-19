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
  
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  loadlevel();
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
