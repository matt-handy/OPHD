// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryReport.h"

#include "../TextRender.h"
#include "../../Cache.h"
#include "../../Constants.h"
#include "../../StructureManager.h"

#include "../../Things/Structures/SurfaceFactory.h"
#include "../../Things/Structures/SeedFactory.h"
#include "../../Things/Structures/UndergroundFactory.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


using namespace NAS2D;


static Rectangle<int> DETAIL_PANEL;
static Factory* SELECTED_FACTORY = nullptr;

std::array<const Image*, ProductType::PRODUCT_COUNT> PRODUCT_IMAGE_ARRAY;
static ProductType SELECTED_PRODUCT_TYPE = ProductType::PRODUCT_NONE;


FactoryReport::FactoryReport() :
	font{&fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL)},
	fontMedium{&fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_MEDIUM)},
	fontMediumBold{&fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_MEDIUM)},
	fontBigBold{&fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_HUGE)},
	factorySeed{&imageCache.load("ui/interface/factory_seed.png")},
	factoryAboveGround{&imageCache.load("ui/interface/factory_ag.png")},
	factoryUnderGround{&imageCache.load("ui/interface/factory_ug.png")},
	btnShowAll{"All"},
	btnShowSurface{"Surface"},
	btnShowUnderground{"Underground"},
	btnShowActive{"Active"},
	btnShowIdle{"Idle"},
	btnShowDisabled{"Disabled"},
	btnIdle{"Idle"},
	btnClearProduction{"Clear Production"},
	btnTakeMeThere{constants::BUTTON_TAKE_ME_THERE},
	btnApply{"Apply"}
{
	/// \todo Decide if this is the best place to have these images live or if it should be done at program start.
	PRODUCT_IMAGE_ARRAY.fill(nullptr);
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_DIGGER] = &imageCache.load("ui/interface/product_robodigger.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_DOZER] = &imageCache.load("ui/interface/product_robodozer.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_MINER] = &imageCache.load("ui/interface/product_robominer.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_EXPLORER] = &imageCache.load("ui/interface/product_roboexplorer.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_TRUCK] = &imageCache.load("ui/interface/product_truck.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_ROAD_MATERIALS] = &imageCache.load("ui/interface/product_road_materials.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_MAINTENANCE_PARTS] = &imageCache.load("ui/interface/product_maintenance_parts.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_CLOTHING] = &imageCache.load("ui/interface/product_clothing.png");
	PRODUCT_IMAGE_ARRAY[ProductType::PRODUCT_MEDICINE] = &imageCache.load("ui/interface/product_medicine.png");

	add(&lstFactoryList, 10, 63);
	lstFactoryList.selectionChanged().connect(this, &FactoryReport::lstFactoryListSelectionChanged);

	add(&btnShowAll, 10, 10);
	btnShowAll.size({75, 20});
	btnShowAll.type(Button::Type::BUTTON_TOGGLE);
	btnShowAll.toggle(true);
	btnShowAll.click().connect(this, &FactoryReport::btnShowAllClicked);

	add(&btnShowSurface, 87, 10);
	btnShowSurface.size({75, 20});
	btnShowSurface.type(Button::Type::BUTTON_TOGGLE);
	btnShowSurface.click().connect(this, &FactoryReport::btnShowSurfaceClicked);

	add(&btnShowUnderground, 164, 10);
	btnShowUnderground.size({75, 20});
	btnShowUnderground.type(Button::Type::BUTTON_TOGGLE);
	btnShowUnderground.click().connect(this, &FactoryReport::btnShowUndergroundClicked);

	add(&btnShowActive, 10, 33);
	btnShowActive.size({75, 20});
	btnShowActive.type(Button::Type::BUTTON_TOGGLE);
	btnShowActive.click().connect(this, &FactoryReport::btnShowActiveClicked);

	add(&btnShowIdle, 87, 33);
	btnShowIdle.size({75, 20});
	btnShowIdle.type(Button::Type::BUTTON_TOGGLE);
	btnShowIdle.click().connect(this, &FactoryReport::btnShowIdleClicked);

	add(&btnShowDisabled, 164, 33);
	btnShowDisabled.size({75, 20});
	btnShowDisabled.type(Button::Type::BUTTON_TOGGLE);
	btnShowDisabled.click().connect(this, &FactoryReport::btnShowDisabledClicked);

	int position_x = Utility<Renderer>::get().size().x - 110;
	add(&btnIdle, position_x, 35);
	btnIdle.type(Button::Type::BUTTON_TOGGLE);
	btnIdle.size({140, 30});
	btnIdle.click().connect(this, &FactoryReport::btnIdleClicked);

	add(&btnClearProduction, position_x, 75);
	btnClearProduction.size({140, 30});
	btnClearProduction.click().connect(this, &FactoryReport::btnClearProductionClicked);

	add(&btnTakeMeThere, position_x, 115);
	btnTakeMeThere.size({140, 30});
	btnTakeMeThere.click().connect(this, &FactoryReport::btnTakeMeThereClicked);

	add(&btnApply, 0, 0);
	btnApply.size({140, 30});
	btnApply.click().connect(this, &FactoryReport::btnApplyClicked);

	add(&cboFilterByProduct, 250, 33);
	cboFilterByProduct.size({200, 20});

	cboFilterByProduct.addItem(constants::NONE, ProductType::PRODUCT_NONE);
	cboFilterByProduct.addItem(constants::CLOTHING, ProductType::PRODUCT_CLOTHING);
	cboFilterByProduct.addItem(constants::MAINTENANCE_SUPPLIES, ProductType::PRODUCT_MAINTENANCE_PARTS);
	cboFilterByProduct.addItem(constants::MEDICINE, ProductType::PRODUCT_MEDICINE);
	cboFilterByProduct.addItem(constants::ROBODIGGER, ProductType::PRODUCT_DIGGER);
	cboFilterByProduct.addItem(constants::ROBODOZER, ProductType::PRODUCT_DOZER);
	cboFilterByProduct.addItem(constants::ROBOEXPLORER, ProductType::PRODUCT_EXPLORER);
	cboFilterByProduct.addItem(constants::ROBOMINER, ProductType::PRODUCT_MINER);
	cboFilterByProduct.addItem(constants::ROAD_MATERIALS, ProductType::PRODUCT_ROAD_MATERIALS);
	cboFilterByProduct.addItem(constants::TRUCK, ProductType::PRODUCT_TRUCK);

	cboFilterByProduct.selectionChanged().connect(this, &FactoryReport::cboFilterByProductSelectionChanged);

	add(&lstProducts, cboFilterByProduct.rect().x + cboFilterByProduct.rect().width + 20, mRect.y + 230);

	txtProductDescription.font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	txtProductDescription.height(128);
	txtProductDescription.textColor(NAS2D::Color{0, 185, 0});
	txtProductDescription.text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

	Control::resized().connect(this, &FactoryReport::resized);
	fillLists();
}


