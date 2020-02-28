#ifndef HALFEDGE_H
#define HALFEDGE_H

#include "common.h"
#include "vertex.h"
#include "edge.h"
#include "face.h"


class HalfEdge
{

public:
    HalfEdge();
    std::shared_ptr<HalfEdge> myOpposite;
    std::shared_ptr<HalfEdge> myNext;
    std::shared_ptr<Vertex> myVertex;
    std::shared_ptr<Edge> myEdge;
    std::shared_ptr<Face> myFace;

};

#endif // HALFEDGE_H
