#include "MyBehaviorTree.h"


bool MySelectorNode::run()
{
	std::list<MyTaskNode*> kids = this->children;

	for (std::list<MyTaskNode*>::iterator it = kids.begin(); it != kids.end(); ++it)
	{
		MyTaskNode* node = *it;

		//run the node
		bool complete = node->run();

		//since we are a selector, if the node fails, we will just
		//try the next one. We only stop when one suceeds
		if (complete) {
			return true;
		}
	}

	//if we exit, that means all nodes failed
	return false;
}

bool MySequenceNode::run()
{
	std::list<MyTaskNode*> kids = this->children;

	for (std::list<MyTaskNode*>::iterator it = kids.begin(); it != kids.end(); ++it)
	{
		MyTaskNode* node = *it;

		//run the node
		bool complete = node->run();

		//since we are a sequence, if one fails, the while sequence fails
		if (!complete) {
			return false;
		}
	}

	//if we exit, that means all nodes ran to completion
	return true;
}
