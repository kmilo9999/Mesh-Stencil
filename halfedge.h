#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "vertex.h"
#include "edge.h"
#include "face.h"

class HalfEdge
{

public:
    HalfEdge();
    HalfEdge* opposite;
    HalfEdge* next;
    Vertex *vertex;
    Edge *edge;
    Face *face;

};

#endif // HALFEDGE_H
