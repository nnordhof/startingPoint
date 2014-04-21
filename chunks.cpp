
#include "chunks.h"

bool containedIn(SVector3 pt, SVector3 min, SVector3 max) {
   return pt.X >= min.X && pt.Y >= min.Y && pt.Z >= min.Z
          && pt.X <= max.X && pt.Y <= max.Y && pt.Z <= max.Z;
}

SVector3 checkCollision(SVector3 ray, SVector3 eye, CMesh *mesh, int tri) {
   float beta, gamma, trace, meta, eihf, gfdi, dheg, akjb, jcal, blkc;
   SVector3 vert1, vert2, vert3, abc, def, ghi, jkl;

   vert1 = mesh->Vertices[mesh->Triangles[tri].vIdx1].pos;
   vert2 = mesh->Vertices[mesh->Triangles[tri].vIdx2].pos;
   vert3 = mesh->Vertices[mesh->Triangles[tri].vIdx3].pos;

   abc = vert1 - vert2;
   def = vert1 - vert3;
   ghi = ray;
   jkl = vert1 - eye;

   eihf = def.Y*ghi.Z-ghi.Y*def.Z;
   gfdi = ghi.X*def.Z-def.X*ghi.Z;
   dheg = def.X*ghi.Y-def.Y*ghi.X;
   akjb = abc.X*jkl.Y-jkl.X*abc.Y;
   jcal = jkl.X*abc.Z-abc.X*jkl.Z;
   blkc = abc.Y*jkl.Z-jkl.Y*abc.Z;

   meta = abc.X*eihf + abc.Y*gfdi + abc.Z*dheg;
   //cout << "checking collision";
   trace = (def.Z*akjb + def.Y*jcal+def.X*blkc) / meta;
   if (trace > 0.0 || fabsf(trace) > ray.length())
      return SVector3(2.0*COLL_LIMIT);
   //cout << ".";
   gamma = (ghi.Z*akjb + ghi.Y*jcal+ghi.X*blkc) / meta;
   if (gamma <= 0.0 || gamma >= 1.0)
      return SVector3(2.0*COLL_LIMIT);
   //cout << ".";
   beta = (jkl.X*eihf + jkl.Y*gfdi+jkl.Z*dheg) / meta;
   if (beta <= 0.0 || beta >= 1.0 - gamma)
      return SVector3(2.0*COLL_LIMIT);
   //cout << ".";

   return SVector3(fabsf(trace), beta, gamma);
}

SVector3 nextChunk(SVector3 pos, SVector3 ray, float scale) {
   float x,y,z, xdis,ydis,zdis, temp;
   SVector3 rayNorm = ray / ray.length();

   if (ray.X < 0.0) {
      temp = -APP * APP;
      x = floor(pos.X*scale) - pos.X * scale;
   }
   else {
      temp = APP * APP;
      x = ceil(pos.X*scale) - pos.X * scale;
   }
   xdis = (x != 0.0 ? x : temp) / (rayNorm.X != 0.0 ? rayNorm.X : temp);
   if (ray.Y < 0.0) {
      temp = -APP * APP;
      y = floor(pos.Y*scale) - pos.Y * scale;
   }
   else {
      temp = APP * APP;
      y = ceil(pos.Y*scale) - pos.Y * scale;
   }
   ydis = (y != 0.0 ? y : temp) / (rayNorm.Y != 0.0 ? rayNorm.Y : temp);
   if (ray.Z < 0.0) {
      temp = -APP * APP;
      z = floor(pos.Z*scale) - pos.Z * scale;
   }
   else {
      temp = APP * APP;
      z = ceil(pos.Z*scale) - pos.Z * scale;
   }
   zdis = (z != 0.0 ? z : temp) / (rayNorm.Z != 0.0 ? rayNorm.Z : temp);
   if (xdis < APP) xdis = APP;
   if (ydis < APP) ydis = APP;
   if (zdis < APP) zdis = APP;
   if (xdis < ydis) {
      if (xdis < zdis) {
         return pos + rayNorm * xdis * BOOST / scale;
      }
      else {
         return pos + rayNorm * zdis * BOOST / scale;
      }
   }
   else {
      if (ydis < zdis) {
         return pos + rayNorm * ydis * BOOST / scale;
      }
      else { //y = z (meet at edge or corner) or y > z
         return pos + rayNorm * zdis * BOOST / scale;
      }
   }
}

void ChunkWorld::print() {
   ChunkData idx;

   for (int i = -scale.Z; i < scale.Z; i++) {
      idx.z = i;
      for (int j = scale.Y - 1; j >= -scale.Y; j--) {
         idx.y = j;
         for (int k = -scale.X; k < scale.X; k++) {
            idx.x = k;
            if (chunkMap.count(idx)) std::cout << "@ ";
            else std::cout << ". ";
         }
         std::cout << "\n";
      }
      std::cout << "\n";
   }
}

Chunk & ChunkWorld::findChunk(float x, float y, float z) {
   ChunkData idx;
   idx.x = (int)floor(x);
   idx.y = (int)floor(y);
   idx.z = (int)floor(z);
   if (chunkMap.count(idx))
      return chunkMap[idx];
   return invalidC;
}