FactoryReport::~FactoryReport()
{
	SELECTED_FACTORY = nullptr;
}


/**
 * Override of the interface provided by ReportInterface class.
 * 
 * \note	Pointer 
 */
void FactoryReport::selectStructure(Structure* structure)
{
	lstFactoryList.currentSelection(dynamic_cast<Factory*>(structure));
}


void FactoryReport::clearSelection()
{
	lstFactoryList.clearSelection();
	SELECTED_FACTORY = nullptr;
}


/**
 * Pass-through function to simulate clicking on the Show All button.
 */
void FactoryReport::refresh()
{
	btnShowAllClicked();
}


/**
 * Fills the factory list with all available factories.
 */
void FactoryReport::fillLists()
{
	SELECTED_FACTORY = nullptr;
	lstFactoryList.clearItems();
	for (auto factory : Utility<StructureManager>::get().structureList(Structure::StructureClass::Factory))
	{
		lstFactoryList.addItem(static_cast<Factory*>(factory));
	}
	checkFactoryActionControls();
}


/**
 * Fills the factory list based on product type.
 */
void FactoryReport::fillFactoryList(ProductType type)
{
	SELECTED_FACTORY = nullptr;
	lstFactoryList.clearItems();
	for (auto f : Utility<StructureManager>::get().structureList(Structure::StructureClass::Factory))
	{
		Factory* factory = static_cast<Factory*>(f);
		if (factory->productType() == type)
		{
			lstFactoryList.addItem(factory);
		}
	}

	checkFactoryActionControls();
}


/**
 * Fills the factory list based on surface/subterranean
 */
