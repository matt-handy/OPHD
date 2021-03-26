#pragma once

#include "PowerStructure.h"

#include "../../Constants.h"

const int SOLAR_PANEL_BASE_PRODUCUCTION = 50;

class SolarPanelArray : public PowerStructure
{
public:
	SolarPanelArray(float meanSolarDistance) : PowerStructure(constants::SOLAR_PANEL1,
		"structures/solar_array1.sprite",
		StructureClass::EnergyProduction,
		StructureTypeID::SID_SOLAR_PANEL1),
		mMeanSolarDistance(meanSolarDistance)
	{
		maxAge(1000);
		turnsToBuild(4);
		requiresCHAP(false);
	}

protected:
	int calculateMaxEnergyProduction() override
	{
		// Prevent possible dividing by zero
		float solarDistance = mMeanSolarDistance != 0 ? mMeanSolarDistance : 1;

		return static_cast<int>(SOLAR_PANEL_BASE_PRODUCUCTION / solarDistance);
	}

private:
	const float mMeanSolarDistance;
};
