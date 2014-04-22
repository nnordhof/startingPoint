#ifndef __CHUNKS_H__
#define __CHUNKS_H__

#include "../Modeling/CMeshLoaderSimple.h"
#include <vector>
#include <map>

#define CHUNK_SIZE 8.0f
#define APP 0.001f
#define BOOST 1.0001f

using namespace std;

class ChunkWorld;
class Chunk;
class MicroChunk;
typedef struct chunk_data ChunkData;
typedef struct obj_data ObjData;

struct chunk_data {
   int x;
   int y;
   int z;
};

struct obj_data {
   int obj;
   int tri;
};

// for mapping purposes
bool operator<(const ChunkData a, const ChunkData b) {
   if (a.x != b.x)
      return a.x < b.x;
   if (a.y != b.y)
      return a.y < b.y;
   return a.z < b.z;
}
bool operator==(const ChunkData a, const ChunkData b) {
   return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool operator!=(const ChunkData a, const ChunkData b) {
   return !(a==b);
}
bool operator<(const ObjData a, const ObjData b) {
   if (a.obj != b.obj)
      return a.obj < b.obj;
   return a.tri < b.tri;
}
bool operator==(const ObjData a, const ObjData b) {
   return a.obj == b.obj && a.tri == b.tri;
}
bool operator!=(const ObjData a, const ObjData b) {
   return !(a==b);
}

SVector3 nextChunk(SVector3 pos, SVector3 ray, float scale);

class MicroChunk {
   public:
      map<ObjData, STriangle> objects;
      SVector3 minBound, maxBound, index;
      int valid;

      MicroChunk() :
         valid(0)
      {
      }

      MicroChunk(float x, float y, float z) :
         valid(1),
         index((int)x,(int)y,(int)z),
         minBound(x/CHUNK_SIZE,y/CHUNK_SIZE,z/CHUNK_SIZE),
         maxBound((x+1.0)/CHUNK_SIZE, (y+1.0)/CHUNK_SIZE,
                  (z+1.0)/CHUNK_SIZE)
      {
      }

      int isValid() {
         return valid;
      }
};

class Chunk {
   public:
      int empty;
      SVector3 minBound, maxBound, index;

      Chunk() :
         index(0,0,0),
         minBound(0,0,0),
         maxBound(0,0,0),
         empty(1)
      {
      }

      Chunk(float x, float y, float z) :
         index(x, y, z),
         minBound(x, y, z),
         maxBound(x+1.0, y+1.0, z+1.0),
         empty(0)
      {
      }

      int isValid() {
         return !empty;
      }

};

class ChunkWorld {
   public:
      map<ChunkData, Chunk> chunkMap;
      map<ChunkData, MicroChunk> uChunkMap;
      Chunk invalidC;
      MicroChunk invalidM;
      SVector3 scale;
      vector<CMesh> objects;
      vector<LightSource> lights;
      vector<mat_t> materials;
      vector<int> matIdcs; //TODO
      int objCount;

      ChunkWorld() :
         objCount(0)
      {
      }

      ChunkWorld(const int & width, const int & height, const int & depth) :
         scale(width, height, depth),
         objCount(0)
      {
      }
      
      void print(); 
      Chunk & findChunk(float x, float y, float z);      
      Chunk & addChunk(float x, float y, float z);
      MicroChunk * findMicroChunk(float x, float y, float z); 
      MicroChunk * addMicroChunk(float x, float y, float z);
      void traceLine(SVector3 start, SVector3 end, ObjData dat);      
      void traceTriangle(SVector3 v1, SVector3 v2, SVector3 v3, ObjData check);
      void populate(CMesh *mesh, int objIndex);
      void repopulate(CMesh *mesh, int objIndex);
      void depopulate(int objIndex);
      int addMaterial(mat_t mat);      
      int addLight(LightSource light);
};


#endif
