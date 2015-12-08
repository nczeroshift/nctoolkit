
/**
* NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckBoundBox.h"
#include "nckMat44.h"
#include "nckVec4.h"

_MATH_BEGIN

BoundBox::BoundBox()
{
	m_Initialized = false;
	m_Max=Vec3(0,0,0);
	m_Min=Vec3(0,0,0);
}

BoundBox::BoundBox(const Vec3 & max,const Vec3 & min)
{
	m_Initialized = true;
	m_Max=Vec3(max.GetX(),max.GetY(),max.GetZ());
	m_Min=Vec3(min.GetX(),min.GetY(),min.GetZ());
}
	
void BoundBox::Insert(const Vec3 & v)
{
	if(!m_Initialized)
	{
		m_Max = v;
		m_Min = v;
		m_Initialized = true;
		return;
	}

	if(v.GetX()> m_Max.GetX())
		m_Max.SetX(v.GetX());
	if(v.GetX()< m_Min.GetX())
		m_Min.SetX(v.GetX());

	if(v.GetY()> m_Max.GetY())
		m_Max.SetY(v.GetY());
	if(v.GetY()< m_Min.GetY())
		m_Min.SetY(v.GetY());

	if(v.GetZ()> m_Max.GetZ())
		m_Max.SetZ(v.GetZ());
	if(v.GetZ()< m_Min.GetZ())
		m_Min.SetZ(v.GetZ());
}

BoundBox BoundBox::operator *(const Mat44 & mat){
	BoundBox bb;

	Vec4 cube[8] ={	Vec4(m_Min.GetX(),m_Min.GetY(),m_Max.GetZ(),1)*mat,
					Vec4(m_Max.GetX(),m_Max.GetY(),m_Max.GetZ(),1)*mat,
					Vec4(m_Min.GetX(),m_Max.GetY(),m_Max.GetZ(),1)*mat,
					Vec4(m_Max.GetX(),m_Min.GetY(),m_Max.GetZ(),1)*mat,

					Vec4(m_Max.GetX(),m_Max.GetY(),m_Min.GetZ(),1)*mat,
					Vec4(m_Min.GetX(),m_Max.GetY(),m_Min.GetZ(),1)*mat,
					Vec4(m_Max.GetX(),m_Min.GetY(),m_Min.GetZ(),1)*mat,
					Vec4(m_Min.GetX(),m_Min.GetY(),m_Min.GetZ(),1)*mat};

	bb.m_Min = bb.m_Max = Vec3(cube[0].GetX(),cube[0].GetY(),cube[0].GetZ());

	for(int i = 0;i<8;i++)
		bb.Insert( Vec3(cube[i].GetX(),cube[i].GetY(),cube[i].GetZ()));
				
	return bb;
}

bool Inside(const BoundBox & bb, const Math::Vec3 & v){
	if(v.GetX() <= bb.GetMax().GetX() && v.GetX() >= bb.GetMin().GetX() &&
		v.GetY() <= bb.GetMax().GetY() && v.GetY() >= bb.GetMin().GetY() &&
		v.GetZ() <= bb.GetMax().GetZ() && v.GetZ() >= bb.GetMin().GetZ()){
			return true;
	}
	return false;
}

bool Intersect(const BoundBox & bb1, const BoundBox & bb2)
{
	return (bb1.GetMin().GetX() <= bb2.GetMax().GetX()) && (bb1.GetMax().GetX() >= bb2.GetMin().GetX()) &&
        (bb1.GetMin().GetY() <= bb2.GetMax().GetY()) && (bb1.GetMax().GetY() >= bb2.GetMin().GetY()) &&
		(bb1.GetMin().GetZ() <= bb2.GetMax().GetZ()) && (bb1.GetMax().GetZ() >= bb2.GetMin().GetZ());
}

_MATH_END
