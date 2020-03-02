#include "edge.h"
#include "halfedge.h"

Edge::Edge(int a , int b):pointA(a),pointB(b)
{

}

bool Edge::operator==(const Edge& d) const {

    return (pointA == d.pointA && pointB == d.pointB)
            || (pointA == d.pointB && pointB == d.pointA);

}

void Edge::flipEdge()
{
   if(myHalfEdge != nullptr && myHalfEdge->myOpposite != nullptr)
   {
       std::shared_ptr<Vertex> v1 = this->myHalfEdge->myNext->myNext->myVertex;
       std::shared_ptr<Vertex> v2 = this->myHalfEdge->myOpposite->myNext->myNext->myVertex;

       std::shared_ptr<Face> f1 =  this->myHalfEdge->myFace;
       std::shared_ptr<Face> f2 =  this->myHalfEdge->myOpposite->myFace;

       //unsigned int c  = this->pointA;
       //unsigned int b  = this->pointB;

       unsigned int a =  v1->myIndex;
       unsigned int d = v2->myIndex;

       this->pointA = d;
       this->pointB = a;

       std::shared_ptr<HalfEdge> tmp = this->myHalfEdge->myNext;
       std::shared_ptr<HalfEdge> tmp2 = this->myHalfEdge->myOpposite->myNext->myNext;

       // update vertex
       this->myHalfEdge->myVertex = this->myHalfEdge->myOpposite->myNext->myNext->myVertex;
       this->myHalfEdge->myVertex->myHalfEdge = this->myHalfEdge;

       // update next halfedge
       this->myHalfEdge->myNext = this->myHalfEdge->myNext->myNext;
       this->myHalfEdge->myNext->myNext = this->myHalfEdge->myOpposite->myNext;

       // update next next halfedge
       this->myHalfEdge->myNext->myNext->myNext =this->myHalfEdge;

       //update face
       std::shared_ptr<HalfEdge> h = this->myHalfEdge;
       Eigen::Vector3i newFace(0.0,0.0,0.0);
       unsigned int index = 0;
       do
       {
           newFace[index++] = h->myVertex->myIndex;
           h = h->myNext;
       }while(h != this->myHalfEdge);

       //update faces
       this->myHalfEdge->myFace->myFaceIndexes = newFace;
       this->myHalfEdge->myFace->myHalfEdge = this->myHalfEdge;

       h = this->myHalfEdge;
       do
       {
           h->myFace->myFaceIndexes = newFace;
           h = h->myNext;
       }while(h != this->myHalfEdge);

       //update oppossite
       this->myHalfEdge->myOpposite->myEdge->pointA =  a;
       this->myHalfEdge->myOpposite->myEdge->pointB =  d;

       //update oppossite vertex
       this->myHalfEdge->myOpposite->myVertex = this->myHalfEdge->myNext->myVertex;
       this->myHalfEdge->myOpposite->myVertex->myHalfEdge = this->myHalfEdge->myOpposite;

       this->myHalfEdge->myOpposite->myNext = tmp2;
       this->myHalfEdge->myOpposite->myNext->myNext = tmp;
       this->myHalfEdge->myOpposite->myNext->myNext->myNext = this->myHalfEdge->myOpposite;

       //update edge
       this->myHalfEdge->myOpposite->myEdge->myHalfEdge = this->myHalfEdge->myOpposite;

       Eigen::Vector3i newOppositeFace(0.0,0.0,0.0);
       index = 0;
       h = this->myHalfEdge->myOpposite;
       do
       {
           newOppositeFace[index++] = h->myVertex->myIndex;
           h = h->myNext;
       }while(h != this->myHalfEdge->myOpposite);

       h =  this->myHalfEdge->myOpposite;
       do
       {
           h->myFace->myFaceIndexes = newFace;
           h = h->myNext;
       }while(h != this->myHalfEdge->myOpposite);

      this->myHalfEdge->myOpposite->myFace->myHalfEdge = this->myHalfEdge->myOpposite;
   }
}

void Edge::slipEdge()
{
   //create mid vertex
   std::shared_ptr<Vertex> v1 = this->myHalfEdge->myVertex;
   std::shared_ptr<Vertex> v2 = this->myHalfEdge->myNext->myVertex;

   std::shared_ptr<Vertex> v3 = this->myHalfEdge->myNext->myNext->myVertex;
   std::shared_ptr<Vertex> v4 = this->myHalfEdge->myOpposite->myNext->myNext->myVertex;


   Eigen::Vector3f midPoint = (v2->myPosition - v1->myPosition) / 2;
   std::shared_ptr<Vertex> v5(new Vertex(midPoint));


  //new edges
  //  std::shared_ptr<Edge>(new Edge());




}
