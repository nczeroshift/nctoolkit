
#ifndef AUTOBFONT_H
#define AUTOBFONT_H

#include <string>

class CellDatai
{
public:
	/// Constructor.
	CellDatai();

	int left;
	int right;
	int top;
	int bottom;
	int redline;
	int xi;
	int yi;
};

void ParseConversionMap(std::string filename, unsigned char **cv);

#endif