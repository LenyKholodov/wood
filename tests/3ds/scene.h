// определение, поддержка и генерация тестовой 3D сцены (тора)

#ifndef __EXAMPLE_SCENE_H
#define __EXAMPLE_SCENE_H

#include <math.h>     // для sin()/cos()
#include "3d.h"
#include "video.h"

#define PI  3.1415926 // для перевода градусов в радианы
#define MAXVERTS  300 // максимальное количество вершин в сцене
#define MAXFACES  300 // максимальное количество граней в сцене
#define SEGS1      10 // количество сегментов в торе
#define SEGS2      10 // количество подсегментов в каждом сегменте тора
#define R1         60 // радиус тора
#define R2         15 // толщина тора

int    numVerts, numFaces; // число (реальное) вершин и граней в сцене
vertex verts[MAXVERTS];    // вершины
face   faces[MAXFACES];    // грани

void makeTorus() {
  int i, j, n;
  float a1, a2;

  numVerts = numFaces = n = 0;
  for (i = 0; i < SEGS1; i++) {
    for (j = 0; j < SEGS2; j++) {
      a1 = i * (2 * PI / SEGS1);
      a2 = j * (2 * PI / SEGS2);
      verts[numVerts].x = (R1 + R2 * cos(a2)) * cos(a1);
      verts[numVerts].y = (R1 + R2 * cos(a2)) * sin(a1);
      verts[numVerts].z = R2 * sin(a2);
      verts[numVerts].u = 1024 * i / SEGS1;
      verts[numVerts].v = 256 * j / SEGS2;
      numVerts++;
    }
  }
  for (i = 0; i < SEGS1; i++) {
    for (j = 0; j < SEGS2; j++) {
      faces[numFaces].v1 = &verts[n + j];
      faces[numFaces].v2 = &verts[(n + ((j + 1) % SEGS2)) % numVerts];
      faces[numFaces].v3 = &verts[(n + ((j + 1) % SEGS2) + SEGS2) % numVerts];
      faces[numFaces].texture = exampleTexture;
      numFaces++;
      faces[numFaces].v1 = &verts[(n + ((j + 1) % SEGS2) + SEGS2) % numVerts];
      faces[numFaces].v2 = &verts[(n + j + SEGS2) % numVerts];
      faces[numFaces].v3 = &verts[n + j];
      faces[numFaces].texture = exampleTexture;
      numFaces++;
    }
    n += SEGS2;
  }
}

void calcNormals() {
  float ax, ay, az, bx, by, bz, nx, ny, nz, l;
  int i;

  for (i = 0; i < numFaces; i++) {
    ax = faces[i].v3->x - faces[i].v1->x;
    ay = faces[i].v3->y - faces[i].v1->y;
    az = faces[i].v3->z - faces[i].v1->z;
    bx = faces[i].v2->x - faces[i].v1->x;
    by = faces[i].v2->y - faces[i].v1->y;
    bz = faces[i].v2->z - faces[i].v1->z;
    nx = faces[i].nx = ay * bz - az * by;
    ny = faces[i].ny = az * bx - ax * bz;
    nz = faces[i].nz = ax * by - ay * bx;
    l = sqrt(nx * nx + ny * ny + nz * nz);
    faces[i].nx /= l;
    faces[i].ny /= l;
    faces[i].nz /= l;
  }
  for (i = 0; i < numVerts; i++) {
    verts[i].nx = 0;
    verts[i].ny = 0;
    verts[i].nz = 0;
  }
  for (i = 0; i < numFaces; i++) {
    faces[i].v1->nx += faces[i].nx;
    faces[i].v1->ny += faces[i].ny;
    faces[i].v1->nz += faces[i].nz;
    faces[i].v2->nx += faces[i].nx;
    faces[i].v2->ny += faces[i].ny;
    faces[i].v2->nz += faces[i].nz;
    faces[i].v3->nx += faces[i].nx;
    faces[i].v3->ny += faces[i].ny;
    faces[i].v3->nz += faces[i].nz;
  }
  for (i = 0; i < numVerts; i++) {
    l = sqrt(verts[i].nx * verts[i].nx + verts[i].ny * verts[i].ny +
             verts[i].nz * verts[i].nz);
    verts[i].nx /= l;
    verts[i].ny /= l;
    verts[i].nz /= l;
  }
}

#endif

