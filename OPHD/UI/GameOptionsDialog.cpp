#include "GameOptionsDialog.h"

#include "../Constants/Strings.h"
#include "../ShellOpenPath.h"
#include <array>


GameOptionsDialog::GameOptionsDialog() :
	Window{constants::WindowSystemTitle},
	btnSave{"Save current game", {this, &GameOptionsDialog::onSave}},
	btnLoad{"Load a saved game", {this, &GameOptionsDialog::onLoad}},
	btnHelp{"Help", {this, &GameOptionsDialog::onHelp}},
	btnReturn{"Return to current game", {this, &GameOptionsDialog::onReturn}},
	btnClose{"Return to Main Menu", {this, &GameOptionsDialog::onClose}}
{
	position({0, 0});
	size({210, 188});

	const auto buttons = std::array{&btnSave, &btnLoad, &btnHelp, &btnReturn, &btnClose};
	for (auto button : buttons)
	{
		button->size({200, 25});
	}

	add(btnSave, {5, 25});
	add(btnLoad, {5, 53});
	add(btnHelp, {5, 81});
	add(btnReturn, {5, 119});
	add(btnClose, {5, 154});

	anchored(true);
}


GameOptionsDialog::~GameOptionsDialog()
{
	btnSave.click().disconnect(this, &GameOptionsDialog::onSave);
	btnLoad.click().disconnect(this, &GameOptionsDialog::onLoad);
	btnReturn.click().disconnect(this, &GameOptionsDialog::onReturn);
	btnClose.click().disconnect(this, &GameOptionsDialog::onClose);
}


void GameOptionsDialog::onEnableChange()
{
	btnSave.enabled(enabled());
	btnLoad.enabled(enabled());
	btnReturn.enabled(enabled());
	btnClose.enabled(enabled());
}


void GameOptionsDialog::update()
{
	if (!visible()) { return; }

	Window::update();
}

void GameOptionsDialog::onSave()
{
	mSignalSave();
}

void GameOptionsDialog::onLoad()
{
	mSignalLoad();
}

void GameOptionsDialog::onHelp()
{
	shellOpenPath("https://wiki.outpost2.net/doku.php?id=outposthd:how_to_play");
}

void GameOptionsDialog::onReturn()
{
	mSignalReturn();
}

void GameOptionsDialog::onClose()
{
	mSignalClose();
}
