// различные мэпперы

#ifndef __EXAMPLE_MAPPERS_H
#define __EXAMPLE_MAPPERS_H

#include <stdlib.h> // для memset(), malloc()
#include "3d.h"     // для projectVertex()
#include "video.h"  // для videoBuffer, XSIZE, YSIZE, DIST

// z-буфер (точнее говоря, 1/z-буфер, или w-буфер)
float *zbuffer[YSIZE];

// инициализация z-буфера
void initZBuffer() {
  int i;

  for (i = 0; i < YSIZE; i++)
		zbuffer[i] = (float*)malloc(XSIZE * sizeof(float));
}

// очистка z-буфера
void clearZBuffer() {
  int i;

  for (i = 0; i < YSIZE; i++)
    memset(zbuffer[i], 0, XSIZE * sizeof(float));
}

// Гуро, z-буфер, опциональная субпиксельная/субтексельная точность
void drawGouraudZBufferedFace(face *f) {
  vertex *a, *b, *c, *tmp_vertex;
  int current_sx, current_sy;
  float tmp, k, x_start, x_end, c_start, c_end, dc_start, dc_end;
  float dx_start, dx_end, dz1_start, dz1_end;
  float z1_start, z1_end;
  float x, cc, z1, dc, dz1;
  int length;
  char *dest;

  a = f->v1;
  b = f->v2;
  c = f->v3;

  // спроецируем вершины грани
  projectVertex(a);
  projectVertex(b);
  projectVertex(c);

  // отсортируем вершины грани по sy
  if (a->sy > b->sy) { tmp_vertex = a; a = b; b = tmp_vertex; }
  if (a->sy > c->sy) { tmp_vertex = a; a = c; c = tmp_vertex; }
  if (b->sy > c->sy) { tmp_vertex = b; b = c; c = tmp_vertex; }

  // грань нулевой высоты рисовать не будем
  if (floor(c->sy) <= ceil(a->sy)) return;

  // считаем освещенность в вершинах
  a->cc = 255 * ((a->rnz < 0) ? (-a->rnz) : 0);
  b->cc = 255 * ((b->rnz < 0) ? (-b->rnz) : 0);
  c->cc = 255 * ((c->rnz < 0) ? (-c->rnz) : 0);

  // посчитаем du/dsx, dv/dsx, d(1/z)/dsx
  // считаем по самой длинной линии (т.е. проходящей через вершину B)
  a->z1 = 1 / (a->rz + DIST);
  b->z1 = 1 / (b->rz + DIST);
  c->z1 = 1 / (c->rz + DIST);
  k = (b->sy - a->sy) / (c->sy - a->sy);
  x_start = a->sx + (c->sx - a->sx) * k;
  c_start = a->cc + (c->cc - a->cc) * k;
  z1_start = a->z1 + (c->z1 - a->z1) * k;
  x_end = b->sx;
  c_end = b->cc;
  z1_end = b->z1;
  dc = (c_start - c_end) / (x_start - x_end);
  dz1 = (z1_start - z1_end) / (x_start - x_end);

  x_start = a->sx;
  c_start = a->cc;
  z1_start = a->z1;
  dx_start = (c->sx - a->sx) / (c->sy - a->sy);
  dc_start = (c->cc - a->cc) / (c->sy - a->sy);
  dz1_start = (c->z1 - a->z1) / (c->sy - a->sy);
#ifdef SUBPIXEL
  tmp = ceil(a->sy) - a->sy;
  x_start += dx_start * tmp;
  c_start += dc_start * tmp;
  z1_start += dz1_start * tmp;
#endif

  if (ceil(b->sy) > ceil(a->sy)) {
    tmp = ceil(a->sy) - a->sy;
    x_end = a->sx;
    c_end = a->cc;
    z1_end = a->z1;
    dx_end = (b->sx - a->sx) / (b->sy - a->sy);
    dc_end = (b->cc - a->cc) / (b->sy - a->sy);
    dz1_end = (b->z1 - a->z1) / (b->sy - a->sy);
  } else {
    tmp = ceil(b->sy) - b->sy;
    x_end = b->sx;
    c_end = b->cc;
    z1_end = b->z1;
    dx_end = (c->sx - b->sx) / (c->sy - b->sy);
    dc_end = (c->cc - b->cc) / (c->sy - b->sy);
    dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
  }
#ifdef SUBPIXEL
  x_end += dx_end * tmp;
  c_end += dc_end * tmp;
  z1_end += dz1_end * tmp;
#endif

  // построчная отрисовка грани
  for (current_sy = ceil(a->sy); current_sy < ceil(c->sy); current_sy++) {
    if (current_sy == ceil(b->sy)) {
      x_end = b->sx;
      c_end = b->cc;
      z1_end = b->z1;
      dx_end = (c->sx - b->sx) / (c->sy - b->sy);
      dc_end = (c->cc - b->cc) / (c->sy - b->sy);
      dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
#ifdef SUBPIXEL
      tmp = ceil(b->sy) - b->sy;
      x_end += dx_end * tmp;
      c_end += dc_end * tmp;
      z1_end += dz1_end * tmp;
#endif
    }

    // x_start должен находиться левее x_end
    if (x_start > x_end) {
      x = x_end;
      cc = c_end;
      z1 = z1_end;
      length = ceil(x_start) - ceil(x_end);
    } else {
      x = x_start;
      cc = c_start;
      z1 = z1_start;
      length = ceil(x_end) - ceil(x_start);
    }

    // считаем адрес начала строки в видеопамяти
    dest = videoBuffer;
    dest += current_sy * XSIZE + (int)ceil(x);

    // текстурируем строку
    current_sx = (int)ceil(x);

    if (length) {
#ifdef SUBTEXEL
      tmp = ceil(x) - x;
      cc += dc * tmp;
#endif
      while (length--) {
        // используем z-буфер для определения видимости текущей точки
        if (zbuffer[current_sy][current_sx] <= z1) {
          *dest = cc;
          zbuffer[current_sy][current_sx] = z1;
        }
        cc += dc;
        z1 += dz1;
        dest++;
        current_sx++;
      }
    }

    // сдвигаем начальные и конечные значения x/u/v/(1/z)
    x_start += dx_start;
    c_start += dc_start;
    z1_start += dz1_start;
    x_end += dx_end;
    c_end += dc_end;
    z1_end += dz1_end;
  }
}

