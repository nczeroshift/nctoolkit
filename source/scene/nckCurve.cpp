
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckCurve.h"

_SCENE_BEGIN

BezierNode::BezierNode(const Math::Vec3 & left, const Math::Vec3 & center, const Math::Vec3 & right) {
    m_Left = left;
    m_Center = center;
    m_Right = right;
}

BezierNode::~BezierNode() {

}

Math::Vec3 BezierNode::GetLeft() const {
    return m_Left;
}

Math::Vec3 BezierNode::GetRight() const {
    return m_Right;
}

Math::Vec3 BezierNode::GetCenter() const {
    return m_Center;
}

Curve::Curve(Graph::Device *dev) : Datablock(dev){

}

Curve::~Curve() {
}

void Curve::Read(BXON::Map * entry) {
    m_Name = entry->GetString("name");

    BXON::Array * splines = entry->GetArray("splines");
   
    m_Splines.reserve(splines->GetSize());

    for (int j = 0; j < splines->GetSize(); j++) {
        BXON::Map * n = splines->GetMap(j);
        BXON::Array * left = n->GetArray("left");
        BXON::Array * center = n->GetArray("center");
        BXON::Array * right = n->GetArray("right");

        int count = left->GetSize() / 3;

        std::vector<BezierNode> nodes;
        nodes.reserve(count);

        for (int k = 0; k < count; k++) {
            float lx = left->GetFloat(k * 3 + 0);
            float ly = left->GetFloat(k * 3 + 1);
            float lz = left->GetFloat(k * 3 + 2);

            float cx = center->GetFloat(k * 3 + 0);
            float cy = center->GetFloat(k * 3 + 1);
            float cz = center->GetFloat(k * 3 + 2);

            float rx = right->GetFloat(k * 3 + 0);
            float ry = right->GetFloat(k * 3 + 1);
            float rz = right->GetFloat(k * 3 + 2);

            Math::Vec3 l = Math::Vec3(lx, ly, lz);
            Math::Vec3 c = Math::Vec3(cx, cy, cz);
            Math::Vec3 r = Math::Vec3(rx, ry, rz);

            nodes.push_back(BezierNode(l, c, r));
        }

        m_Splines.push_back(nodes);
    }
    
}

void Curve::Render(float resolution) {
    if (resolution <= 0)
        resolution = 1.0;

    for (int i = 0; i < m_Splines.size(); i++) {
        m_Device->Begin(Graph::PRIMITIVE_LINES);
        float factor = 1.0 / resolution;
        std::vector<BezierNode> nodes = m_Splines[i];
        for (int j = 0; j < nodes.size()-1; j++) {
            for (float a = 0.0; a < 1.0; a += factor)
            {
                Math::Vec3 p1 = nodes[j].GetCenter();
                Math::Vec3 p2 = nodes[j].GetRight();
                Math::Vec3 p3 = nodes[j+1].GetLeft();
                Math::Vec3 p4 = nodes[j+1].GetCenter();

                Math::Vec3 v1 = Math::BezierInterpolation(p1, p2,
                    p3, p4, a);

                Math::Vec3 v2 = Math::BezierInterpolation(p1, p2,
                    p3, p4, a+factor);

                m_Device->Vertex(v1.GetX(), v1.GetY(), v1.GetZ());
                m_Device->Vertex(v2.GetX(), v2.GetY(), v2.GetZ());
            }
        }
        m_Device->End();
    }
}

DatablockType Curve::GetType() {
    return DATABLOCK_CURVE;
}

_SCENE_END