void FactoryReport::fillFactoryList(bool surface)
{
	SELECTED_FACTORY = nullptr;
	lstFactoryList.clearItems();
	for (auto f : Utility<StructureManager>::get().structureList(Structure::StructureClass::Factory))
	{
		Factory* factory = static_cast<Factory*>(f);
		if (surface && (factory->name() == constants::SURFACE_FACTORY || factory->name() == constants::SEED_FACTORY))
		{
			lstFactoryList.addItem(factory);
		}
		else if (!surface && factory->name() == constants::UNDERGROUND_FACTORY)
		{
			lstFactoryList.addItem(factory);
		}
	}

	checkFactoryActionControls();
}


/**
 * Fills the factory list based on structure state.
 */
void FactoryReport::fillFactoryList(StructureState state)
{
	SELECTED_FACTORY = nullptr;
	lstFactoryList.clearItems();
	for (auto f : Utility<StructureManager>::get().structureList(Structure::StructureClass::Factory))
	{
		if (f->state() == state)
		{
			lstFactoryList.addItem(static_cast<Factory*>(f));
		}
	}

	lstFactoryList.setSelection(0);
	checkFactoryActionControls();
}


/**
 * Sets visibility for factory action controls.
 */
void FactoryReport::checkFactoryActionControls()
{
	bool actionControlVisible = !lstFactoryList.empty();

	btnIdle.visible(actionControlVisible);
	btnClearProduction.visible(actionControlVisible);
	btnTakeMeThere.visible(actionControlVisible);
	btnApply.visible(actionControlVisible);
	lstProducts.visible(actionControlVisible);

	if (actionControlVisible) { lstFactoryList.setSelection(0); }
}


void FactoryReport::resized(Control* /*c*/)
{
	const auto comboEndPoint = cboFilterByProduct.rect().endPoint();

	lstFactoryList.size({comboEndPoint.x - 10, mRect.height - 74});

	DETAIL_PANEL = {
		comboEndPoint.x + 20,
		rect().y + 10,
		rect().width - comboEndPoint.x - 30,
		rect().y + mRect.height - 69
	};

	int position_x = mRect.width - 150;
	btnIdle.position({position_x, btnIdle.positionY()});
	btnClearProduction.position({position_x, btnClearProduction.positionY()});
	btnTakeMeThere.position({position_x, btnTakeMeThere.positionY()});

	btnApply.position({position_x, mRect.height + 8});

	lstProducts.size({DETAIL_PANEL.width / 3, DETAIL_PANEL.height - 219});
	lstProducts.selectionChanged().connect(this, &FactoryReport::lstProductsSelectionChanged);

	txtProductDescription.position(lstProducts.rect().crossXPoint() + NAS2D::Vector{158, 0});
	txtProductDescription.width(mRect.width - txtProductDescription.positionX() - 30);
}


/**
 * This has been overridden to handle some UI elements that need
 * special handling.
 */
void FactoryReport::visibilityChanged(bool visible)
{
	if (!SELECTED_FACTORY) { return; }

	StructureState _state = SELECTED_FACTORY->state();
	btnApply.visible(visible && (_state == StructureState::Operational || _state == StructureState::Idle));
	checkFactoryActionControls();
}


void FactoryReport::filterButtonClicked(bool clearCbo)
{
	btnShowAll.toggle(false);
	btnShowSurface.toggle(false);
	btnShowUnderground.toggle(false);
	btnShowActive.toggle(false);
	btnShowIdle.toggle(false);
	btnShowDisabled.toggle(false);

	if (clearCbo) { cboFilterByProduct.clearSelection(); }
}


void FactoryReport::btnShowAllClicked()
{
	filterButtonClicked(true);
	btnShowAll.toggle(true);

	fillLists();
}


void FactoryReport::btnShowSurfaceClicked()
{
	filterButtonClicked(true);
	btnShowSurface.toggle(true);
	fillFactoryList(true);
}


void FactoryReport::btnShowUndergroundClicked()
{
	filterButtonClicked(true);
	btnShowUnderground.toggle(true);
	fillFactoryList(false);
}


void FactoryReport::btnShowActiveClicked()
{
	filterButtonClicked(true);
	btnShowActive.toggle(true);
	fillFactoryList(StructureState::Operational);
}


void FactoryReport::btnShowIdleClicked()
{
	filterButtonClicked(true);
	btnShowIdle.toggle(true);
	fillFactoryList(StructureState::Idle);
}


