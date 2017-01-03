#include "TextArea.h"


TextArea::TextArea() : mNumLines(0)
{}


TextArea::~TextArea()
{}


StringList TextArea::split(const char *str, char delim)
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while (*str != delim && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}


void TextArea::processString()
{
	mFormattedList.clear();

	if (width() < 1 || !fontSet() || text().empty())
		return;

	StringList tokenList = split(text().c_str());
	
	size_t w = 0, i = 0;
	while (i < tokenList.size())
	{
		string line;
		while (w < width() && i < tokenList.size())
		{
			int tokenWidth = font().width(tokenList[i] + " ");
			w += tokenWidth;
			if (w >= width())
			{
				/**
				 * \todo	In some edge cases where the width of the TextArea is too
				 *			narrow for a single word/token, this will result in an infinite
				 *			loop. This edge case will need to be resolved either by splitting
				 *			the token that's too wide or by simply rendering it as is.
				 */
				//++i;
				break;
			}

			line += (tokenList[i] + " ");
			if (tokenList[i] == "\n")
			{
				++i;
				break;
			}

			++i;

		}
		w = 0;
		mFormattedList.push_back(line);
	}

	mNumLines = static_cast<size_t>(height() / font().height());
}


void TextArea::onSizeChanged()
{
	Control::onSizeChanged();
	processString();
}


void TextArea::onTextChanged()
{
	processString();
}


void TextArea::onFontChanged()
{
	processString();
}


void TextArea::update()
{
	draw();
}


void TextArea::draw()
{
	Renderer& r = Utility<Renderer>::get();

	if(highlight())
		r.drawBox(rect(), 255, 255, 255);

	for (size_t i = 0; i < mFormattedList.size() && i < mNumLines; ++i)
		r.drawText(font(), mFormattedList[i], positionX(), positionY() + (font().height() * i), 255, 255, 255);
}
