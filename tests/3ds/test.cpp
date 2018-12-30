// ������ ������ 3DS-�����

// ����������: ��� #define ������ ��������� �� #inlcude "mappers.h"
#define SUBPIXEL      // �������� ��������� ������������� ��������
#define SUBTEXEL      // �������� ��������� ������������� ��������

// ���������� �����������
#include <setjmp.h>
#include <stdio.h>
#include <math.h>
#include "3d.h"
#include "video.h"
#include "mappers.h"
#include "scene.h"

// ���������� ���� ������ ����� � ����������� �� �����������
#ifdef __WATCOMC__
typedef unsigned short uint16;
typedef int int32;
#endif

#ifdef __BORLANDC__
typedef unsigned int uint16;
typedef long int32;
#endif

// ������������ ���������� �������������� ��������
#define MAXOBJS 256

// �������������� ������ ��� ������
#define CHUNK_MAIN     0x4D4D
#define CHUNK_OBJMESH  0x3D3D
#define CHUNK_OBJBLOCK 0x4000
#define CHUNK_TRIMESH  0x4100
#define CHUNK_VERTLIST 0x4110
#define CHUNK_FACELIST 0x4120

// ���������� ���������� 3DS-������
jmp_buf env;           // ��� ��������� ������
FILE *infile;          // ����
int  dpos, dsize;      // ��� ������� �����-������
int  numObjs;          // ����� �������� � �����
char tmpstr[256];      // ��������� ������
char *err;             // ����� ��� ��������� ��������� �� ������
int  begVert[MAXOBJS]; // ������ ��������� ������� �������� � ����� �������

// ���������� ������� �����-������ ��� ������ ������ (�����)
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

// ������ � ���� ���������� � ������ (���������� ��������, ������, ������)
void statChunk(int maxpos) {
  uint16 id, num;
  int32  len;
  char   *c;

  while (dpos < maxpos) { // ���� � ����� (��� �����) ��� �������� ������
    dread(&id, 2);        //   ������ id ��������
    dread(&len, 4);       //   ������ ��� �����
    if (len == 0) return;
    len -= 6;

    switch (id) {         //   ������������ ������ ��� [���]�����
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

// ������ � �������� ������ (������, ������)
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

// ���������� ���������
int load3DS(char *filename, char *errmsg) {
  int i;

  err = errmsg;
  sprintf(err, "");

  // ��������� ����
  if (!(infile = fopen(filename, "rb"))) {
    sprintf(err, "�� ���� ������� ���� %s.", filename);
    return 0;
  }

  // �������� ����������
  if (setjmp(env)) {
    sprintf(err, "statChunk() ����������� � �������. ���� %s, ������, ���������.",
      filename);
    fclose(infile);
    return 0;
  }
  dinit();
  numObjs = numVerts = numFaces = 0;
  statChunk(dsize);
  if (numVerts > MAXVERTS) {
    sprintf(err, "������� ����� ������.");
    return 0;
  }
  if (numFaces > MAXFACES) {
    sprintf(err, "������� ����� ������.");
    return 0;
  }

  // ��������� �����
  if (setjmp(env)) {
    if (strlen(err) == 0) sprintf(err, "������ ��� ������ �����.");
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

  initZBuffer(); // �������������� z-�����
  initVideo();   // �������������� ������������

  // ������ �����
  if (load3DS("test.3ds", err) == 0) {
    doneVideo();
    printf("������: %s\n", err);
    exit(0);
  }

  calcNormals(); // ��������� �������
  initPalette(); // �������� �������

  while (!kbhit()) {
    clearZBuffer();     // ������� z-�����
    clearVideoBuffer(); // ������� ����������

    // ������������ ��� ������� �������
    for (i = 0; i < numVerts; i++)
      xRotateVertex(&verts[i], frame * PI / 360);

    // ������ ��� ����� ������� � ����������
    for (i = 0; i < numFaces; i++)
      drawGouraudZBufferedFace(&faces[i]);

    // �������� ���������� �� �����
    showVideoBuffer();
    frame++;
  }

  // ������������ � ��������� ����������
  doneVideo();
}
