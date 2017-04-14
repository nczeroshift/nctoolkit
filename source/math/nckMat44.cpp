
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckMat44.h"
#include "nckQuat.h"
#include "nckVec4.h"
#include "nckVec3.h"
#include "nckVec2.h"

_MATH_BEGIN

Mat44::Mat44()
{
	
}

Mat44::Mat44(const float *vec)
{
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			m_n[i][j] = vec[i*4+j];
			
}

Mat44 Identity()
{
	Mat44 mat;
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			if(i == j)
				mat.m_n[i][j] = 1.0f;
			else
				mat.m_n[i][j] = 0.0f;
	return mat;
}

Mat44 & Mat44::operator *= (const Mat44 & mat){
	return *this = mat * *this;
}

Mat44 & Mat44::operator = (const Mat44 & mat){
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			m_n[i][j] = mat.m_n[i][j];
	return *this;
}

Mat44 operator * (const Mat44 & matA, const Mat44 & matB){
	Mat44 res;
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
			res.m_n[i][j] = matA.m_n[i][0]*matB.m_n[0][j]+
							matA.m_n[i][1]*matB.m_n[1][j]+
							matA.m_n[i][2]*matB.m_n[2][j]+
							matA.m_n[i][3]*matB.m_n[3][j];
	return res;
}

Mat44 Translate(const Vec3 & vec){
	Mat44 temp = Identity();
	temp.m_n[3][0] = vec.GetX();
	temp.m_n[3][1] = vec.GetY();
	temp.m_n[3][2] = vec.GetZ();
	return temp;
}


Mat44 Scale(const Vec3 & vec){
	Mat44 temp = Identity();
	temp.m_n[0][0] = vec.GetX();
	temp.m_n[1][1] = vec.GetY();
	temp.m_n[2][2] = vec.GetZ();
	return temp;
}


Vec3 operator * (const Vec3 & vec,const Mat44 & mat)
{
	Vec3 vec1(mat.m_n[0][0],mat.m_n[1][0],mat.m_n[2][0]);
	Vec3 vec2(mat.m_n[0][1],mat.m_n[1][1],mat.m_n[2][1]);
	Vec3 vec3(mat.m_n[0][2],mat.m_n[1][2],mat.m_n[2][2]);

	return Vec3(Dot(vec1,vec),Dot(vec2,vec),Dot(vec3,vec));
}

Vec2 operator * (const Vec2& vec,const Mat44& mat)
{
	Vec2 vec1(mat.m_n[0][0],mat.m_n[1][0]);
	Vec2 vec2(mat.m_n[0][1],mat.m_n[1][1]);

	return Vec2(Dot(vec1,vec),Dot(vec2,vec));
}


Vec4 operator * (const Vec4& vec,const Mat44 & mat)
{
	Vec4 vec1(mat.m_n[0][0],mat.m_n[1][0],mat.m_n[2][0],mat.m_n[3][0]);
	Vec4 vec2(mat.m_n[0][1],mat.m_n[1][1],mat.m_n[2][1],mat.m_n[3][1]);
	Vec4 vec3(mat.m_n[0][2],mat.m_n[1][2],mat.m_n[2][2],mat.m_n[3][2]);
	Vec4 vec4(mat.m_n[0][3],mat.m_n[1][3],mat.m_n[2][3],mat.m_n[3][3]);

	return Vec4(Dot(vec1,vec),Dot(vec2,vec),Dot(vec3,vec),Dot(vec4,vec));
}

Vec4 operator * (const Mat44& mat ,const Vec4& vec)
{
	Vec4 vec1(mat.m_n[0][0],mat.m_n[0][1],mat.m_n[0][2],mat.m_n[0][3]);
	Vec4 vec2(mat.m_n[1][0],mat.m_n[1][1],mat.m_n[1][2],mat.m_n[1][3]);
	Vec4 vec3(mat.m_n[2][0],mat.m_n[2][1],mat.m_n[2][2],mat.m_n[2][3]);
	Vec4 vec4(mat.m_n[3][0],mat.m_n[3][1],mat.m_n[3][2],mat.m_n[3][3]);

	return Vec4(Dot(vec1,vec),Dot(vec2,vec),Dot(vec3,vec),Dot(vec4,vec));
}


