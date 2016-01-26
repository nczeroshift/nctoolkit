
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckFont.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckDataIO.h"
#include <stdio.h>

_GUI_BEGIN

FontMap::Character::Character()
{
	left = 0;
	right = 0;
	top = 0;
	bottom = 0;
	redline = 0;
}

FontMap::Character::Character(float xi,float yi,float l, float r,float t,float b,float rl)
{
	left = l;
	right = r;
	top = t;
	bottom = b;
	redline = rl;
	x=xi;
	y=yi;
}

FontMap::FontMap(Graph::Device *dev)
{
	m_PositionAccuracy = FONT_POSITION_ACCURACY_FLOAT;
	m_gDevice = dev;
	m_Density = 1.0;
}

FontMap::FontMap(Graph::Device *dev, float density) : FontMap(dev) {
	m_Density = density;
}

FontMap::~FontMap()
{
	m_CharactersMap[0].clear();
	m_CharactersMap[1].clear();
}

void FontMap::Load(const std::string & filename)
{
	char ch;
	Core::FileReader * reader = Core::FileReader::Open(filename);
	
	if(!reader)
		THROW_EXCEPTION("Unable to open the font map file : \"" + filename);

	std::string line_buffer = "";

	bool bold_flag = false;
	float dimensions = 1024;

	while(reader->Read(&ch,1)>0)
	{
		if(ch == '\r')
			continue;

		if(ch == '\n')
		{
			if(line_buffer.size()>1)
			{
				// Comment, ignore.
				if(line_buffer.substr(0,2) == "//")
				{
					line_buffer = "";
					continue;
				}

				std::string first_tag = line_buffer.substr(0,line_buffer.find(","));
				std::string right_str = line_buffer.substr(line_buffer.find(",")+1,line_buffer.length());

				if(first_tag == "Map")
				{
					char ch_id;
					int n_id;
					float redline;
					float left;
					float right;
					float top;
					float bottom;

					sscanf(right_str.c_str(),"%d,%c,%f,%f,%f,%f,%f",&n_id,&ch_id,&redline,&left,&right,&top,&bottom);

					float xi=float(n_id%16)/16.0f;
					float yi=float(n_id/16)/16.0f;


					int map_id = 0;

					if(bold_flag && n_id>=128)
						map_id = 1;

					m_CharactersMap[map_id].insert(std::pair<unsigned int,Character>((unsigned int)ch_id,Character(xi,yi,left/dimensions,
							right/dimensions,
							top/dimensions,
							bottom/dimensions,
							redline/dimensions )));
				}
				else if(first_tag == "Parameter")
				{	
					std::string attribute = right_str.substr(0,right_str.find(","));
					std::string value = right_str.substr(right_str.find(",")+1,right_str.length());

					if(attribute == "Dimension")
					{
						dimensions = (float)atof(value.c_str());
					}
					else if(attribute == "Bold")
					{
						if(value == "True")
							bold_flag = true;
					}	

				}


				line_buffer = "";
			}	
		}
		else
		{
			// Remove tabs or spaces
			if(ch != '\t' && ch != ' ')
				line_buffer += ch;
		}
	}

	SafeDelete(reader);
}

void FontMap::SetPositionAccuracy(FontPositionAccuracy accuracy){
	m_PositionAccuracy = accuracy;
}

