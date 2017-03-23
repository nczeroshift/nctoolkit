
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_CURVE_H
#define NCK_CURVE_H

#include "nckGraphics.h"
#include "nckDatablock.h"
#include "nckAnimation.h"
#include "nckMat44.h"
#include "nckVec3.h"
#include "nckQuat.h"
#include "bxon.hpp"
#include <vector>
#include <list>

_SCENE_BEGIN

class BezierNode {
public:
    BezierNode(const Math::Vec3 & left, const Math::Vec3 & center, const Math::Vec3 & right);
    ~BezierNode();

    Math::Vec3 GetLeft() const;
    Math::Vec3 GetRight() const;
    Math::Vec3 GetCenter() const;

private:
    Math::Vec3 m_Left;
    Math::Vec3 m_Center;
    Math::Vec3 m_Right;
};

class Curve : public Datablock
{
public:
    /// Constructor.
    Curve(Graph::Device *dev);

    /// Destructor.
    ~Curve();

    /// Load data from file.
    /// @param f Reference to file stream.
    void Read(Core::DataReader *f);

#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif

    /// Get datablock type.
    DatablockType GetType();

    void Render(float resolution = 12.0);

private:
    std::vector<std::vector<BezierNode>> m_Splines;
};

_SCENE_END

#endif // #ifndef NCK_CURVE_H
