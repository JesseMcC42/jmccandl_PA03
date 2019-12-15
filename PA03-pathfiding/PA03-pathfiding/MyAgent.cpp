#include "MyAgent.h"
#include "MyGame.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include "mathtool/Box.h"
#include "MyPathPlanner.h"

namespace GMUCS425
{

  void MyAgent::handle_event(SDL_Event & e)
  {
    if(this->movable)
    {
      //do nothing now...
    }
  }

  //update this agent's motion, looks, sound, etc
  void MyAgent::update()
  {
    //do nothing by default...
  }

  void MyAgent::display()
  {
    if(!this->visible) return; //not visible...
    //setup positions and ask sprite to draw something
    this->sprite->display(x, y, scale, degree);
  }

  //show HUD (heads-up display) or status bar
  void MyAgent::draw_HUD()
  {
    //draw nothing by defaut, your task to display the location of an agent
    //on the upper left corner
    //read http://lazyfoo.net/tutorials/SDL/16_true_type_fonts/index.php
  }

  bool MyAgent::collide(MyAgent * other)
  {
    mathtool::Box2d box1, box2;
    box1.x=x;
    box1.y=y;
    box1.width=this->sprite->getWidth(scale);
    box1.height=this->sprite->getHeight(scale);

    box2.x=other->x;
    box2.y=other->y;
    box2.width=other->sprite->getWidth(other->scale);
    box2.height=other->sprite->getHeight(other->scale);

    return box1.intersect(box2);
  }

