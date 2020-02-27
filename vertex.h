#ifndef VERTEX_H
#define VERTEX_H

#include <Eigen/StdVector>
class HalfEdge;

struct Point
{
    Eigen::Vector3f Point;
};

class Vertex
{
public:
    Vertex();

    HalfEdge* h;

};

#endif // VERTEX_H
