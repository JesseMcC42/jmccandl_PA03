#pragma once

#include "MySprite.h"
#include "mathtool/Point.h"

#include "MyBehaviorTree.h"

namespace GMUCS425
{
    //this defines the transform of the agent
    class MyAgent
    {
      public:

        //movable: true if this agent can move
        //collision: true if this agent can collision with other agents
        MyAgent(char c, bool movable=true, bool collision=true)
        {
          x=y=degree=0;
		  trueX = trueY = 0;
          scale=1;
          visible=true;
          sprite=NULL;
          this->movable=movable;
          this->collision=collision;
		  this->c= c;

		  left = false;
		  collide_with = NULL;
		  behavior = NULL;

        }

		virtual char getC() { return c; };

        //react to the events
        virtual void handle_event(SDL_Event & e);

        //update this agent's motion, looks, sound, etc
        virtual void update();

        //render this agent
        virtual void display();

        //show HUD (heads-up display) or status bar
        virtual void draw_HUD();

		int getCollisionTimer() { return collision_free_timer; }
		void setCollisionTimer(int x) { this->collision_free_timer = x; }

		int getCollision() { return collision_timer; }
		void setCollision(int x) { this->collision_timer = x; }

        //transforms
        void rotate(float degree){ this->degree+=degree; }
        void rotateTo(float degree){ this->degree=degree; }

        void tranlate(float x, float y){ this->x+=x; this->y+=y; }
        void tranlateTo(float x, float y){ this->x=x; this->y=y; }

        void scaleTo(float s){ this->scale=s; }

		void setLeft(bool x) { this->left = x; }
		bool getLeft() { return left; }

		void setX(float x) { this->x = x; }

        //display
        void show(){ visible=true; }
        void hide(){ visible=false; }

        //sprite, aks costume
        void setSprite(MySprite * sprite){ this->sprite=sprite; }
		MySprite* getSprite() { return this->sprite;}

		float getScale() { return this->scale; }

        //motion/animation
        //void glide(float x, float y, float seconds);

        bool collide(MyAgent * other);
		MyAgent* getCollideWith() { return this->collide_with; }

        //ACCESS METHODS
        bool is_movable() const { return movable; }
        bool is_visible() const { return visible; }

        float getX() const { return x; }
        float getY() const { return y; }
        float getAngle() const { return degree; }
        float getScale() const { return scale; }
		void setTrue(float x, float y) { trueX = x; trueY = y; }

		float OGX() { return trueX; }
		float OGY() { return trueY; }

      protected:

        void draw_bounding_box();

		char c;
		bool left;
		MyAgent* collide_with;
		MyBehaviorTree* behavior; //NEW, build this!

		int collision_free_timer = 10;
		int collision_timer = 15;

        //current position and orientations
		float x, y, trueX, trueY;
        float degree; //rotation
        float scale;

			bool has_goal;
    		bool visible;
    		bool movable;
    		bool collision;

        MySprite * sprite; //current sprite
        //it is possible that you can have more than one sprites
        //vector<MySprite *> sprites; //the sprites
    };

    class MyHumanAgent : public MyAgent
    {
      public:
        MyHumanAgent(char c, bool movable=true, bool collision=true)
        :MyAgent(c, movable,collision){orig_x=INT_MAX; left=true; collide_with=NULL;}
        virtual void update();
        virtual void display();
        virtual void handle_event(SDL_Event & e);
      private:
        int orig_x;
        bool left;
    };


    class MyDogAgent : public MyAgent
    {
      public:
        MyDogAgent(char c, bool movable=true, bool collision=true):MyAgent(c, movable,collision)
        {radius=FLT_MAX; center_x=center_y=INT_MAX; ccw=false; collide_with=NULL;}
        virtual void update();
        virtual void display();
        virtual void handle_event(SDL_Event & e);
		int getTimer() { return this->sound_timer; }
		void setTimer(int x) { this->sound_timer = x; }
		int getOrigin() { return orig_x; }
		void setOrigin(float x) { this->orig_x = x; }
	protected:
		void build_behavior_tree();
		int sound_timer;
		int orig_x;
	  private:
		int collision_timer = 15;
        float radius;
        int center_x, center_y;
        bool ccw;
        MyAgent * collide_with;
        int collision_free_timer=10;
    };

}//end namespace