void FactoryReport::btnShowDisabledClicked()
{
	filterButtonClicked(true);
	btnShowDisabled.toggle(true);
	fillFactoryList(StructureState::Disabled);
}


void FactoryReport::btnIdleClicked()
{
	SELECTED_FACTORY->forceIdle(btnIdle.toggled());
}


void FactoryReport::btnClearProductionClicked()
{
	SELECTED_FACTORY->productType(ProductType::PRODUCT_NONE);
	lstProducts.clearSelection();
	cboFilterByProductSelectionChanged();
}


void FactoryReport::btnTakeMeThereClicked()
{
	takeMeThereCallback()(SELECTED_FACTORY);
}


void FactoryReport::btnApplyClicked()
{
	SELECTED_FACTORY->productType(SELECTED_PRODUCT_TYPE);
	cboFilterByProductSelectionChanged();
}


void FactoryReport::lstFactoryListSelectionChanged()
{
	SELECTED_FACTORY = lstFactoryList.selectedFactory();

	if (!SELECTED_FACTORY)
	{
		checkFactoryActionControls();
		return;
	}

	/// \fixme Ugly
	if (SELECTED_FACTORY->name() == constants::SEED_FACTORY) { factoryImage = factorySeed; }
	else if (SELECTED_FACTORY->name() == constants::SURFACE_FACTORY) { factoryImage = factoryAboveGround; }
	else if (SELECTED_FACTORY->name() == constants::UNDERGROUND_FACTORY) { factoryImage = factoryUnderGround; }

	btnIdle.toggle(SELECTED_FACTORY->state() == StructureState::Idle);
	btnIdle.enabled(SELECTED_FACTORY->state() == StructureState::Operational || SELECTED_FACTORY->state() == StructureState::Idle);

	btnClearProduction.enabled(SELECTED_FACTORY->state() == StructureState::Operational || SELECTED_FACTORY->state() == StructureState::Idle);

	lstProducts.dropAllItems();
	if (SELECTED_FACTORY->state() != StructureState::Destroyed)
	{
		const Factory::ProductionTypeList& _pl = SELECTED_FACTORY->productList();
		for (auto item : _pl)
		{
			lstProducts.addItem(productDescription(item), static_cast<int>(item));
		}
	}
	lstProducts.clearSelection();
	lstProducts.setSelectionByName(productDescription(SELECTED_FACTORY->productType()));
	SELECTED_PRODUCT_TYPE = SELECTED_FACTORY->productType();

	StructureState _state = SELECTED_FACTORY->state();
	btnApply.visible(_state == StructureState::Operational || _state == StructureState::Idle);
}


void FactoryReport::lstProductsSelectionChanged()
{
	SELECTED_PRODUCT_TYPE = static_cast<ProductType>(lstProducts.selectionTag());
}


void FactoryReport::cboFilterByProductSelectionChanged()
{
	if (cboFilterByProduct.currentSelection() == constants::NO_SELECTION) { return; }
	filterButtonClicked(false);
	fillFactoryList(static_cast<ProductType>(cboFilterByProduct.selectionTag()));
}


