#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "PathFont.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

#include <hb.h>
#include <hb-ot.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

void setfont(char* test, glm::vec3 anchor,glm::uvec2 const &drawable_size, glm::vec4 color, float boxwidth, float boxheight, bool scrolldialogue, int &scrolldialogueindex) //display text of the specified index
{
	glDisable(GL_DEPTH_TEST);
	float aspect = float(drawable_size.x) / float(drawable_size.y);
	DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	));

	char *fontfile;
	char *text;
	
	const float pixelwidth = 64.0f;
	const float resolution = 1024.0f;
	const float beglinex = anchor.x;
	const float begliney = anchor.y;

	fontfile = "PressStart.ttf";
	text = test;


	FT_Library  library;   /* handle to library     */
	FT_Face     face;  
	 
	FT_Init_FreeType( &library );
	FT_New_Face( library, fontfile, 0, &face );
	
	FT_Set_Pixel_Sizes( face, uint8_t(pixelwidth), uint8_t(pixelwidth));   /* pixel_height          */
	FT_GlyphSlot  slot = face->glyph;

	FT_Select_Charmap(
          face,               /* target face object */
          FT_ENCODING_BIG5 );

	//Look into freetype
	
	/* And converted to absolute positions. */
	{
		
		for (int i = 0; i < strlen(test); i++)
		{
			if(!scrolldialogue)
			{
				FT_Load_Char(face,test[i], FT_LOAD_RENDER); //load the character
				if(test[i] == '\n') //newline!
				{
					if((i != strlen(test) - 1))
					{
						anchor.y -= (1/resolution *pixelwidth)*1.0f;
						anchor.x = beglinex;
					}
					
					FT_Load_Char( face, 32 , FT_LOAD_RENDER); //load a space instead
					
				}

				
				float origX = anchor.x;
				float origY = anchor.y;
				
				for(int y = (slot->bitmap.rows - 1); y >= 0 ; y--)
				{
					for(uint32_t x = 0; x < (slot->bitmap.width); x++)
					{
						if(slot->bitmap.buffer[y*slot->bitmap.width+ x])
						{
							glm::vec3 anchordup1 = anchor;
							glm::vec3 anchordup2 = anchor;
							glm::vec3 anchordup3 = anchor;
							anchordup1.x += 1/resolution;
							anchordup2.y += 1/resolution;
							anchordup3.x += 1/resolution;
							anchordup3.y += 1/resolution;

							glm::mat4x3 mat(anchor,anchordup1,anchordup2,anchordup3);
							
							lines.draw_rect(mat,color);
						}
						
						
						anchor.x += 1/resolution;
						
					}
					anchor.y += 1/resolution;
					anchor.x = origX;
					//std::cout << "\n";
				}

				anchor.x += (1/resolution *slot->bitmap.width)*1.0f + 1/resolution * 4.0f;
				anchor.y = origY;

				//if we wanted to shake, move y in a sinusoid function

				if(test[i] == ' ') //space!
				{
					anchor.x += (1/resolution *pixelwidth)/2.0f;	
					
				}

				if(abs(anchor.x) - abs(beglinex)> boxwidth) //if we have extended past the width of the box
				{
					anchor.y -= (1/resolution *pixelwidth)*1.0f;
					anchor.x = beglinex;	
				}

				
				if(abs(anchor.y)- abs(begliney)> boxheight) //if we have extended past the height of the box
				{
					//set pause render at the index of the string
					//when space is pressed resume at the index
					
					scrolldialogue = true;
					scrolldialogueindex = i;
					anchor.y = begliney;
					anchor.x = beglinex;
				}
	
			}
		}
		scrolldialogue = true;
	}

	FT_Done_Face    ( face );
  	FT_Done_FreeType( library );

}

GLuint guppy_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > guppy_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("guppy.pnct"));
	guppy_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > guppy_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("guppy.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = guppy_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = guppy_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

PlayMode::PlayMode() : scene(*guppy_scene) {
	//get pointers to leg for convenience:
//for (auto &transform : scene.transforms) {
		//if (transform.name == "Hip.FL") hip = &transform;
	
	//}


	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	

	guppy.Populate_NPC_Tree(data_path("Guppy_Dialogue.txt"));
	guppy.Populate_Player_Tree(data_path("Player-Guppy_Dialogue.txt"));
	guppy.Populate_Affection_Tree(data_path("Guppy_Affection.txt"));
	guppy.Set_Name("[Guppy]");
	
	
}


PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		}  else if (evt.key.keysym.sym == SDLK_RIGHT) {
			hright.downs += 1;
			hright.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			hleft.downs += 1;
			hleft.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		}  else if (evt.key.keysym.sym == SDLK_LEFT) {
			hleft.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			hright.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}


