#pragma once

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>
#include <algorithm>

#include "Core/UIContainer.h"
#include "Core/Slider.h"

#include "../Constants/UiConstants.h"
#include "../Things/Structures/Factory.h"


/**
 * Implements a ListBox control.
 */
class FactoryListBox : public UIContainer
{
public:
	typedef NAS2D::Signals::Signal0<void> SelectionChangedCallback;

public:
	class FactoryListBoxItem
	{
	public:
		FactoryListBoxItem(Factory* _f) : factory(_f) {}

		~FactoryListBoxItem() {}

	public:
		Factory*	factory = nullptr;

		Point_2d	size;
		Point_2d	icon_slice;
	};

public:
	FactoryListBox();
	virtual ~FactoryListBox();

	void addItem(Factory* factory);
	void removeItem(Factory* factory);
	void clearItems();

	int count() const { return mItems.size(); }
	bool empty() const { return mItems.empty(); }

	int currentHighlight() const { return mCurrentHighlight; }
	int currentSelection() const { return mCurrentSelection; }
	void currentSelection(int selection) { mCurrentSelection = selection; mSelectionChanged(); }

	const std::string& selectionText() const { return mItems[mCurrentSelection].factory->name(); }

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

	void update();

protected:
	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(double _position);

private:
	void _init();
	void _update_item_display();
	void _hook_events(bool hook);

	virtual void visibilityChanged(bool visible) final;

	virtual void onFocusChanged() final;
	virtual void onSizeChanged() final;

private:
	typedef std::vector<FactoryListBoxItem> FactoryItemList;

private:
	int							mCurrentHighlight = constants::NO_SELECTION;	/**< Currently highlighted selection index. */
	int							mCurrentSelection = constants::NO_SELECTION;	/**< Current selection index. */
	int							mCurrentOffset = 0;								/**<  */

	int							mItemWidth = 0;									/**< Width of items. */
	int							mLineCount = 0;									/**< Number of lines that can be displayed. */

	FactoryItemList				mItems;											/**< List of items preserved in the order in which they're added. */

	Color_4ub					mText = COLOR_WHITE;							/**< Text Color */
	Color_4ub					mHighlightBg = COLOR_GREEN;						/**< Highlight Background color. */
	Color_4ub					mHighlightText = COLOR_WHITE;					/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;								/**< Callback for selection changed callback. */
	Slider						mSlider;										/**<  */

	bool						mSorted = false;								/**< Flag indicating that all Items should be sorted. */
};