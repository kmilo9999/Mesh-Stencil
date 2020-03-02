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

   myVertices = std::vector<std::shared_ptr<Vertex>>(_vertices.size(),nullptr);
   std::map<std::string ,std::shared_ptr<Edge>> edges;

   for(Vector3i f: _faces)
   {

      std::shared_ptr<Face> face(new Face());
      std::shared_ptr<HalfEdge> before ;
      std::shared_ptr<HalfEdge> first ;


      for(int i = 0 ; i < 3;++i)
      {

          unsigned int index1 = f[i];
          unsigned int index2 = f[(i+1)%3];

          std::shared_ptr<Vertex> currentVertex;
          if(myVertices[index1] == nullptr)
          {
            currentVertex = std::shared_ptr<Vertex>(new Vertex(_vertices[f[i]]));
            currentVertex->myIndex = index1;
            myVertices[index1] = currentVertex;
          }
          else
          {
            currentVertex = myVertices[index1];
          }

          std::string strKey = std::to_string(index1)+","+std::to_string(index2);
          std::string strInvKey = std::to_string(index2)+","+std::to_string(index1);


          std::shared_ptr<HalfEdge> currentHalfEdge(new HalfEdge());
          if(i == 0)
          {
            first = currentHalfEdge;
          }

          std::shared_ptr<Edge> currentEdge(new Edge(f[i],f[(i+1)%3]));
          std::map<std::string ,std::shared_ptr<Edge>>::iterator it =
              edges.find(strKey);
          std::map<std::string ,std::shared_ptr<Edge>>::iterator it2 =
              edges.find(strInvKey);


          if(it == edges.end())
          {
            edges[strKey] = currentEdge;
            currentEdge->myHalfEdge = currentHalfEdge;
            currentVertex->myHalfEdge = currentHalfEdge;
            face->myHalfEdge = currentHalfEdge;
            face->myFaceIndexes = f;
            currentHalfEdge->myEdge = currentEdge;
            currentHalfEdge->myFace = face;
            currentHalfEdge->myVertex = currentVertex;

            if(before != nullptr)
            {
               before->myNext = currentHalfEdge;
            }

          }

          if(it2 != edges.end())
          {
             currentHalfEdge->myOpposite = it2->second->myHalfEdge;
             if(it2->second->myHalfEdge->myOpposite == nullptr)
             {
                 it2->second->myHalfEdge->myOpposite = currentHalfEdge;
             }
             it = edges.find(strKey);
             edges.erase(it);
             edges.erase(it2);
          }

          before = currentHalfEdge;

      }

     before->myNext = first;


   }
}

