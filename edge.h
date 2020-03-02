#ifndef EDGE_H
#define EDGE_H


#include "common.h"

class HalfEdge;

class Edge
{
public:
    Edge(int pA , int  pB);
    std::shared_ptr<HalfEdge> myHalfEdge;
    int pointA;
    int pointB;
    void flipEdge();
    void slipEdge();

    bool operator == (const Edge& d) const ;
};

#endif // EDGE_H
