
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckFrustum.h"
#include "nckMat44.h"

_MATH_BEGIN

Frustum::Frustum()
{
	for (unsigned int i = 0;i<6;i++)
		m_Plane[i] = Plane();
}

void Frustum::Update(const Mat44 & mv,const Mat44 & proj)
{
	// Based on Frustum setup from NVidia Samples (Practical PSM)
	// and the paper Fast Extraction of Viewing Frustum Planes from World-View-Projection Matrix.
 	Mat44 mvp = mv * proj;

	float *m = (float*)&mvp;

	m_Plane[0] = Plane(m[3] - m[0],	m[7] - m[4],	m[11] - m[8],	m[15] - m[12]); // left
	m_Plane[1] = Plane(m[3] + m[0],	m[7] + m[4],	m[11] + m[8],	m[15] + m[12]); // right
	m_Plane[2] = Plane(m[3] - m[1],	m[7] - m[5],	m[11] - m[9],	m[15] - m[13]); // bottom
	m_Plane[3] = Plane(m[3] + m[1],	m[7] + m[5],	m[11] + m[9],	m[15] + m[13]); // top
	m_Plane[4] = Plane(m[3] - m[2],	m[7] - m[6],	m[11] - m[10],	m[15] - m[14]); // near
	m_Plane[5] = Plane(m[3] + m[2],	m[7] + m[6],	m[11] + m[10],	m[15] + m[14]); // far

	for ( int i = 0; i < 6; i++ )
		m_Plane[i] = Normalize(m_Plane[i]);
}

bool Frustum::CheckSphere(const Vec3& center,const float & radius) const
{
	for(unsigned int i = 0; i < 6; i++ ) {
	if ( Distance(m_Plane[i],center) < -radius )
		return false;
	}
	return true;
}

bool Frustum::CheckCube(const Vec3 & min,const Vec3 & max) const
{
	for ( int i = 0; i < 6; i++ ) {
		if ( Distance(m_Plane[i],Vec3(min.GetX(), min.GetY(), min.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(min.GetX(), min.GetY(), max.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(min.GetX(), max.GetY(), min.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(min.GetX(), max.GetY(), max.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(max.GetX(), min.GetY(), min.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(max.GetX(), min.GetY(), max.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(max.GetX(), max.GetY(), min.GetZ())) > 0.0f )
			continue;

		if ( Distance(m_Plane[i],Vec3(max.GetX(), max.GetY(), max.GetZ())) > 0.0f )
			continue;

		return false;
	}
	return true;
}

_MATH_END
