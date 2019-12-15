#pragma once

#include "MyAgent.h"
#include "mathtool/Point.h"

namespace GMUCS425
{
    class MyPathPlanner; //define in MyPathPlanner.h

    class MyCatAgent : public MyAgent
    {
    public:
      MyCatAgent(char c, bool movable=true, bool collision=true):MyAgent(c, movable,collision)
	  {
		  dx = dy = 0; collide_with = NULL; has_goal = left = false; planner = NULL; lives = 9; balls=0;
	  }

      virtual void handle_event(SDL_Event & e);
      virtual void update();
      virtual void draw_HUD();
      //render this agent
      virtual void display();

    private:

      void move_to_next_waypoint();
      void pathing();

      //display goal and path if there is any
      void draw_goal_and_path();

      //path planning related data
      bool has_goal;
      mathtool::Point2d goal;
      std::list<mathtool::Point2d> path;
      MyPathPlanner * planner;

      //basic data
	  int balls;
	  int lives;
      int dx, dy;
      bool left;
      MyAgent * collide_with;
    };

}//end namespace
