#ifndef FACE_H
#define FACE_H

#include "common.h"
class HalfEdge;

class Face
{
public:
    Face();
    std::shared_ptr<HalfEdge> myHalfEdge;
    Eigen::Vector3i myFace;
};

#endif // FACE_H
