#pragma once

#include "Robot.h"
#include "../../Constants/Strings.h"


class Robominer: public Robot
{
public:
	Robominer(): Robot(constants::ROBOMINER, "robots/robominer.sprite", RobotType::Miner)
	{
	}

	void update() override { updateTask(); }
};
