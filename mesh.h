#ifndef MESH_H
#define MESH_H


#include "common.h"
#include "halfedge.h"

EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix2f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3f)
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Matrix3i)

class Mesh
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    void initFromVectors(const std::vector<Eigen::Vector3f> &vertices,
    const std::vector<Eigen::Vector3i> &faces);
    void loadFromFile(const std::string &filePath);
    void saveToFile(const std::string &filePath);

   void createHalfEdges();
   void processeVertexEdges(int index);
   void testFlipEdge(int index);
   void flipEdge(int vertexIndex);
   void splitEdge(int vertexIndex);

private:
   // std::map<unsigned int, std::shared_ptr<Vertex>> myVertices;

    std::vector<std::shared_ptr<Vertex>> myVertices;
    std::vector<Eigen::Vector3f> _vertices;
    std::vector<Eigen::Vector3i> _faces;

    std::vector<HalfEdge> _halfEdges;
    std::vector<Edge> _edges;
    std::vector<Vertex*> _hVertices;

};

#endif // MESH_H