void FactoryReport::drawDetailPane(Renderer& renderer)
{
	NAS2D::Color defaultTextColor{0, 185, 0};

	const auto startPoint = DETAIL_PANEL.startPoint();
	renderer.drawImage(*factoryImage, startPoint + NAS2D::Vector{0, 25});
	renderer.drawText(*fontBigBold, SELECTED_FACTORY->name(), startPoint + NAS2D::Vector{0, -8}, defaultTextColor);

	auto statusPosition = startPoint + NAS2D::Vector{138, 20};
	renderer.drawText(*fontMediumBold, "Status", statusPosition, defaultTextColor);

	bool isStatusHighlighted = SELECTED_FACTORY->disabled() || SELECTED_FACTORY->destroyed();
	statusPosition.x += fontMediumBold->width("Status") + 20;
	renderer.drawText(*fontMedium, SELECTED_FACTORY->stateDescription(), statusPosition, (isStatusHighlighted ? NAS2D::Color::Red : defaultTextColor));

	renderer.drawText(*fontMediumBold, "Resources Required", startPoint + NAS2D::Vector{138, 60}, defaultTextColor);

	const auto labelWidth = fontMediumBold->width("Resources Required");

	// MINERAL RESOURCES
	const ProductionCost& _pc = productCost(SELECTED_FACTORY->productType());
	const std::array requiredResources{
		std::pair{"Common Metals", _pc.commonMetals()},
		std::pair{"Common Minerals", _pc.commonMinerals()},
		std::pair{"Rare Metals", _pc.rareMetals()},
		std::pair{"Rare Minerals", _pc.rareMinerals()},
	};
	auto position = startPoint + NAS2D::Vector{138, 80};
	for (auto [title, value] : requiredResources) {
		drawLabelAndValueLeftJustify(position, labelWidth, title, std::to_string(value), defaultTextColor);
		position.y += 15;
	}

	// POPULATION
	bool isPopulationRequirementHighlighted = SELECTED_FACTORY->populationAvailable()[0] != SELECTED_FACTORY->populationRequirements()[0];
	auto text = std::to_string(SELECTED_FACTORY->populationAvailable()[0]) + " / " + std::to_string(SELECTED_FACTORY->populationRequirements()[0]);
	drawLabelAndValueLeftJustify(position, labelWidth, "Workers", text, (isPopulationRequirementHighlighted ? NAS2D::Color::Red : defaultTextColor));
}


void FactoryReport::drawProductPane(Renderer& renderer)
{
	const auto textColor = NAS2D::Color{0, 185, 0};
	renderer.drawText(*fontBigBold, "Production", NAS2D::Point{DETAIL_PANEL.x, DETAIL_PANEL.y + 180}, textColor);

	int position_x = DETAIL_PANEL.x + lstProducts.size().x + 20;

	if (SELECTED_PRODUCT_TYPE != ProductType::PRODUCT_NONE)
	{
		renderer.drawText(*fontBigBold, productDescription(SELECTED_PRODUCT_TYPE), NAS2D::Point{position_x, DETAIL_PANEL.y + 180}, textColor);
		renderer.drawImage(*PRODUCT_IMAGE_ARRAY[static_cast<std::size_t>(SELECTED_PRODUCT_TYPE)], NAS2D::Point{position_x, lstProducts.positionY()});
		txtProductDescription.update();
	}

	if (SELECTED_FACTORY->productType() == ProductType::PRODUCT_NONE) { return; }
	
	renderer.drawText(*fontBigBold, "Progress", NAS2D::Point{position_x, DETAIL_PANEL.y + 358}, textColor);
	renderer.drawText(*fontMedium, "Building " + productDescription(SELECTED_FACTORY->productType()), NAS2D::Point{position_x, DETAIL_PANEL.y + 393}, textColor);

	float percent = 0.0f;
	if (SELECTED_FACTORY->productType() != ProductType::PRODUCT_NONE)
	{
		percent = static_cast<float>(SELECTED_FACTORY->productionTurnsCompleted()) /
			static_cast<float>(SELECTED_FACTORY->productionTurnsToComplete());
	}
	
	drawBasicProgressBar(position_x, DETAIL_PANEL.y + 413, mRect.width - position_x - 10, 30, percent, 4);

	const auto text = std::to_string(SELECTED_FACTORY->productionTurnsCompleted()) + " / " + std::to_string(SELECTED_FACTORY->productionTurnsToComplete());
	renderer.drawText(*fontMediumBold, "Turns", NAS2D::Point{position_x, DETAIL_PANEL.y + 449}, textColor);
	renderer.drawText(*fontMedium, text, NAS2D::Point{mRect.width - fontMedium->width(text) - 10, DETAIL_PANEL.y + 449}, textColor);
}


void FactoryReport::update()
{
	if (!visible()) { return; }
	auto& renderer = Utility<Renderer>::get();

	const auto textColor = NAS2D::Color{0, 185, 0};
	const auto positionX = cboFilterByProduct.rect().x + cboFilterByProduct.rect().width;
	renderer.drawLine(NAS2D::Point{positionX + 10, mRect.y + 10}, NAS2D::Point{positionX + 10, mRect.y + mRect.height - 10}, textColor);
	renderer.drawText(*font, "Filter by Product", NAS2D::Point{positionX - font->width("Filter by Product"), mRect.y + 10}, textColor);

	if (SELECTED_FACTORY)
	{
		drawDetailPane(renderer);
		drawProductPane(renderer);
	}

	UIContainer::update();
}
