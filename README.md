# octet

HONROABLE MENTIONS
Baggio Pereira: He helped me quite a bit early on, helping me with the building of the level. He also helped me figure out some of the physics issues I was having midway through and honestly just was incredibly helpful throughout this project.
Michael Craddock Tools and middleware coursework 1
In this document we’ll go over what work has been done for this piece of coursework, showing off the code used and what it is being used for and how. Difficulties encountered in the project and things that could have either been changed or done differently.
To start off the project in its core shows off a level being read in and loaded into the scene via a text file, and collision detection that detects when two objects collide and then reacts accordingly. The layout of the project is that of a target range which allows the player to move the camera around and shoot at objects that have been loaded in via a text file. 
To start things off we’ll go over the loading of the text file in. To start this off we need to firstly need to read in the file.  To do this we reference the textfile and then open the file and take the content and store it. This will then be used when building the level.
 
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
As you can see we print the contents to make sure that it has properly loaded, this is to show us that the level has indeed been read in correctly and displays as intended. Prior to building the level this was used to confirm the file could be read in and loaded to read properly.
The second thing we do is then build in the level; we do this by going through an iteration stage where we take the symbols used in the text file and give them a definition and attach meshes to that character. So for example in this code we use a few different characters that all build into different things
_ = a part of the floor
| = a part of the wall
X = a target
N = a fake target
F = a regular block
The code will go through the text file reading these characters and then it’ll build them into the scene. We created an add mesh function earlier that creates a mesh and adds a rigid body to it, we call upon that function when creating these blocks and use them to craft our level.
Collision detection is done by assigning each unique block an enum to them so that it is easy to reference them later on in the code.  We then use the enums to tell what is colliding with what, and then when they collide with each other it’ll print out that they have collided and then it will also load sound files from the assets folder and play the sound on collision.
One of the main features is being able to shoot, this was done simply by creating a function for the bullet similar to creating the mesh, but adding an extra bit of code that tracks the direction and rotation of the camera and fires the bullet in the direction the camera is facing. The actual firing of the bullet is handled in the camera code by checking if the left mouse button is down, and if it is it calls upon the bullet function and spawns in the bullet and plays sound at the same time. One issue that was confusing for a long period of time was the frame rate of the project would drop fairly low once the camera function was added. For a while we could not figure out why this was, however it turned out to be because the material for the bullet was being called and created in the camera function rather than in app_init like all the other materials, once I rearranged the code the frame rate shot up.

There are a lot of things that could have been done differently and things that we wanted to add that we couldn’t either due to time or to lack of knowledge.  These things include Hinges, limited shots, and a score board. We couldn’t quite figure out how to implement hinges using the loading system in place. This is one of the things that were most desired to go into this project but I could not figure out how to implement it using the code available to me at this time.
The scoreboard and limited shots were a very optional thing that would just add a little bit more to the project but again due to time and uncertainty of how to implement it these were not added in.
In conclusion whilst disappointed with the fact that hinges were not added in time, I’m happy with the way the project has turned out and very happy that the frame rate issue that was plaguing this project was fixed as well. If this project could be redone hinges would probably be added and checked earlier and possibly refining the camera controls would be another thing to look into for this project.