void Mesh::flipEdge(int vertexIndex)
{
    std::shared_ptr<Vertex> v = myVertices[vertexIndex];
    std::shared_ptr<Edge> edge = v->myHalfEdge->myEdge;
    if(edge->myHalfEdge != nullptr && edge->myHalfEdge->myOpposite != nullptr)
    {
        std::shared_ptr<Vertex> v1 = edge->myHalfEdge->myNext->myNext->myVertex;
        std::shared_ptr<Vertex> v2 = edge->myHalfEdge->myOpposite->myNext->myNext->myVertex;

        std::shared_ptr<Face> f1 =  edge->myHalfEdge->myFace;
        std::shared_ptr<Face> f2 =  edge->myHalfEdge->myOpposite->myFace;

        //unsigned int c  = this->pointA;
        //unsigned int b  = this->pointB;

        unsigned int a =  v1->myIndex;
        unsigned int d = v2->myIndex;

        edge->pointA = d;
        edge->pointB = a;

        std::shared_ptr<HalfEdge> tmp = edge->myHalfEdge->myNext;
        std::shared_ptr<HalfEdge> tmp2 = edge->myHalfEdge->myOpposite->myNext->myNext;

        // update vertex
        edge->myHalfEdge->myVertex = edge->myHalfEdge->myOpposite->myNext->myNext->myVertex;
        edge->myHalfEdge->myVertex->myHalfEdge = edge->myHalfEdge;

        // update next halfedge
        edge->myHalfEdge->myNext = edge->myHalfEdge->myNext->myNext;
        edge->myHalfEdge->myNext->myNext = edge->myHalfEdge->myOpposite->myNext;

        // update next next halfedge
        edge->myHalfEdge->myNext->myNext->myNext =edge->myHalfEdge;

        //update face
        std::shared_ptr<HalfEdge> h = edge->myHalfEdge;
        Eigen::Vector3i newFace(0.0,0.0,0.0);
        unsigned int index = 0;
        do
        {
            newFace[index++] = h->myVertex->myIndex;
            h = h->myNext;
        }while(h != edge->myHalfEdge);

        //update faces
        edge->myHalfEdge->myFace->myFaceIndexes = newFace;
        edge->myHalfEdge->myFace->myHalfEdge = edge->myHalfEdge;

        h = edge->myHalfEdge;
        do
        {
            h->myFace->myFaceIndexes = newFace;
            h = h->myNext;
        }while(h != edge->myHalfEdge);

        //update oppossite
        edge->myHalfEdge->myOpposite->myEdge->pointA =  a;
        edge->myHalfEdge->myOpposite->myEdge->pointB =  d;

        //update oppossite vertex
        edge->myHalfEdge->myOpposite->myVertex = edge->myHalfEdge->myNext->myVertex;
        edge->myHalfEdge->myOpposite->myVertex->myHalfEdge = edge->myHalfEdge->myOpposite;

        edge->myHalfEdge->myOpposite->myNext = tmp2;
        edge->myHalfEdge->myOpposite->myNext->myNext = tmp;
        edge->myHalfEdge->myOpposite->myNext->myNext->myNext = edge->myHalfEdge->myOpposite;

        //update edge
        edge->myHalfEdge->myOpposite->myEdge->myHalfEdge = edge->myHalfEdge->myOpposite;

        Eigen::Vector3i newOppositeFace(0.0,0.0,0.0);
        index = 0;
        h = edge->myHalfEdge->myOpposite;
        do
        {
            newOppositeFace[index++] = h->myVertex->myIndex;
            h = h->myNext;
        }while(h != edge->myHalfEdge->myOpposite);

        h =  edge->myHalfEdge->myOpposite;
        do
        {
            h->myFace->myFaceIndexes = newFace;
            h = h->myNext;
        }while(h != edge->myHalfEdge->myOpposite);

       edge->myHalfEdge->myOpposite->myFace->myHalfEdge = edge->myHalfEdge->myOpposite;
    }
}