Vec3 operator * (const Mat44& mat ,const Vec3& vec)
{
	Vec3 vec1(mat.m_n[0][0],mat.m_n[0][1],mat.m_n[0][2]);
	Vec3 vec2(mat.m_n[1][0],mat.m_n[1][1],mat.m_n[1][2]);
	Vec3 vec3(mat.m_n[2][0],mat.m_n[2][1],mat.m_n[2][2]);

	return Vec3(Dot(vec1,vec),Dot(vec2,vec),Dot(vec3,vec));
}

Vec2 operator * (const Mat44& mat,const Vec2& vec)
{
	Vec2 vec1(mat.m_n[0][0],mat.m_n[0][1]);
	Vec2 vec2(mat.m_n[1][0],mat.m_n[1][1]);
	return Vec2(Dot(vec1,vec),Dot(vec2,vec));
}

Mat44 Transpose(const Mat44& mat)
{
	Mat44 res;
	for(int i = 0;i<4;i++)
		for(int j = 0;j<4;j++)
		{
			res.m_n[i][j] = mat.m_n[j][i];
		}
	return res;
}

Mat44 RotateX(const float & alpha)
{
	//http://en.wikipedia.org/wiki/Rotation_matrix

	Mat44 res = Identity();

	res.m_n[1][1] = cos(alpha);
	res.m_n[1][2] = sin(alpha);
	res.m_n[2][1] = -sin(alpha);
	res.m_n[2][2] = cos(alpha);

	return res;
}

Mat44 RotateY(const float & alpha)
{
	Mat44 res = Identity();
	res.m_n[0][0] = cos(alpha);
	res.m_n[0][2] = -sin(alpha);
	res.m_n[2][2] = cos(alpha);
	res.m_n[2][0] = sin(alpha);
	return res;
}

Mat44 RotateZ(const float & alpha)
{
	Mat44 res = Identity();
	res.m_n[0][0] = cos(alpha);
	res.m_n[0][1] = sin(alpha);
	res.m_n[1][0] = -sin(alpha);
	res.m_n[1][1] = cos(alpha);
	return res;
}