Chunk & ChunkWorld::addChunk(float x, float y, float z) {
   ChunkData idx;
   Chunk temp(floor(x), floor(y), floor(z));
   idx.x = (int)floor(x);
   idx.y = (int)floor(y);
   idx.z = (int)floor(z);
   chunkMap.insert(std::pair<ChunkData,Chunk>(idx,temp));
   return findChunk(x,y,z);
}

MicroChunk * ChunkWorld::findMicroChunk(float x, float y, float z) {
   ChunkData idx;
   idx.x = (int)floor(x*CHUNK_SIZE);
   idx.y = (int)floor(y*CHUNK_SIZE);
   idx.z = (int)floor(z*CHUNK_SIZE);
   if (uChunkMap.count(idx))
      return &uChunkMap[idx];
   return &invalidM;
}

MicroChunk * ChunkWorld::addMicroChunk(float x, float y, float z) {
   ChunkData idx;
   MicroChunk temp(floor(x*CHUNK_SIZE), floor(y*CHUNK_SIZE), floor(z*CHUNK_SIZE));
   idx.x = (int)floor(x*CHUNK_SIZE);
   idx.y = (int)floor(y*CHUNK_SIZE);
   idx.z = (int)floor(z*CHUNK_SIZE);
   uChunkMap.insert(std::pair<ChunkData,MicroChunk>(idx,temp));
   if (!findChunk(x,y,z).isValid()) {
      addChunk(x,y,z);
   }
   return findMicroChunk(x,y,z);
}

void ChunkWorld::traceLine(SVector3 start, SVector3 end, ObjData dat) {
   SVector3 line = end - start;
   SVector3 increment = (line / line.length()) / (CHUNK_SIZE * CHUNK_SIZE);
   SVector3 iter = start;
   SVector3 last = end - start;
   MicroChunk *step;
   //while iterator has not reached the end of the line
   //printf("%f, %f, %f, %f\n", iter.X, iter.Y, iter.Z, line.dotProduct(end-iter));
   while (line.dotProduct(end - iter) >= 0.0) {
      step = findMicroChunk(iter.X, iter.Y, iter.Z);
      //std::cout << "in loop";
      //check to see if still in same chunk (unlikely)
      if (!step->isValid()) {
         //if not, add chunk to map
         step = addMicroChunk(iter.X, iter.Y, iter.Z);
      }
      if (!step->objects.count(dat)) {
         step->objects.insert(std::pair<ObjData,STriangle>(dat, STriangle()));
         //std::cout << "found a chunk" << step->objects.size() << "\n";
      }
      //increment by normalized line in CHUNK_SIZE units
      //iter += increment;
      iter = nextChunk(iter, line, CHUNK_SIZE);
   //printf("%f, %f, %f, %f\n", iter.X, iter.Y, iter.Z, line.dotProduct(end-iter));
   }
   //std::cout << "traced a line\n";
}

void ChunkWorld::traceTriangle(SVector3 v1, SVector3 v2, SVector3 v3, ObjData check) {
   SVector3 line = v2 - v1;
   SVector3 increment = (line / line.length()) / CHUNK_SIZE;
   SVector3 iter = v1;
   SVector3 last = v2 - v1;

   //printf("%f, %f, %f, %f\n", line.X, line.Y, line.Z, line.dotProduct(v2-iter));
   while (line.dotProduct(v2 - iter) > 0.0) {
      traceLine(iter, v3, check);
      iter = nextChunk(iter, line, CHUNK_SIZE);
   //printf("%f, %f, %f, %f\n", line.X, line.Y, line.Z, line.dotProduct(v2-iter));
   }
   //std::cout << "traced a triangle\n";
}

void ChunkWorld::populate(CMesh *mesh, int objIndex) {
   SVector3 last, vert1, vert2, vert3;
   std::vector<MicroChunk> triBound;
   ObjData dat;
   ChunkData idx;
   MicroChunk uChunk;

   objCount = objects.size();
   dat.obj = objIndex;
   for (int i = 0; i < mesh->Triangles.size(); i++) {
      dat.tri = i;
      vert1 = mesh->Vertices[mesh->Triangles[i].vIdx1].pos;
      vert2 = mesh->Vertices[mesh->Triangles[i].vIdx2].pos;
      vert3 = mesh->Vertices[mesh->Triangles[i].vIdx3].pos;
      traceTriangle(vert1, vert2, vert3, dat);
      //printf("triangle size %f %f %f\n", vert1.X, vert1.Y, vert1.Z);
      //printf("triangle size %f %f %f\n", vert2.X, vert2.Y, vert2.Z);
      //printf("triangle size %f %f %f\n\n", vert3.X, vert3.Y, vert3.Z);
   }
   objects.push_back(*mesh);
}

int ChunkWorld::addMaterial(mat_t mat) {
   mat.objId = materials.size() - 1;
   materials.push_back(mat);
   return mat.objId;
}

int ChunkWorld::addLight(LightSource light) {
   lights.push_back(light);
   return lights.size() - 1;
}