bool FontMap::DrawChar(unsigned int ch, float &dx,float &dy,float size, bool bold)
{
	FontMap::Character c;

	if(ch == ' '){
		dx+= 4*size/16.0f;
		return true;
	}

	std::map<unsigned int,Character>::iterator entry;

	if(bold && m_CharactersMap[1].size())
		entry = m_CharactersMap[1].find(ch);

	if(!bold || m_CharactersMap[1].size()==0 || entry == m_CharactersMap[1].end() ){
		entry = m_CharactersMap[0].find(ch);

		if(entry == m_CharactersMap[0].end())
			return false;
	}

	c = entry->second;

	const float off = 0.0625f;
	const float ioff = 16.0f;

	// 0.75f -> Center
	// 0.5f -> Top
	// 1.0f -> Bottom
	float valign = 0.75;


	float yoff =  size*((1.0f - c.redline*ioff)-valign) ;
	float xoff = size*c.left*ioff;

	if(m_PositionAccuracy == FONT_POSITION_ACCURACY_FLOAT)
	{
		m_gDevice->Begin(Graph::PRIMITIVE_QUADS);
		m_gDevice->TexCoord(c.x,c.y);
		m_gDevice->Vertex(dx-xoff,dy+yoff);

		m_gDevice->TexCoord(c.x,c.y+off);
		m_gDevice->Vertex(dx-xoff,dy+size+yoff);

		m_gDevice->TexCoord(c.x+off,c.y+off);
		m_gDevice->Vertex(dx+size-xoff,dy+size+yoff);

		m_gDevice->TexCoord(c.x+off,c.y);
		m_gDevice->Vertex(dx+size-xoff,dy+yoff);
		m_gDevice->End();
	}
	else
	{
		m_gDevice->Begin(Graph::PRIMITIVE_QUADS);
		m_gDevice->TexCoord(c.x,c.y);
		m_gDevice->Vertex((int)(dx-xoff),(int)(dy+yoff));

		m_gDevice->TexCoord(c.x,c.y+off);
		m_gDevice->Vertex((int)(dx-xoff),(int)(dy+size+yoff));

		m_gDevice->TexCoord(c.x+off,c.y+off);
		m_gDevice->Vertex((int)(dx+size-xoff),int(dy+size+yoff));

		m_gDevice->TexCoord(c.x+off,c.y);
		m_gDevice->Vertex((int)(dx+size-xoff),(int)(dy+yoff));
		m_gDevice->End();
	}

	dx+= size*(c.right - c.left)*ioff + 1.0f*size/16.0f;

	return true;

}

void FontMap::Draw(float x, float y,float size,std::string str,bool bold,FontAlignment align)
{
	x *= m_Density;
	y *= m_Density;
	size *= m_Density;

	float dx = x;
	float dy = y;

	std::string temp_str = str;

	if(align == FONT_ALIGNMENT_MIDDLE)
	{
		if(temp_str.find("\n")!=std::string::npos)
			dx-= GetLength(size,temp_str.substr(0,temp_str.find("\n")),bold)/2;
		else
			dx-= GetLength(size,temp_str,bold)/2;
	}

	if(align == FONT_ALIGNMENT_RIGHT)
	{
		if(temp_str.find("\n")!=std::string::npos)
			dx-= GetLength(size,temp_str.substr(0,temp_str.find("\n")),bold);
		else
			dx-= GetLength(size,temp_str,bold);
	}

	for(unsigned int i = 0; i<temp_str.length();i++)
	{
		if(str[i] == L'\n')
		{
			dx = x;
	
			if (i + 1 < temp_str.length()) {
				std::string sStr = temp_str.substr(i + 1);

				if (align == FONT_ALIGNMENT_MIDDLE)
				{
					if (sStr.find("\n") != std::string::npos)
						dx -= GetLength(size, sStr.substr(i, sStr.find("\n")), bold) / 2;
					else
						dx -= GetLength(size, sStr, bold) / 2;
				}

				if (align == FONT_ALIGNMENT_RIGHT)
				{
					if (sStr.find("\n") != std::string::npos)
						dx -= GetLength(size, sStr.substr(i, sStr.find("\n")), bold);
					else
						dx -= GetLength(size, sStr, bold);
				}
			}

			dy+=size;

			continue;
		}
		DrawChar(str[i],dx,dy,size,bold);
	}
}

float FontMap::GetLength(float size,std::string str, bool bold)
{
	float dx = 0;

	for(unsigned int i = 0; i<str.length();i++)
	{
		unsigned int ch = (unsigned int)str[i];

		FontMap::Character c;

		if(ch == ' '){
			dx+= 4*size/16.0f;
			continue;
		}

		if(ch == '\n')
			break;

		std::map<unsigned int,Character>::iterator entry;

		if(bold && m_CharactersMap[1].size())
			entry = m_CharactersMap[1].find(ch);
		if(!bold || m_CharactersMap[1].size()==0 || entry == m_CharactersMap[1].end() )
		{
			entry = m_CharactersMap[0].find(ch);
			if(entry == m_CharactersMap[0].end())
				continue;
		}

		c = entry->second;

		//const float off = 0.0625f;
		const float ioff = 16.0f;

		dx+= size*(c.right - c.left)*ioff + 1.0f*size/16.0f;
	}

	return dx;
}

_GUI_END