bool Invert(const Mat44& mat,Mat44 *target)
{
	if(!target)
		return false;

	// Tricks of the 3d game programming gurus, pag 433.
	float det =  ( mat.m_n[0][0] * ( mat.m_n[1][1] * mat.m_n[2][2] - mat.m_n[1][2] * mat.m_n[2][1] ) -
				   mat.m_n[0][1] * ( mat.m_n[1][0] * mat.m_n[2][2] - mat.m_n[1][2] * mat.m_n[2][0] ) +
				   mat.m_n[0][2] * ( mat.m_n[1][0] * mat.m_n[2][1] - mat.m_n[1][1] * mat.m_n[2][0] ) );

	// test determinate to see if it's 0
	if (fabs(det) < 1E-5)
	   return false;

	float det_inv  = 1.0f / det;

	target->m_n[0][0] =  det_inv * ( mat.m_n[1][1] * mat.m_n[2][2] - mat.m_n[1][2] * mat.m_n[2][1] );
	target->m_n[0][1] = -det_inv * ( mat.m_n[0][1] * mat.m_n[2][2] - mat.m_n[0][2] * mat.m_n[2][1] );
	target->m_n[0][2] =  det_inv * ( mat.m_n[0][1] * mat.m_n[1][2] - mat.m_n[0][2] * mat.m_n[1][1] );
	target->m_n[0][3] = 0.0f; // always 0

	target->m_n[1][0] = -det_inv * ( mat.m_n[1][0] * mat.m_n[2][2] - mat.m_n[1][2] * mat.m_n[2][0] );
	target->m_n[1][1] =  det_inv * ( mat.m_n[0][0] * mat.m_n[2][2] - mat.m_n[0][2] * mat.m_n[2][0] );
	target->m_n[1][2] = -det_inv * ( mat.m_n[0][0] * mat.m_n[1][2] - mat.m_n[0][2] * mat.m_n[1][0] );
	target->m_n[1][3] = 0.0f; // always 0

	target->m_n[2][0] =  det_inv * ( mat.m_n[1][0] * mat.m_n[2][1] - mat.m_n[1][1] * mat.m_n[2][0] );
	target->m_n[2][1] = -det_inv * ( mat.m_n[0][0] * mat.m_n[2][1] - mat.m_n[0][1] * mat.m_n[2][0] );
	target->m_n[2][2] =  det_inv * ( mat.m_n[0][0] * mat.m_n[1][1] - mat.m_n[0][1] * mat.m_n[1][0] );
	target->m_n[2][3] = 0.0f; // always 0

	target->m_n[3][0] = -( mat.m_n[3][0] * target->m_n[0][0] + mat.m_n[3][1] * target->m_n[1][0] + mat.m_n[3][2] * target->m_n[2][0] );
	target->m_n[3][1] = -( mat.m_n[3][0] * target->m_n[0][1] + mat.m_n[3][1] * target->m_n[1][1] + mat.m_n[3][2] * target->m_n[2][1] );
	target->m_n[3][2] = -( mat.m_n[3][0] * target->m_n[0][2] + mat.m_n[3][1] * target->m_n[1][2] + mat.m_n[3][2] * target->m_n[2][2] );
	target->m_n[3][3] = 1.0f; // always 0

	return true;
}

Mat44 Rotate(const Quat& q)
{
	// 3D Math Primer For Graphics & Game Development
	// Pages 185-187
	float x = q.GetX();
	float y = q.GetY();
	float z = q.GetZ();
	float w = q.GetW();

	Mat44 mat;
	mat.m_n[0][0] = 1.0f - 2.0f * ( y * y + z * z ); 
	mat.m_n[0][1] = 2.0f * (x * y + z * w);
	mat.m_n[0][2] = 2.0f * (x * z - y * w);
	mat.m_n[0][3] = 0.0f;  

	mat.m_n[1][0] = 2.0f * ( x * y - z * w );  
	mat.m_n[1][1] = 1.0f - 2.0f * ( x * x + z * z ); 
	mat.m_n[1][2] = 2.0f * (z * y + x * w );  
	mat.m_n[1][3] = 0.0f;  

	mat.m_n[2][0]= 2.0f * ( x * z + y * w );
	mat.m_n[2][1] = 2.0f * ( y * z - x * w );
	mat.m_n[2][2] = 1.0f - 2.0f * ( x * x + y * y );  
	mat.m_n[2][3] = 0.0f;  

	mat.m_n[3][0] = 0;  
	mat.m_n[3][1] = 0;  
	mat.m_n[3][2] = 0;  
	mat.m_n[3][3] = 1.0f;

	return mat;
}

Mat44 RotationPart(const Mat44 & mat)
{
	Mat44 mat_r = mat;

	mat_r.m_n[3][0] = 0;
	mat_r.m_n[3][1] = 0;
	mat_r.m_n[3][2] = 0;

	return mat_r;
}

