
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_VPROFILE_H
#define NCK_VPROFILE_H

#include <stdlib.h>
#include <vector>

_GRAPHICS_BEGIN

#define VERTEX_POSITION		1
#define VERTEX_NORMAL		2
#define VERTEX_UV           4
#define VERTEX_COLOR		8

struct VertexElement{
	unsigned int offset;
	unsigned int components;
	unsigned int type;
	unsigned int index;
};

/// Structure of vertex data elements.
class VertexProfile
{
public:
	VertexProfile(){}

	VertexProfile(int capacity){
		m_Elements.reserve(capacity);
	}

	~VertexProfile()
	{
		m_Elements.clear();
	}

	VertexElement operator [](int id) const{
		return m_Elements[id];
	}

	unsigned int GetVertexSize(){
		unsigned int size = 0;
		for(size_t i = 0;i<m_Elements.size();i++)
		{
			switch (m_Elements[i].type)
			{
				case VERTEX_POSITION:
					size += 4*m_Elements[i].components;
					continue;

				case VERTEX_NORMAL:
					size+=4*3;
					continue;

				case VERTEX_UV:
					size+=4*m_Elements[i].components;
					continue;

				case VERTEX_COLOR:
					size+=m_Elements[i].components;
					continue;

				default:
					continue;
			}
		}
		return size;
	}

	void PushBack(unsigned int offset,unsigned int components,unsigned int type,unsigned int index)
	{
		if(m_Elements.size() == m_Elements.capacity())
			return;

		VertexElement element;
		element.offset = offset;
		element.components = components;
		element.type = type;
		element.index = index;

		m_Elements.push_back(element);
	}

	unsigned int Size() const{
		return (int)m_Elements.size();
	}

private:
	std::vector<VertexElement>	m_Elements;
};

// Basic vertex data support class.
class BasicVertex
{
public:
	/// Constructor
	BasicVertex()
	{
		m_Loc[0]=0;m_Loc[1]=0;m_Loc[2]=0;
		m_Nor[0]=0;m_Nor[1]=0;m_Nor[2]=0;
		m_Color[0]=0;m_Color[1]=0;m_Color[2]=0;m_Color[3]=0;
		m_Tex[0]=0;m_Tex[1]=0;
	}

	~BasicVertex(){}

	/// Position
	float m_Loc[3];

	/// Normal
	float m_Nor[3];

	/// Color
	unsigned char m_Color[4];

	/// Texture UV Coordinates
	float m_Tex[2];
};

_GRAPHICS_END

#endif // #ifndef NCK_VPROFILE_H
