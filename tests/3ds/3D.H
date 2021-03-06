// ����������� 3D-�������� � ������� �������������

#ifndef __EXAMPLE_3D_H
#define __EXAMPLE_3D_H

// ������ ������ � ���������� �� ������ ���������� � video.h
#include "video.h"
#include <math.h>

// ��������� "�������"
typedef struct {
  float x, y, z;       // ���������� �������
  float rx, ry, rz;    // ��������������� (eg, ����������) ���������� �������
  float sx, sy;        // �������� (���������������) ���������� �������
  float u, v;          // ��������������� �� ���������� ��������
  float uz, vz, z1;    // u/z, v/z, 1/z (��� ������������-������������������)
  float nx, ny, nz;    // ���������� ������� ������� � �������
  float rnx, rny, rnz; // ��������������� (eg, ����������) ���������� �������
  float cc;            // ������������ ������� (��� ��������� �� ����)
} vertex;

// ��������� "�����"
typedef struct {
  vertex *v1, *v2, *v3; // ��������� �� ������� �����
  char *texture;        // ��������� �� ��������
  float nx, ny, nz;     // ���������� ������� ������� � �����
} face;

// ������� ������������� �������
void projectVertex(vertex *v) {
  v->sx = XSIZE/2 + v->rx * DIST / (v->rz + DIST);
  v->sy = YSIZE/2 - v->ry * DIST / (v->rz + DIST);
}

// ������� ������� ������������ ��� Ox �� ���� angle
void xRotateVertex(vertex *v, float angle) {
  register float mysin = sin(angle);
  register float mycos = cos(angle);

  // ������������ ���� �����
  v->rx = v->x;
  v->rz = v->z * mycos - v->y * mysin;
  v->ry = v->z * mysin + v->y * mycos;

  // ������������ ������� � ������� � ���� �����
  v->rnx = v->nx;
  v->rnz = v->nz * mycos - v->ny * mysin;
  v->rny = v->nz * mysin + v->ny * mycos;
}

#endif

