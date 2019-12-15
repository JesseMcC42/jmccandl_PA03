#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <SDL_mixer.h>

#ifdef _WIN32
#undef main
#endif

#include <iostream>
#include <string>

#include "MyTexture.h"
#include "MyScene.h"

namespace GMUCS425
{

	class MyGame; //define later in this file
	MyGame * getMyGame();
	void setMyGame(MyGame * game);

	class MyGame
	{

	public:

		MyGame(int width=640, int height=480)
		{
			gWindow=NULL;
			gRenderer=NULL;
			SCREEN_WIDTH=width;
			SCREEN_HEIGHT=height;
		}

		virtual ~MyGame(){ close(); }

		//Starts up SDL and creates window
		virtual bool init(const std::string name);

		//Loads media
		virtual bool loadMedia(const std::string & config);

		virtual bool loadLoad();
		virtual bool displayWin();
		virtual bool playAgainQ();

		//the game loop!
		virtual void run();

		//Frees media and shuts down SDL
		virtual void close();

		int getIntro() { return intro; }
		void setIntro(int val) { intro = val; }

		void handle_event(SDL_Event& e);

		//access methods
		SDL_Window* getWindow(){ return gWindow; }
		SDL_Renderer * getRenderer() { return gRenderer; }
		MyTextureManager * getTextureManager() { return &m_TextureManager; }
		MySpriteManager * getSpriteManager(){ return &m_SpriteManager; }
		MySceneManager * getSceneManager() { return &m_SceneManager; }
		MySceneManager* getSceneManagerl() { return &m_SceneManagerL; }
		MySceneManager* getSceneManagerW() { return &m_SceneManagerW; }
		MySceneManager* getSceneManagerF() { return &m_SceneManagerF; }

		void resizeWidow(int w, int h);
		int getScreenWidth() const { return SCREEN_WIDTH; }
		int getScreenHeight() const { return SCREEN_HEIGHT; };


		Mix_Music* BARK;
		Mix_Music* MUSIC;

	protected:

		int intro = 0;

		//Screen dimension constants
		int SCREEN_WIDTH = 640;
		int SCREEN_HEIGHT = 480;

		//The window we'll be rendering to
		SDL_Window* gWindow;

		//The window renderer
		SDL_Renderer* gRenderer;

		MyTextureManager m_TextureManager;
		MySpriteManager m_SpriteManager;
		MySceneManager m_SceneManager;
		MySceneManager m_SceneManagerL;
		MySceneManager m_SceneManagerW;
		MySceneManager m_SceneManagerF;


		//for future assignments!
		//MySoundManager m_SoundManager;
		//MyFontManager m_FontManager;
	};

}//end namespace GMUCS425
