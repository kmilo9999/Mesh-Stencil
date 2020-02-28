#include "mesh.h"

#include <iostream>
#include <fstream>

#include <QFileInfo>
#include <QString>


#define TINYOBJLOADER_IMPLEMENTATION
#include "util/tiny_obj_loader.h"

using namespace Eigen;
using namespace std;

void Mesh::initFromVectors(const std::vector<Vector3f> &vertices,
           const std::vector<Vector3i> &faces)
{
    _vertices = vertices;
    _faces = faces;
}

void Mesh::loadFromFile(const std::string &filePath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    QFileInfo info(QString(filePath.c_str()));
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                                info.absoluteFilePath().toStdString().c_str(), (info.absolutePath().toStdString() + "/").c_str(), true);
    if(!err.empty()) {
        std::cerr << err << std::endl;
    }

    if(!ret) {
        std::cerr << "Failed to load/parse .obj file" << std::endl;
        return;
    }

    for(size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shapes[s].mesh.num_face_vertices[f];

            Vector3i face;
            for(size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

		face[v] = idx.vertex_index;

            }
            _faces.push_back(face);

            index_offset += fv;
        }
    }
    for(size_t i = 0; i < attrib.vertices.size(); i += 3) {
	_vertices.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
    }
    std::cout << "Loaded " << _faces.size() << " faces and " << _vertices.size() << " vertices" << std::endl;
}

void Mesh::saveToFile(const std::string &filePath)
{
    std::ofstream outfile;
    outfile.open(filePath);

    // Write vertices
    for (size_t i = 0; i < _vertices.size(); i++)
    {
        const Vector3f &v = _vertices[i];
        outfile << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    }

    // Write faces
    for (size_t i = 0; i < _faces.size(); i++)
    {
        const Vector3i &f = _faces[i];
        outfile << "f " << (f[0]+1) << " " << (f[1]+1) << " " << (f[2]+1) << std::endl;
    }

    outfile.close();
}

void Mesh::createHalfEdges()
{

   std::map<std::string ,std::shared_ptr<HalfEdge>> edges;

   for(Vector3i f: _faces)
   {
      std::shared_ptr<Face> face(new Face());
      for(int i = 0 ; i < 3;++i)
      {
          unsigned int index1 = f[i];
          unsigned int index2 = f[i+1%3];

          std::map<unsigned int ,std::shared_ptr<Vertex>>::iterator vertexIndexIt =
                  myVertices.find(index1);

          std::shared_ptr<Vertex> currentVertex;
          if(vertexIndexIt == myVertices.end())
          {
            currentVertex = std::shared_ptr<Vertex>(new Vertex(_vertices[f[i]]));
          }
          else
          {
            currentVertex = vertexIndexIt->second;
          }

          std::string strKey = std::to_string(index1)+","+std::to_string(index2);
          std::string strInvKey = std::to_string(index2)+","+std::to_string(index1);


          std::shared_ptr<HalfEdge> halfEdg(new HalfEdge());
          std::shared_ptr<Edge> ed(new Edge(f[i],f[i+1%3]));
          std::map<std::string ,std::shared_ptr<HalfEdge>>::iterator it =
              edges.find(strKey);
          std::map<std::string ,std::shared_ptr<HalfEdge>>::iterator it2 =
              edges.find(strInvKey);

          if(it == edges.end())
          {
            ed->myHalfEdge = halfEdg;
            currentVertex->myHalfEdges.push_back(halfEdg);
            face->myHalfEdge = halfEdg;
            halfEdg->myEdge = ed;
          }

          if(it2 != edges.end())
          {
             halfEdg->myOpposite = it2->second;
             edges.erase(it);
             edges.erase(it2);

          }


      }
   }
}
