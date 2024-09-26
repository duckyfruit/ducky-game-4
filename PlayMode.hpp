#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "DialogueTree.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void scene1(float elapsed);
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space, hleft, hright;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	int scenenum = 0;

	float timer= 0.0f;
	float speed = 15.0f;
	
	bool revealtext = false;
	bool lockdialogue = false;
	
	bool scrolldialogue = false;
	int scrolldialogueindex = 0;

	int selectdialogue = 0;

	float spacepressed = 0.0f;
	float arrowpressed = 0.0f;

	float boxwidth = 1.125f; //width and height of player dialogue box
	float boxheight = 0.1f;

	DialogueTree guppy;
	std::string NPCline;
	std::vector<std::string>Playerline;

	//hexapod leg to wobble:
	Scene::Transform *hip = nullptr;
	Scene::Transform *upper_leg = nullptr;
	Scene::Transform *lower_leg = nullptr;
	glm::quat hip_base_rotation;
	glm::quat upper_leg_base_rotation;
	glm::quat lower_leg_base_rotation;
	float wobble = 0.0f;

	glm::vec3 get_leg_tip_position();

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
