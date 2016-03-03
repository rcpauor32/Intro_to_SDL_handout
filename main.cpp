// ----------------------------------------------------------------
// Awesome simple game with SDL
// http://wiki.libsdl.org/APIByCategory
// ----------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "time.h"

#include "SDL\include\SDL.h"
#include "SDL_image\include\SDL_image.h"
// TODO 1: Add SDL_mixer library
#include "SDL_mixer\include\SDL_mixer.h"
// You need to add the include file (.h)
// The library (.lib) and make sure all .dll
// are in the executable's folder. Everything 
// is in the folder file already.

#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )
//
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

// Globals --------------------------------------------------------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCROLL_SPEED 5
#define SHIP_SPEED 3
#define NUM_SHOTS 32
#define NUM_MISSILE_SHOTS 1
#define SHOT_SPEED 10
#define MISSILE_SHOT_SPEED 8
#define MAX_KEYS 300

enum KEY_STATE
{
	KEY_IDLE,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct projectile
{
	int x, y;
	bool alive;
};

struct globals
{
	SDL_Window* window;
	SDL_Surface* w_surface;
	SDL_Renderer* renderer;
	SDL_Texture* background;
	int background_width;
	SDL_Texture* ship;
	int ship_x;
	int ship_y;
	SDL_Texture* shot;
	SDL_Texture* m_shot, *m_explosive;
	projectile shots[NUM_SHOTS];
	projectile m_shots;
	int last_shot;
	bool fire, missile, up, down, left, right;
	bool m_explosion;
	KEY_STATE* keyboard;
	// TODO 4:
	// Add pointers to store music and the laser fx
	int scroll;

	Mix_Chunk* laserfx;
	Mix_Music* music;

	SDL_Rect mis_spr, m_expl_spr;
	int m_expl_counter;

} g;

// ----------------------------------------------------------------
void Start()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create window --
	g.window = SDL_CreateWindow("Super Awesome Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	g.w_surface = SDL_GetWindowSurface(g.window);

	// Create renderer --
	g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 255);

	// Load image lib --
	IMG_Init(IMG_INIT_PNG);
	g.background = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("background.png"));
	g.ship = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("ship.png"));
	g.shot = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("shot.png"));
	g.m_shot = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("LtR_sprites.png"));
	g.m_explosive = SDL_CreateTextureFromSurface(g.renderer, IMG_Load("sprites.png"));
	SDL_QueryTexture(g.background, NULL, NULL, &g.background_width, NULL);

	// Create mixer --
	// TODO 2:
	// Initialize the audio library 
	// Check https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer.html#SEC7

	int innited = Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024);

	// TODO 5:
	// Load music.ogg and play it. Store the pointer to it.
	
	g.music = Mix_LoadMUS("music.ogg");
	Mix_PlayMusic(g.music, -1);

	// Load the laser.wav file and store a pointer to it
	g.laserfx = Mix_LoadWAV("laser.wav");

	// Init other vars --
	g.scroll = 0;
	g.ship_x = 100;
	g.ship_y = SCREEN_HEIGHT / 2;
	g.fire = g.up = g.down = g.left = g.right = g.missile = false;
	g.last_shot = 0;
	g.keyboard = (KEY_STATE*)malloc(sizeof(KEY_STATE) * MAX_KEYS);
	memset(g.keyboard, KEY_IDLE, MAX_KEYS);

	g.mis_spr.x = 224;
	g.mis_spr.y = 64;
	g.mis_spr.h = 32;
	g.mis_spr.w = 32;

	g.m_expl_spr.x = 32;
	g.m_expl_spr.y = 0;
	g.m_expl_spr.h = 32;
	g.m_expl_spr.w = 32;

	g.m_expl_counter = 0;

}

// ----------------------------------------------------------------
void Finish()
{
	// TODO 6:
	// Stop and free the music and fx
	
	Mix_FreeMusic(g.music);
	Mix_FreeChunk(g.laserfx);

	// TODO 3:
	// Close the audio library
	
	Mix_Quit();
	Mix_CloseAudio();

	SDL_DestroyTexture(g.background);
	SDL_DestroyTexture(g.ship);
	IMG_Quit();
	SDL_DestroyRenderer(g.renderer);
	SDL_DestroyWindow(g.window);
	SDL_Quit();
	free(g.keyboard);
}