void PlayMode::scene1(float elapsed) {
	
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	timer += elapsed * speed;
	spacepressed += elapsed;
	arrowpressed += elapsed;
	wobble -= std::floor(wobble);

	if(space.pressed && spacepressed >=0.25f)
	{	
		if(revealtext || (int(timer) >= guppy.get_NPC_Dialogue().length()))//text has been revealed,
		{
			timer = 0.0f;
			guppy.Update_Index(selectdialogue);
			revealtext = false;
		}
		else
		{
			revealtext = !revealtext;
		}
		
		spacepressed = 0.0f;
	}

	if(hleft.pressed && arrowpressed >=0.25f)
	{
		selectdialogue =  0;
		arrowpressed = 0.0f;
	}
	if(hright.pressed && arrowpressed >=0.25f)
	{
		selectdialogue =  1;
		arrowpressed = 0.0f;
	}
	

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = -frame[2];

		camera->transform->position += move.x * frame_right + move.y * frame_forward;
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs= 0;
	hright.downs = 0;
	hleft.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f,1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.65f, 0.8f, 1.0f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);
	
	
	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		constexpr float H = 0.09f;
		glm::vec4 textcolor(0x00,0x00,0x00,0xff); //black text color
		glm::vec4 highlightcolor(0xff,0xff,0xff,0xff); //white text color
		
		int noscroll = 0;
		
		//draw NPC text
		if(!guppy.get_NPC_Dialogue().empty())
			NPCline = guppy.get_NPC_Dialogue();
		else
			lockdialogue=true;
		
		setfont(&guppy.get_Name()[0],glm::vec3(-aspect/2.0f - 0.2f * H, -0.4f + 0.1f * H, 0.0), drawable_size,textcolor, boxwidth, boxheight,scrolldialogue,noscroll);
		if(!revealtext && !lockdialogue) //load the dialogue letter by letter
		{		
			std::vector<char> temp;
			for(int i = 0; i < int(timer); i ++)
			{
				if(i > NPCline.length())
				break;
				temp.emplace_back(NPCline[i]);
			}
			temp.emplace_back('\0');
			setfont(&temp[scrolldialogueindex],glm::vec3(-aspect/2.0f - 0.2f * H, -0.5f + 0.1f * H, 0.0), drawable_size,textcolor, boxwidth, boxheight,scrolldialogue,scrolldialogueindex);
		}
		else
		setfont(&NPCline[scrolldialogueindex],glm::vec3(-aspect/2.0f - 0.2f * H, -0.5f + 0.1f * H, 0.0), drawable_size,textcolor, boxwidth, boxheight,scrolldialogue,scrolldialogueindex); 
		//load the dialogue at once

		//draw Player text
		if(revealtext || (int(timer) >= guppy.get_NPC_Dialogue().length()) ) //if NPC text has shown up
		{
			Playerline = guppy.get_Player_Dialogue();
			
			if(Playerline.size() == 1) //only one choice
			setfont(&(Playerline[0])[0],glm::vec3(-aspect/2.0f - 0.2f * H, -0.5f + 0.1f * H, 0.0), drawable_size,highlightcolor, boxwidth, boxheight,scrolldialogue,noscroll);
			else if(Playerline.size() == 2) //two choices
			{
				if(selectdialogue == 0) //change color of one based on select dialogue
				{
					setfont(&(Playerline[0])[0],glm::vec3(-1.75f, -0.75f + 0.1f * H, 0.0), drawable_size,highlightcolor, boxwidth, boxheight,scrolldialogue,noscroll);
					setfont(&(Playerline[1])[0],glm::vec3(0.25f, -0.75f + 0.1f * H, 0.0), drawable_size,textcolor, boxwidth, boxheight,scrolldialogue,noscroll);
				}
				else if(selectdialogue == 1)
				{
					setfont(&(Playerline[0])[0],glm::vec3(-1.75f, -0.75f + 0.1f * H, 0.0), drawable_size,textcolor, boxwidth, boxheight,scrolldialogue,noscroll);
					setfont(&(Playerline[1])[0],glm::vec3(0.25f, -0.75f + 0.1f * H, 0.0), drawable_size,highlightcolor, boxwidth, boxheight,scrolldialogue,noscroll);
				}
			}
		}
	}
	GL_ERRORS();
}

glm::vec3 PlayMode::get_leg_tip_position() {
	//the vertex position here was read from the model in blender:
	return lower_leg->make_local_to_world() * glm::vec4(-1.26137f, -11.861f, 0.0f, 1.0f);
}