Quat Rotation(const Mat44 & mat)
{
	// 3D Math Primer For Graphics & Game Development
	// Pages 190

	// Input matrix:
	float mat11 = mat.m_n[0][0],mat12=mat.m_n[0][1],mat13=mat.m_n[0][2];
	float mat21 = mat.m_n[1][0],mat22=mat.m_n[1][1],mat23=mat.m_n[1][2];
	float mat31 = mat.m_n[2][0],mat32=mat.m_n[2][1],mat33=mat.m_n[2][2];

	// Output quaternion
	float w,x,y,z;

	// Determine which of w, x, y, or z has the largest absolute value
	float fourWSquaredMinus1 = mat11 + mat22 + mat33;
	float fourXSquaredMinus1 = mat11 - mat22 - mat33;
	float fourYSquaredMinus1 = mat22 - mat11 - mat33;
	float fourZSquaredMinus1 = mat33 - mat11 - mat22;

	int biggestIndex = 0;

	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	// Perform square root and division
	float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	float mult = 0.25f / biggestVal;

	// Apply table to compute quaternion values
	switch (biggestIndex) {
		case 0:
			w = biggestVal;
			x = (mat23 - mat32) * mult;
			y = (mat31 - mat13) * mult;
			z = (mat12 - mat21) * mult;
			break;
		case 1:
			x = biggestVal;
			w = (mat23 - mat32) * mult;
			y = (mat12 + mat21) * mult;
			z = (mat31 + mat13) * mult;
			break;
		case 2:
			y = biggestVal;
			w = (mat31 - mat13) * mult;
			x = (mat12 + mat21) * mult;
			z = (mat23 + mat32) * mult;
			break;
		case 3:
			z = biggestVal;
			w = (mat12 - mat21) * mult;
			x = (mat31 + mat13) * mult;
			y = (mat23 + mat32) * mult;
			break;
	}

	return Quat(x,y,z,w);
}

Vec3 TranslationPart(const Mat44 & mat){
	return Vec3(mat.m_n[3][0],mat.m_n[3][1],mat.m_n[3][2]);
}

Mat44 Ortho(float left, float top, float right, float bottom, float znear, float zfar) {
    float m[16] = {
       2.0f / (right - left), 0, 0, 0,
       0, 2.0f / (top - bottom), 0, 0,
       0, 0, -2.0f / (zfar - znear), 0,
       -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zfar + znear) / (zfar - znear), 1
    };
    return Mat44(m);
}

Mat44 Perspective(float aspect, float fov, float znear, float zfar) {
    float xymax = znear * tan(fov * M_PI / 360.0);
    float ymin = -xymax;
    float xmin = -xymax;

    float width = xymax - xmin;
    float height = xymax - ymin;

    float depth = zfar - znear;
    float q = -(zfar + znear) / depth;
    float qn = -2 * (zfar * znear) / depth;

    float w = 2 * znear / width;
    w = w / aspect;
    float h = 2 * znear / height;

    float m[16];

    m[0] = w;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;

    m[4] = 0;
    m[5] = h;
    m[6] = 0;
    m[7] = 0;

    m[8] = 0;
    m[9] = 0;
    m[10] = q;
    m[11] = -1;

    m[12] = 0;
    m[13] = 0;
    m[14] = qn;
    m[15] = 0;

    return Mat44(m);
}

Mat44 CubemapTransform(const Vec3 & position, int faceId) {
    Vec3 orientation;
    switch (faceId) {
    case 0:
        orientation = Math::Vec3(-180, -270, 0); break; // X+
    case 1:
        orientation = Math::Vec3(180, -90, 0); break;   // X-
    case 2:
        orientation = Math::Vec3(90, 0, 0); break;      // Y+
    case 3:
        orientation = Math::Vec3(-90, 0, 0); break;     // Y-
    case 4:
        orientation = Math::Vec3(180, 0, 0); break;     // Z+
    case 5:
        orientation = Math::Vec3(0, 0, 180); break;     // Z-
    default:
        break;
    };

    return Math::Translate(Math::Vec3(-position.GetX(), -position.GetY(), -position.GetZ())) * Math::RotateZ(-orientation.GetZ() * M_PI / 180) *
        Math::RotateY(-orientation.GetY() * M_PI / 180) *  Math::RotateX(-orientation.GetX() * M_PI / 180);
}

_MATH_END