// аффинное текстурирование, z-буфер, опциональная субпиксельная и
// субтексельная точность
void drawAffineTexturedZBufferedFace(face *f) {
  vertex *a, *b, *c, *tmp_vertex;
  int current_sx, current_sy, length;
  float tmp, k;
  float x_start, x_end, u_start, v_start, u_end, v_end;
  float dx_start, dx_end, du_start, dv_start, dz1_start;
  float du_end, dv_end, dz1_end, z1_start, z1_end, x, u, v, z1, du, dv, dz1;
  char *dest;

  a = f->v1;
  b = f->v2;
  c = f->v3;

  // спроецируем вершины грани
  projectVertex(a);
  projectVertex(b);
  projectVertex(c);

  // отсортируем вершины грани по sy
  if (a->sy > b->sy) { tmp_vertex = a; a = b; b = tmp_vertex; }
  if (a->sy > c->sy) { tmp_vertex = a; a = c; c = tmp_vertex; }
  if (b->sy > c->sy) { tmp_vertex = b; b = c; c = tmp_vertex; }

  // грань нулевой высоты рисовать не будем
  if (floor(c->sy) <= ceil(a->sy)) return;

  // посчитаем du/dsx, dv/dsx, d(1/z)/dsx
  // считаем по самой длинной линии (т.е. проходящей через вершину B)
  a->z1 = 1 / (a->rz + DIST);
  b->z1 = 1 / (b->rz + DIST);
  c->z1 = 1 / (c->rz + DIST);
  k = (b->sy - a->sy) / (c->sy - a->sy);
  x_start = a->sx + (c->sx - a->sx) * k;
  u_start = a->u + (c->u - a->u) * k;
  v_start = a->v + (c->v - a->v) * k;
  z1_start = a->z1 + (c->z1 - a->z1) * k;
  x_end = b->sx;
  u_end = b->u;
  v_end = b->v;
  z1_end = b->z1;
  du = (u_start - u_end) / (x_start - x_end);
  dv = (v_start - v_end) / (x_start - x_end);
  dz1 = (z1_start - z1_end) / (x_start - x_end);

  x_start = a->sx;
  u_start = a->u;
  v_start = a->v;
  z1_start = a->z1;
  dx_start = (c->sx - a->sx) / (c->sy - a->sy);
  du_start = (c->u - a->u) / (c->sy - a->sy);
  dv_start = (c->v - a->v) / (c->sy - a->sy);
  dz1_start = (c->z1 - a->z1) / (c->sy - a->sy);
#ifdef SUBPIXEL
  tmp = ceil(a->sy) - a->sy;
  x_start += dx_start * tmp;
  u_start += du_start * tmp;
  v_start += dv_start * tmp;
  z1_start += dz1_start * tmp;
#endif

  if (ceil(b->sy) > ceil(a->sy)) {
    tmp = ceil(a->sy) - a->sy;
    x_end = a->sx;
    u_end = a->u;
    v_end = a->v;
    z1_end = a->z1;
    dx_end = (b->sx - a->sx) / (b->sy - a->sy);
    du_end = (b->u - a->u) / (b->sy - a->sy);
    dv_end = (b->v - a->v) / (b->sy - a->sy);
    dz1_end = (b->z1 - a->z1) / (b->sy - a->sy);
  } else {
    tmp = ceil(b->sy) - b->sy;
    x_end = b->sx;
    u_end = b->u;
    v_end = b->v;
    z1_end = b->z1;
    dx_end = (c->sx - b->sx) / (c->sy - b->sy);
    du_end = (c->u - b->u) / (c->sy - b->sy);
    dv_end = (c->v - b->v) / (c->sy - b->sy);
    dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
  }
#ifdef SUBPIXEL
  x_end += dx_end * tmp;
  u_end += du_end * tmp;
  v_end += dv_end * tmp;
  z1_end += dz1_end * tmp;
#endif

  // построчная отрисовка грани
  for (current_sy = ceil(a->sy); current_sy < ceil(c->sy); current_sy++) {
    if (current_sy == ceil(b->sy)) {
      x_end = b->sx;
      u_end = b->u;
      v_end = b->v;
      z1_end = b->z1;
      dx_end = (c->sx - b->sx) / (c->sy - b->sy);
      du_end = (c->u - b->u) / (c->sy - b->sy);
      dv_end = (c->v - b->v) / (c->sy - b->sy);
      dz1_end = (c->z1 - b->z1) / (c->sy - b->sy);
#ifdef SUBPIXEL
      tmp = ceil(b->sy) - b->sy;
      x_end += dx_end * tmp;
      u_end += du_end * tmp;
      v_end += dv_end * tmp;
      z1_end += dz1_end * tmp;
#endif
    }

    // x_start должен находиться левее x_end
    if (x_start > x_end) {
      x = x_end;
      u = u_end;
      v = v_end;
      z1 = z1_end;
      length = ceil(x_start) - ceil(x_end);
    } else {
      x = x_start;
      u = u_start;
      v = v_start;
      z1 = z1_start;
      length = ceil(x_end) - ceil(x_start);
    }

    // считаем адрес начала строки в видеопамяти
    dest = videoBuffer;
    dest += current_sy * XSIZE + (int)ceil(x);

    // текстурируем строку
    current_sx = (int)ceil(x);

    if (length) {
#ifdef SUBTEXEL
      tmp = ceil(x) - x;
      u += du * tmp;
      v += dv * tmp;
#endif
      while (length--) {
        // используем z-буфер для определения видимости текущей точки
        if (zbuffer[current_sy][current_sx] <= z1) {
          *dest = f->texture[(((int)v) * 256 + (int)u) & 0xFFFF];
          zbuffer[current_sy][current_sx] = z1;
        }
        u += du;
        v += dv;
        z1 += dz1;
        dest++;
        current_sx++;
      }
    }

    // сдвигаем начальные и конечные значения x/u/v/(1/z)
    x_start += dx_start;
    u_start += du_start;
    v_start += dv_start;
    z1_start += dz1_start;
    x_end += dx_end;
    u_end += du_end;
    v_end += dv_end;
    z1_end += dz1_end;
  }
}

#endif

