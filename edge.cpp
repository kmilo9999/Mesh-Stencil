#include "edge.h"

Edge::Edge(int a , int b):pointA(a),pointB(b)
{

}

bool Edge::operator==(const Edge& d) const {

    return (pointA == d.pointA && pointB == d.pointB)
            || (pointA == d.pointB && pointB == d.pointA);

}
