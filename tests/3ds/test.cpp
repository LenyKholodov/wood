// пример чтения 3DS-файла

// ПРИМЕЧАНИЕ: эти #define должны находится ДО #inlcude "mappers.h"
#define SUBPIXEL      // включает поддержку субпиксельной точности
#define SUBTEXEL      // включает поддержку субтексельной точности

// подключаем бибилиотеки
#include <setjmp.h>
#include <stdio.h>
#include <math.h>
#include "3d.h"
#include "video.h"
#include "mappers.h"
#include "scene.h"

// определяем пару нужных типов в зависимости от компилятора
#ifdef __WATCOMC__
typedef unsigned short uint16;
typedef int int32;
#endif

#ifdef __BORLANDC__
typedef unsigned int uint16;
typedef long int32;
#endif

// максимальное количество обрабатываемых объектов
#define MAXOBJS 256

// идентификаторы нужных нам блоков
#define CHUNK_MAIN     0x4D4D
#define CHUNK_OBJMESH  0x3D3D
#define CHUNK_OBJBLOCK 0x4000
#define CHUNK_TRIMESH  0x4100
#define CHUNK_VERTLIST 0x4110
#define CHUNK_FACELIST 0x4120

// переменные загрузчика 3DS-файлов
jmp_buf env;           // для обработки ошибок
FILE *infile;          // файл
int  dpos, dsize;      // для системы ввода-вывода
int  numObjs;          // число объектов в сцене
char tmpstr[256];      // временная строка
char *err;             // место для помещения сообщения об ошибке
int  begVert[MAXOBJS]; // номера начальных вершины объектов в общем массиве

// реализация системы ввода-вывода для нашего случая (файла)
void drestart() {
  if (fseek(infile, 0, SEEK_SET)) longjmp(env, 1);
  dpos = 0;
}

void dinit() {
  if (fseek(infile, 0, SEEK_END)) longjmp(env, 1);
  dsize = ftell(infile);
  if (dsize == -1L) longjmp(env, 1);
  drestart();
}

void dread(void *buf, int len) {
  if (fread(buf, 1, len, infile) != len) longjmp(env, 1);
  dpos += len;
}

char dgetc() { char c; dread(&c, 1); return c; }

void dreadstr(char *buf) {
  char c;
  while ((c = dgetc()) != 0x00) *buf++ = c;
  *buf++ = 0x00;
}

void dskip(int len) {
  if (fseek(infile, len, SEEK_CUR)) longjmp(env, 1);
  dpos += len;
}

int deof() { return (dpos == dsize); }

// чтение и сбор статистики о данных (количество объектов, граней, вершин)
void statChunk(int maxpos) {
  uint16 id, num;
  int32  len;
  char   *c;

  while (dpos < maxpos) { // пока в блоке (или файле) еще остались данные
    dread(&id, 2);        //   читаем id подблока
    dread(&len, 4);       //   читаем его длину
    if (len == 0) return;
    len -= 6;

    switch (id) {         //   обрабатываем нужные нам [под]блоки
      case CHUNK_MAIN:
        statChunk(dpos + len);
        break;
      case CHUNK_OBJMESH:
        statChunk(dpos + len);
        break;
      case CHUNK_OBJBLOCK:
        dreadstr(tmpstr);
        len -= (strlen(tmpstr) + 1);
        statChunk(dpos + len);
        break;
      case CHUNK_TRIMESH:
        numObjs++;
        statChunk(dpos + len);
        break;
      case CHUNK_VERTLIST:
        dread(&num, 2);
        numVerts += num;
        dskip(len - 2);
        break;
      case CHUNK_FACELIST:
        dread(&num, 2);
        numFaces += num;
        dskip(len - 2);
        break;
      default:
        dskip(len);
        break;
    }
  }
}

// чтение и загрузка данных (граней, вершин)
void loadObjects(int maxpos) {
  uint16 id, num, tmp16;
  int32  len;
  int    i, j;

  while (dpos < maxpos) {
    dread(&id, 2);
    dread(&len, 4);
    if (len == 0) return;
    len -= 6;

    switch (id) {
      case CHUNK_MAIN:
      case CHUNK_OBJMESH:
        loadObjects(dpos + len);
        break;
      case CHUNK_OBJBLOCK:
        dreadstr(tmpstr);
        len -= (strlen(tmpstr) + 1);
        loadObjects(dpos + len);
        break;
      case CHUNK_TRIMESH:
        loadObjects(dpos + len);
        break;
      case CHUNK_VERTLIST:
        begVert[numObjs] = numVerts;
        dread(&num, 2);
        for (i = numVerts; i < numVerts + num; i++) {
          dread(&verts[i].x, 4);
          dread(&verts[i].z, 4);
          dread(&verts[i].y, 4);
        }
        numVerts += num;
        break;
      case CHUNK_FACELIST:
        dread(&num, 2);
        j = begVert[numObjs];
        for (i = numFaces; i < numFaces + num; i++) {
          dread(&tmp16, 2); faces[i].v1 = &verts[j + tmp16];
          dread(&tmp16, 2); faces[i].v2 = &verts[j + tmp16];
          dread(&tmp16, 2); faces[i].v3 = &verts[j + tmp16];
          faces[i].texture = exampleTexture;
          dskip(2);
        }
        numFaces += num;
        break;
      default:
        dskip(len);
        break;
    }
  }
}

// собственно загрузчик
int load3DS(char *filename, char *errmsg) {
  int i;

  err = errmsg;
  sprintf(err, "");

  // открываем файл
  if (!(infile = fopen(filename, "rb"))) {
    sprintf(err, "не могу открыть файл %s.", filename);
    return 0;
  }

  // собираем статистику
  if (setjmp(env)) {
    sprintf(err, "statChunk() завершилась с ошибкой. Файл %s, видимо, поврежден.",
      filename);
    fclose(infile);
    return 0;
  }
  dinit();
  numObjs = numVerts = numFaces = 0;
  statChunk(dsize);
  if (numVerts > MAXVERTS) {
    sprintf(err, "слишком много вершин.");
    return 0;
  }
  if (numFaces > MAXFACES) {
    sprintf(err, "слишком много граней.");
    return 0;
  }

  // загружаем сцену
  if (setjmp(env)) {
    if (strlen(err) == 0) sprintf(err, "ошибка при чтении файла.");
    fclose(infile);
    return 0;
  }
  numObjs = numVerts = numFaces = 0;
  drestart();
  loadObjects(dsize);

  fclose(infile);
  return 1;
}

void main() {
  int  i, frame = 0;
  char err[128];

  initZBuffer(); // инициализируем z-буфер
  initVideo();   // инициализируем видеосистему

  // грузим сцену
  if (load3DS("test.3ds", err) == 0) {
    doneVideo();
    printf("Ошибка: %s\n", err);
    exit(0);
  }

  calcNormals(); // посчитаем нормали
  initPalette(); // поставим палитру

  while (!kbhit()) {
    clearZBuffer();     // очистим z-буфер
    clearVideoBuffer(); // очистим видеобуфер

    // поворачиваем все вершины объекта
    for (i = 0; i < numVerts; i++)
      xRotateVertex(&verts[i], frame * PI / 360);

    // рисуем все грани объекта в видеобуфер
    for (i = 0; i < numFaces; i++)
      drawGouraudZBufferedFace(&faces[i]);

    // копируем видеобуфер на экран
    showVideoBuffer();
    frame++;
  }

  // возвращаемся в текстовый видеорежим
  doneVideo();
}