  void MyAgent::draw_bounding_box()
  {
    float my_W=this->sprite->getWidth(scale);
    float my_H=this->sprite->getHeight(scale);

    SDL_Rect box; //create a rect
    box.x = x;  //controls the rect's x coordinate
    box.y = y; // controls the rect's y coordinte
    box.w = my_W; // controls the width of the rect
    box.h = my_H; // controls the height of the rect

    SDL_Renderer * renderer=getMyGame()->getRenderer();
    SDL_SetRenderDrawColor(renderer,255,100,0,100);
    //SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderDrawRect(renderer,&box);

    box.w/=10;
    box.h=box.w;
    SDL_SetRenderDrawColor(renderer,0,0,0,100);
    SDL_RenderFillRect(renderer,&box);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////HUMAN
  void MyHumanAgent::display()
  {
    if(!this->visible) return; //not visible...
    //setup positions and ask sprite to draw something
    this->sprite->display(x, y, scale, degree, NULL, this->left?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
    //draw_bounding_box();
  }

  void MyHumanAgent::update()
  {

		  mathtool::Point2d goal;
		  for (MyAgent* ag : getMyGame()->getSceneManager()->get_active_scene()->get_agents()) {
			  if (ag->getC() == 'd') {
				  goal = mathtool::Point2d(ag->getX(), ag->getY());

				  if (goal[0] < 0) {
					  goal[0] = 0;
				  }
				  else if (goal[0] > getMyGame()->getScreenWidth()) {
					  goal[0] = getMyGame()->getScreenWidth();
				  }
				  if (goal[1] < 0) {
					  goal[1] = 0;
				  }
				  else if (goal[1] > getMyGame()->getScreenHeight()) {
					  goal[1] = getMyGame()->getScreenHeight();
				  }
			  }
		  }

		  std::list<mathtool::Point2d> path;

		  MyScene* scene = getMyGame()->getSceneManager()->get_active_scene();
		  MyPathPlanner* planner = new MyGridPathPlanner(scene, this, scene->get_width() * 2, scene->get_height() * 2);
		  assert(planner);
		  if (!planner->build()) {
			  std::cerr << "! Error: Failed to build a motion planner" << std::endl;
			  delete planner;
			  planner = NULL;
			  return;
		  }

		  bool r = planner->find_path(mathtool::Point2d(x, y), goal, path);
		  if (!r) //failed to fina path
		  {
			  this->has_goal = false;
		  }

		  //move to next waypoint
		  const int delta = 2;
		  if (path.size() == 0) {
			  return;
		  }
		  auto waypt = path.front(); //way point
		  mathtool::Point2d pos(x, y); //current position;
		  mathtool::Vector2d v = waypt - pos;
		  while (v.normsqr() < 2) {
			  path.pop_front();
			  if (path.empty())
			  {
				  return; //no more way points....
			  }
			  waypt = path.front();
			  v = waypt - pos;
		  }
		  float d = v.norm();
		  if (d > delta) v = v * (delta / d);

		  left = (v[0] < 0); //facing left now?

		  //check terrain, if the Cat is in watery area, slow down
		  const Uint32* terrain = getMyGame()->getSceneManager()->get_active_scene()->get_terrain();
		  int terrain_width = getMyGame()->getScreenWidth();
		  int terrain_height = getMyGame()->getScreenHeight();
		  Uint32 watery = terrain[((int)y) * terrain_width + ((int)x)] & 255;
		  float scale = (2 - watery * 1.0f / 255); //1~2
		  v = v * scale;

		  float dx = (int)v[0];
		  float dy = (int)v[1];

		  //update
		  x += dx;
		  y += dy;
	  
  }


  void MyHumanAgent::handle_event(SDL_Event & e)
  {
    //return;
    if(this->collision && collide_with!=NULL)
    {
      //std::cout<<"already in collision with "<<collide_with<<std::endl;

      return;
    }

    if(e.type==SDL_USEREVENT)
    {
      if(e.user.code == 1)
      {
        if(e.user.data1==this || e.user.data2==this)
        {
          MyAgent * other = (MyAgent *)((e.user.data1!=this)?e.user.data1:e.user.data2);

          if(other!=collide_with)
          {
            collide_with=other;
            left=!left;
          }
          this->collision_free_timer=10;
          this->collision=true;
        }
        //else collide_with=NULL; //no collision
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////Dog
  void MyDogAgent::display()
  {
    //MyAgent::display();
    if(!this->visible) return; //not visible...
    //setup positions and ask sprite to draw something
    this->sprite->display(x, y, scale, degree, NULL, this->ccw?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
    //draw_bounding_box();
  }


  void MyDogAgent::handle_event(SDL_Event & e)
  {
    //std::cout<<"--------------------------------------"<<std::endl;
    if(this->collision && collide_with!=NULL)
    {
      //std::cout<<"already in collision with "<<collide_with<<std::endl;

      return;
    }
    //else
    //  std::cout<<"NO in collision: this->collision="<<this->collision<<" with "<<collide_with<<std::endl;

    if(e.type==SDL_USEREVENT)
    {
      if(e.user.code == 1)
      {
        if(e.user.data1==this || e.user.data2==this)
        {
          MyAgent * other = (MyAgent *)((e.user.data1!=this)?e.user.data1:e.user.data2);
          if(other!=this->collide_with)
          {
            //std::cout<<"changed to: "<<other<<" from " << collide_with<<std::endl;
            collide_with=other;
            ccw=!ccw;
          }
          this->collision_free_timer=10;
          this->collision=true;
        }//end if
      }//end if
    }//end if
  }

  void MyDogAgent::update()
  {
	  if (this->behavior == NULL)
	  {
		  this->build_behavior_tree();
		  assert(this->behavior);
	  }

	  this->behavior->run();
	  if (this->getTimer() >= 0) {
		  this->setTimer(this->getTimer() - 1);
	  }


	  if (!this->collision)
	  {
		  if (collision_free_timer >= 0) this->collision_free_timer--;
		  else collide_with = NULL; //no collision
	  }

	  this->collision = false;
  }

  
  class MyDogTaskNode : public MyTaskNode
  {
  public:
	  MyDogTaskNode(MyDogAgent* agent) { dog = agent; }
  protected:
	  MyDogAgent* dog;
  };


  //TODO: Define more tasks here
  class SeeHuman : public MyDogTaskNode
  {
  public:
	  SeeHuman(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {

		  list<MyAgent* > agentList = GMUCS425::getMyGame()->getSceneManager()->get_active_scene()->get_agents();
		  for (list<MyAgent*>::iterator it = agentList.begin(); it != agentList.end(); ++it)
		  {
			  MyAgent* object = *it;
			  if (object->getC() == 'z') {
				  float dist = sqrt(sqr((object->getX()) - (this->agent->getX())) + sqr((object->getY()) - (this->agent->getY())));
				  if (dist <= 80) {

					  //cerr << "TASK See Human" << endl;
					  return true;
				  }
			  }
		  }
		  return false;
	  }
  private:
	  MyDogAgent* agent; //remove this
  };



  //TODO: Define more tasks here
  class Bark : public MyDogTaskNode
  {
  public:
	  Bark(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {
		  //TODO: implement this
		  //cerr << "TASK Caw" << endl;
		  if (agent->getTimer() <= 0) {
			  

			  return true;
		  }

		  return true;
	  }
  private:
	  MyDogAgent* agent; //remove this
  };


  //TODO: Define more tasks here
  class RunAway : public MyDogTaskNode
  {
  public:
	  RunAway(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {
		  list<MyAgent* > agentList = GMUCS425::getMyGame()->getSceneManager()->get_active_scene()->get_agents();
		  for (list<MyAgent*>::iterator it = agentList.begin(); it != agentList.end(); ++it)
		  {
			  MyAgent* object = *it;
			  if (object->getC() == 'z') {
				  float dist = sqrt(sqr((object->getX()) - (this->agent->getX())) + sqr((object->getY()) - (this->agent->getY())));
				  if (dist <= 100) {
					  //cerr << "TASK Run Away" << endl;

					  //get the normalized vector away from the chicken
					  mathtool::Vector2d vec = mathtool::Vector2d(double(object->getX() - this->agent->getX()), double(object->getY() - this->agent->getY())).normalize();

					  //we want to move a total of two steps each move. Normalized is one, so multiply by two
					  this->agent->tranlateTo(this->agent->getX() - (vec[0] * 2), this->agent->getY() - (vec[1] * 2));

					  return true;
				  }
			  }
		  }
		  return false;
	  }
  private:
	  MyDogAgent* agent; //remove this
  };


  //TODO: Define more tasks here
  class SeeCat : public MyDogTaskNode
  {
  public:
	  SeeCat(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {
		  list<MyAgent* > agentList = GMUCS425::getMyGame()->getSceneManager()->get_active_scene()->get_agents();
		  for (list<MyAgent*>::iterator it = agentList.begin(); it != agentList.end(); ++it)
		  {
			  MyAgent* object = *it;
			  if (object->getC() == 'd') {
				  float dist = sqrt(sqr((object->getX()) - (this->agent->getX())) + sqr((object->getY()) - (this->agent->getY())));
				  if (dist <= 200) {

					  //cerr << "TASK See Cat" << endl;
					  return true;
				  }
			  }
		  }
		  return false;
	  }
  private:
	  MyDogAgent* agent; //remove this
  };


  //TODO: Define more tasks here
  class Chase : public MyDogTaskNode
  {
  public:
	  Chase(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {
		  list<MyAgent* > agentList = GMUCS425::getMyGame()->getSceneManager()->get_active_scene()->get_agents();
		  for (list<MyAgent*>::iterator it = agentList.begin(); it != agentList.end(); ++it)
		  {
			  MyAgent* object = *it;
			  if (object->getC() == 'd') {
				  float dist = sqrt(sqr((object->getX()) - (this->agent->getX())) + sqr((object->getY()) - (this->agent->getY())));
				  if (dist <= 200) {
					  //cerr << "TASK Run Away" << endl;

					  if (dist > 20) {
						  //get the normalized vector toward the dragon
						  mathtool::Vector2d vec = mathtool::Vector2d(double(object->getX() - this->agent->getX()), double(object->getY() - this->agent->getY())).normalize();

						  //we want to move a total of two steps each move. Normalized is one, so multiply by two
						  this->agent->tranlateTo(this->agent->getX() + (vec[0] * 2), this->agent->getY() + (vec[1] * 2));

						  return true;
					  }
				  }
			  }
		  }
		  return false;
	  }
  private:
	  MyDogAgent* agent; //remove this
  };


  //TODO: Define more tasks here
  class IdleWalking : public MyDogTaskNode
  {
  public:
	  IdleWalking(MyDogAgent* agent) :MyDogTaskNode(agent)
	  {
		  this->agent = agent;
	  }
	  bool run() {
		  //cerr << "Idle" << endl;

		  MyAgent* other = this->agent->getCollideWith();

		  if (other != NULL) {

			  mathtool::Box2d box1, box2;
			  box1.x = this->agent->getX();
			  box1.y = this->agent->getY();
			  box1.width = this->agent->getSprite()->getWidth(this->agent->getScale());
			  box1.height = this->agent->getSprite()->getHeight(this->agent->getScale());

			  box2.x = other->getX();
			  box2.y = other->getY();
			  box2.width = other->getSprite()->getWidth(other->getScale());
			  box2.height = other->getSprite()->getHeight(other->getScale());

			  if (box1.intersect(box2)) {

				  if (this->agent->getCollision() <= 0) {
					  this->agent->setLeft(!(this->agent->getLeft()));
					  this->agent->setCollision(15);
				  }
			  }



		  }

		  this->agent->setCollision(this->agent->getCollision() - 1);

		  if (this->agent->getLeft()) {
			  this->agent->setX(this->agent->getX() - 2);
		  }
		  else {
			  this->agent->setX(this->agent->getX() + 2);
		  }
		  if (this->agent->getX() >= this->agent->getOrigin() + 40) {
			  this->agent->setLeft(true);
		  }
		  if (this->agent->getX() < this->agent->getOrigin() - 40) {
			  this->agent->setLeft(false);
		  }
		  return true;

	  }
  private:
	  MyDogAgent* agent; //remove this
  };

  void MyDogAgent::build_behavior_tree()
  {
	  MyTaskNode* root = NULL;

	  //TODO:
	  //Create a root
	  //Create more nodes using MyDogTask1~MyDogTask100
	  //use add_kid to build the rest of the tree!

	  //EXAMPLE: REMOVE THIS BLOCK

	  MySelectorNode* dummy_node = new MySelectorNode();

	  MySequenceNode* human_seq = new MySequenceNode();
	  SeeHuman* seeHum = new SeeHuman(this);
	  RunAway* run = new RunAway(this);


	  human_seq->add_kid(seeHum);
	  human_seq->add_kid(run);

	  MySequenceNode* chase_seq = new MySequenceNode();
	  SeeCat* seeCat = new SeeCat(this);
	  Bark* bark = new Bark(this);
	  Chase* chase = new Chase(this);

	  chase_seq->add_kid(seeCat);
	  chase_seq->add_kid(bark);
	  chase_seq->add_kid(chase);

	  IdleWalking* idle = new IdleWalking(this);

	  dummy_node->add_kid(human_seq);
	  dummy_node->add_kid(chase_seq);
	  dummy_node->add_kid(idle);


	  root = dummy_node;
	  //EXAMPLE: REMOVE THIS BLOCK

	  //build the tree using the root
	  this->behavior = new MyBehaviorTree(root);
	  this->setOrigin(getX());

  }


}//end namespace
