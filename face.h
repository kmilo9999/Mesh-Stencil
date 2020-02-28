#ifndef FACE_H
#define FACE_H

#include "common.h"
class HalfEdge;

class Face
{
public:
    Face();
    std::unique_ptr<HalfEdge> myHalfEdge;
};

#endif // FACE_H
