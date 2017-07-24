#include "ProgressBar.h"

using namespace std;

ProgressBar::ProgressBar() : mUsesImage(false), mCurrent(0), mEnd(0)
{
	mSkinOut.push_back(Image("ui/skin/button_top_left.png"));
	mSkinOut.push_back(Image("ui/skin/button_top_middle.png"));
	mSkinOut.push_back(Image("ui/skin/button_top_right.png"));
	mSkinOut.push_back(Image("ui/skin/button_middle_left.png"));
	mSkinOut.push_back(Image("ui/skin/button_middle_middle.png"));
	mSkinOut.push_back(Image("ui/skin/button_middle_right.png"));
	mSkinOut.push_back(Image("ui/skin/button_bottom_left.png"));
	mSkinOut.push_back(Image("ui/skin/button_bottom_middle.png"));
	mSkinOut.push_back(Image("ui/skin/button_bottom_right.png"));

	mSkinIn.push_back(Image("ui/skin/button_pressed_top_left.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_top_middle.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_top_right.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_middle_left.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_middle_middle.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_middle_right.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_bottom_left.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_bottom_middle.png"));
	mSkinIn.push_back(Image("ui/skin/button_pressed_bottom_right.png"));
}
ProgressBar::ProgressBar(Uint32 _End) : mUsesImage(false), mCurrent(0)
{
	ProgressBar();
	mEnd = _End;
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::image(const std::string& _path, ImageMode _m)
{
	mImage = Image(_path);
	mImageMode = _m;
	mUsesImage = true;
}


bool ProgressBar::hasImage() const
{
	return mImage.loaded();
}

void ProgressBar::update()
{
	draw();
}

/**
 * Draws the button.
 */
void ProgressBar::draw()
{
	if (!visible())
		return;
	Uint32	iWidth = 0;
	Renderer& r = Utility<Renderer>::get();

	r.drawImageRect(rect().x(), rect().y(), rect().width(), rect().height(), mSkinOut);
	r.drawImageRect(rect().x()+2, rect().y()+2, rect().width()-4, rect().height()-4, mSkinIn);

	iWidth = 0;
	if (mEnd > 0)
		iWidth = (rect().width() - 8) * mCurrent / mEnd;
	if (iWidth <= 0) return;
	if (hasImage() && mImageMode == ImageMode::Repeating)
	{
		r.drawImageRepeated(mImage, rect().x() + 4, rect().y() + 4, iWidth, rect().height() - 8);
	} else if (hasImage() && mImageMode == ImageMode::Stretching){
		r.drawImageStretched(mImage, rect().x() + 4, rect().y() + 4, iWidth, rect().height() - 8);
	} else if (hasImage() && mImageMode == ImageMode::Straight) {
		r.drawSubImage(mImage, rect().x() + 4, rect().y() + 4, 0, 0, iWidth, rect().height() - 8);
	} else {
		r.drawBoxFilled(rect().x() + 4, rect().y() + 4, iWidth, rect().height() - 8, mColorR, mColorG, mColorB, mColorAlpha);
	}
}

