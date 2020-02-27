#ifndef EDGE_H
#define EDGE_H

#include <Eigen/StdVector>

class HalfEdge;

class Edge
{
public:
    Edge(int pA , int  pB);
    HalfEdge *myHalfEdge;
    int pointA;
    int pointB;

    bool operator == (const Edge& d) const {
       return pointA == d.pointA && pointB == d.pointB;
    }
};

#endif // EDGE_H
