#ifndef VERTEX_H
#define VERTEX_H

#include "common.h"

class HalfEdge;

typedef Eigen::Vector3f Point;

class Vertex
{
public:
    Vertex(Point p);

    std::shared_ptr<HalfEdge> myHalfEdge;
    Point myPosition;
    int myIndex;
};

#endif // VERTEX_H