// ----------------------------------------------------------------
bool CheckInput()
{
	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// cyle keyboard keys
	for(int i = 0; i < MAX_KEYS; ++i)
	{	
		if(keys[i] == 1)
			g.keyboard[i] = (g.keyboard[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			g.keyboard[i] = (g.keyboard[i] == KEY_REPEAT || g.keyboard[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	if(g.keyboard[SDL_SCANCODE_ESCAPE] == KEY_DOWN)
		return false;

	g.up = g.keyboard[SDL_SCANCODE_UP] == KEY_REPEAT;
	g.down = g.keyboard[SDL_SCANCODE_DOWN] == KEY_REPEAT;
	g.left = g.keyboard[SDL_SCANCODE_LEFT] == KEY_REPEAT;
	g.right = g.keyboard[SDL_SCANCODE_RIGHT] == KEY_REPEAT;
	g.fire = g.keyboard[SDL_SCANCODE_SPACE] == KEY_DOWN;
	g.missile = g.keyboard[SDL_SCANCODE_Q] == KEY_DOWN;

	return true;
}

// ----------------------------------------------------------------
void MoveStuff()
{
	// Calc new ship position
	// TODO 8: move the ship
	// Knowing the booleans up/down/left/right
	// hold true if the player is trying to move in that direction
	// calc player's next position (stored in g.x and g.y)

	if (g.up && g.ship_y > 0) {
		g.ship_y -= 10;
	}

	if (g.down && g.ship_y < 710) {
		g.ship_y += 10;
	}

	if (g.left && g.ship_x > 0) {
		g.ship_x -= 10;
	}

	if (g.right && g.ship_x < 950) {
		g.ship_x += 10;
	}

	if (g.fire)
	{
		g.fire = false;

		Mix_PlayChannel(-1, g.laserfx, 0);

		if (g.last_shot == NUM_SHOTS)
			g.last_shot = 0;

		g.shots[g.last_shot].alive = true;
		g.shots[g.last_shot].x = g.ship_x + 32;
		g.shots[g.last_shot].y = g.ship_y;
		g.last_shot++;
	}

	for (int i = 0; i < NUM_SHOTS; ++i)
	{
		if (g.shots[i].alive)
		{
			if (g.shots[i].x < SCREEN_WIDTH)
				g.shots[i].x += SHOT_SPEED;
			else
				g.shots[i].alive = false;
		}
	}

	if (g.missile)
	{
		g.missile = false;

		if (!g.m_shots.alive) {
			Mix_PlayChannel(-1, g.laserfx, 0);
			g.m_shots.alive = true;
			g.m_shots.x = g.ship_x + 32;
			g.m_shots.y = g.ship_y;
		}

		else{
			g.m_explosion = true;
			g.m_shots.alive = false;
		}
	}

	if (g.m_shots.alive)
	{
		if (g.m_shots.x < SCREEN_WIDTH)
				g.m_shots.x += MISSILE_SHOT_SPEED;
		else
			g.m_shots.alive = false;
	}

	g.m_expl_counter++;

	if (g.m_expl_counter >= 30) {
		g.m_explosion = false;
		g.m_expl_counter = 0;
	}

}

// ----------------------------------------------------------------
void Draw()
{
	// Clear screen to black
	SDL_RenderClear(g.renderer);

	SDL_Rect target;

	// Draw the background and scroll --
	// TODO 9: scroll the background
	// you should move target.x over time
	// Remember that you have to draw the
	// background twice to fake repetition

	
	
	target.x = g.scroll;
	target.y = 0;
	target.w = g.background_width;
	target.h = SCREEN_HEIGHT;

	g.scroll -= 20;
	
	SDL_RenderCopy(g.renderer, g.background, NULL, &target);

	target.x = g.scroll + g.background_width;
	target.y = 0;
	target.w = g.background_width;
	target.h = SCREEN_HEIGHT;

	SDL_RenderCopy(g.renderer, g.background, NULL, &target);

	if (g.scroll < -(g.background_width * 2))
		g.scroll = 0;

	// Draw the ship --
	target.x = g.ship_x;
	target.y = g.ship_y;
	target.w = 64;
	target.h = 64;
	SDL_RenderCopy(g.renderer, g.ship, NULL, &target);

	// Draw lasers --
	target.w = 64;
	target.h = 64;

	for(int i = 0; i < NUM_SHOTS; ++i)
	{
		if(g.shots[i].alive)
		{
			target.x = g.shots[i].x;
			target.y = g.shots[i].y;
			SDL_RenderCopy(g.renderer, g.shot, NULL, &target);
			
		}
	}

	if (g.m_explosion) {
		target.x = g.m_shots.x;
		target.y = g.m_shots.y;
		SDL_RenderCopy(g.renderer, g.m_explosive, &g.m_expl_spr, &target);
	}

	if (g.m_shots.alive)
	{
		target.x = g.m_shots.x;
		target.y = g.m_shots.y;
		SDL_RenderCopy(g.renderer, g.m_shot, &g.mis_spr , &target);

	}

		


	// Finally present framebuffer
	SDL_RenderPresent(g.renderer);
}

// ----------------------------------------------------------------
int main(int argc, char* argv[])
{
	Start();

	while(CheckInput())
	{
		MoveStuff();
		Draw();
	}

	Finish();
	return(EXIT_SUCCESS);
}

// TODO 7: Play the laser fx 
// (you need to find where this should happen)