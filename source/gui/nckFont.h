
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_FONT_H
#define NCK_FONT_H

#include "nckGuiConfig.h"
#include "nckGraphics.h"
#include <map>

_GUI_BEGIN

/// Font alignment modes.
enum FontAlignment{
	FONT_ALIGNMENT_LEFT,
	FONT_ALIGNMENT_MIDDLE,
	FONT_ALIGNMENT_RIGHT,
};

enum FontPositionAccuracy{
	FONT_POSITION_ACCURACY_FLOAT,
	FONT_POSITION_ACCURACY_INTEGER,
};

/**
* Characters map and rendering class.
*/
class FontMap
{
public:
	/// Character cell position attributes.
	class Character
	{
	public:
		Character();
		Character(float xi,float yi,float l, float r,float t,float b,float rl);

		float left,right;
		float top,bottom;
		float x,y;
		float redline;
	};

	/// Constructor.
	FontMap(Graph::Device *dev);

	/// Destructor.
	~FontMap();

	/**
	* Draw a single character.
	* @param ch Wide character.
	* @param dx Current x position.
	* @param dy Current y position.
	* @param size Character cell size (in pixels).
	* @param bold Use bold characters.
	* @return False if the character doesn't exist in this map, true otherwise.
	*/
	bool DrawChar(unsigned int ch, float &dx,float &dy,float size, bool bold = false);

	/**
	* Draw a text string.
	* @param x String start x position.
	* @param y String start y position.
	* @param size Character cell size (in pixels).
	* @param str Wide characters string text.
	* @param bold Use bold characters.
	*/
	void Draw(float x, float y,float size,std::string str,bool bold = false,FontAlignment align = FONT_ALIGNMENT_LEFT);

	/**
	* Load character map configuration from file.
	* return True if the file was correctly loaded, false otherwise.
	*/
	void Load(const std::string &filename);

	/// Get string length.
	float GetLength(float size,std::string str, bool bold = false);

	///
	void SetPositionAccuracy(FontPositionAccuracy accuracy);

private:

	FontPositionAccuracy m_PositionAccuracy;

	/// Reference to the graphics device.
	Graph::Device *m_gDevice;

	/// Normal and bold characters map.
	std::map<unsigned int,Character> m_CharactersMap[2];
};

_GUI_END

#endif // #ifndef NCK_FONT_H