void Mesh::splitEdge(int vertexIndex)
{
    std::shared_ptr<HalfEdge> vHalfEdge = myVertices[vertexIndex]->myHalfEdge;

    std::shared_ptr<Vertex> v1 = vHalfEdge->myVertex;
    std::shared_ptr<Vertex> v2 = vHalfEdge->myNext->myVertex;

    std::shared_ptr<Vertex> v3 = vHalfEdge->myNext->myNext->myVertex;
    std::shared_ptr<Vertex> v4 = vHalfEdge->myOpposite->myNext->myNext->myVertex;

    unsigned int newfaceIndex = myVertices.size();

    Eigen::Vector3f midPoint = (v2->myPosition - v1->myPosition) / 2;
    std::shared_ptr<Vertex> v5(new Vertex(midPoint));
    v5->myIndex = newfaceIndex;
    myVertices.push_back(v5);


    //new edges
   // std::shared_ptr<Edge> newEdge1(new Edge(v5->myIndex,v3->myIndex));
   // std::shared_ptr<Edge> newEdge2(new Edge(v5->myIndex,v2->myIndex));
   // std::shared_ptr<Edge> newEdge3(new Edge(v5->myIndex,v4->myIndex));
   // std::shared_ptr<Edge> newEdge4(new Edge(v5->myIndex,v1->myIndex));



    //faces
    std::shared_ptr<Face> newFace1(new Face());
    newFace1->myFaceIndexes= Eigen::Vector3i(v5->myIndex,v3->myIndex,v1->myIndex);

    std::shared_ptr<Face> newFace2(new Face());
    newFace2->myFaceIndexes = Eigen::Vector3i(v5->myIndex,v2->myIndex,v3->myIndex);

    std::shared_ptr<Face> newFace3(new Face());
    newFace2->myFaceIndexes = Eigen::Vector3i(v5->myIndex,v4->myIndex,v2->myIndex);

    std::shared_ptr<Face> newFace4(new Face());
    newFace2->myFaceIndexes = Eigen::Vector3i(v5->myIndex,v1->myIndex,v4->myIndex);

    std::vector<std::shared_ptr<Face>> newFaces;
    newFaces.push_back(newFace1);
    newFaces.push_back(newFace2);
    newFaces.push_back(newFace3);
    newFaces.push_back(newFace4);

    std::map<std::string ,std::shared_ptr<Edge>> memEdges;


    for(int i = 0 ; i < 4; ++i)
    {
      std::shared_ptr<Face> f = newFaces[i];
      std::shared_ptr<HalfEdge> before ;
      std::shared_ptr<HalfEdge> first ;

      for(int j = 0; j < 3;j++)
      {
        if(j % 2 == 1)
        {
           unsigned int index1 = f->myFaceIndexes[j];
           std::shared_ptr<HalfEdge> h = myVertices[index1]->myHalfEdge;
           h->myFace = f;
        }
        else
        {
            unsigned int index1 = f->myFaceIndexes[j];
            unsigned int index2 = f->myFaceIndexes[(j+1)%3];

            std::string strKey = std::to_string(index1)+","+std::to_string(index2);
            std::string strInvKey = std::to_string(index2)+","+std::to_string(index1);
            std::map<std::string ,std::shared_ptr<Edge>>::iterator it =
                memEdges.find(strKey);
            std::map<std::string ,std::shared_ptr<Edge>>::iterator it2 =
                memEdges.find(strInvKey);


            std::shared_ptr<HalfEdge> halfEdge(new HalfEdge());
            if(i == 0)
            {
              first = halfEdge;
            }

            std::shared_ptr<Edge> newEdge;
            if(it == memEdges.end())
            {
               newEdge = std::shared_ptr<Edge>(new Edge(index1,index2));
               std::shared_ptr<Face> face (new Face());
               newEdge->myHalfEdge = halfEdge;
               halfEdge->myEdge = newEdge;
               halfEdge->myVertex = myVertices[index1];
               face->myHalfEdge = halfEdge;
               face->myFaceIndexes = f->myFaceIndexes;
               halfEdge->myFace = face;
               myVertices[index1]->myHalfEdge = halfEdge;
               memEdges[strKey] = newEdge;

               if(before != nullptr)
               {
                  before->myNext = halfEdge;
               }
            }
            else
            {
                newEdge = memEdges[strKey];
            }


            if(it2 !=  memEdges.end())
            {
               halfEdge->myOpposite = it2->second->myHalfEdge;
               it2->second->myHalfEdge->myOpposite = halfEdge;
               memEdges.erase(it2);
               memEdges.erase(it);
            }

            before = halfEdge;
          }

        }

    }

    /*std::shared_ptr<HalfEdge> currentHalfEdge = v->myHalfEdge;
    currentHalfEdge->myEdge = newEdge1;
    currentHalfEdge->myFace = newFace1;

    std::shared_ptr<HalfEdge> newHalfEdge1(new HalfEdge());
    newHalfEdge1->myEdge = newEdge1;
    newHalfEdge1->myFace = newFace2;

    std::shared_ptr<HalfEdge> newHalfEdge2(new HalfEdge());
    newHalfEdge1->myEdge = newEdge3;
    newHalfEdge1->myFace = newFace3;

    std::shared_ptr<HalfEdge> newHalfEdge3(new HalfEdge());
    newHalfEdge1->myEdge = newEdge2;
    newHalfEdge1->myFace = newFace4;

    std::shared_ptr<HalfEdge> tmp = currentHalfEdge->myNext->myNext;
    currentHalfEdge->myNext = newHalfEdge1;
    newHalfEdge1->myNext = tmp;*/




}

void Mesh::processeVertexEdges(int index)
{
   std::shared_ptr<Vertex> v = myVertices[index];
   std::shared_ptr<HalfEdge> h =  v->myHalfEdge;

   do{
      std::cout << h->myEdge->pointA << " "<< h->myEdge->pointB << std::endl;
      h = h->myOpposite->myNext;
   }while(h != v->myHalfEdge);


}

void Mesh::testFlipEdge(int index)
{
   std::shared_ptr<Vertex> v = myVertices[index];
   v->myHalfEdge->myEdge->flipEdge();
}
