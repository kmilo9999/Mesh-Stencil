#ifndef VERTEX_H
#define VERTEX_H

#include "common.h"

class HalfEdge;

typedef Eigen::Vector3f Point;

class Vertex
{
public:
    Vertex(Point p);

    std::vector<std::shared_ptr<HalfEdge>> myHalfEdges;
    Point myPosition;
};

#endif // VERTEX_H
