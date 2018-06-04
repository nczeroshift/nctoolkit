
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_TRANSFORM_H
#define NCK_TRANSFORM_H

#include "nckMathConfig.h"

#include "nckVec3.h"
#include "nckVec4.h"
#include "nckMat44.h"

_MATH_BEGIN

enum VectorAxis{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
	AXIS_W = 3,
};

/**
 * Convert normal vector to rotation matrix.
 * @param vec Normal vector.
 * @param axisIndex
 * @param upIndex
 * @param up
 */
Math::Mat44 VecToMat(const Math::Vec3 & vec, VectorAxis axisIndex, VectorAxis upIndex, const Math::Vec3 & up = Math::Vec3(0,0,1));

/**
 * Compute camera matrix aimed to a target.
 * @param target Where the camera is aimed.
 * @param position Where the camera is placed.
 * @param up Camera up vector.
 */
Math::Mat44 LookAt(const Math::Vec3 & target, const Math::Vec3 & position, const Math::Vec3 & up);

Math::Vec3 UnProject(const Math::Mat44 & projection, const Math::Mat44 & view, const Math::Vec2 & point, const Math::Vec2 & vpSize, float winZ = 1.0);

Math::Vec2 Project(const Math::Mat44 & projection, const Math::Mat44 & view, const Math::Vec3 & vec, const Math::Vec2 & vpSize);

_MATH_END

#endif // #ifndef NCK_TRANSFORM_H
