#pragma once

#include "../Things/Structures/Structure.h"

#include "Core/UIContainer.h"
#include "Core/ToolTip.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Resource/Image.h>


struct StorableResources;
class Population;


class ResourceInfoBar : public UIContainer
{
public:
	ResourceInfoBar(const StorableResources& resources, const Population& population, const int& currentMorale, const int& previousMorale, const int& food);

	bool isResourcePanelVisible() const;
	bool isPopulationPanelVisible() const;

	void update() override;
	void draw() const override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y) override;

	int totalStorage(Structure::StructureClass, int) const;

private:
	const StorableResources& mResourcesCount;
	const Population& mPopulation;
	const int& mCurrentMorale;
	const int& mPreviousMorale;
	const int& mFood;

	const NAS2D::Image& mUiIcons;

	// Bare Control's use for ToolTips
	Control mTooltipPopulation;
	Control mTooltipEnergy;
	Control mTooltipFoodStorage;
	Control mTooltipResourceStorage;
	Control mTooltipResourceBreakdown;

	ToolTip mToolTip;

	bool mPinResourcePanel = false;
	bool mPinPopulationPanel = false;
};
