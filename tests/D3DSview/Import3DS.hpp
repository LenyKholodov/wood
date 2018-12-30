
// D3DSView

// Written by William Chin
// Developed in MSDEV C++ v6.0 using the DirectX 7.0 SDK
// Date : 16th May 2000
// File : Import3DS.hpp

// Website: http://members.aol.com/billybop7
// Email:  billybop7@aol.com 

#ifndef IMPORT3DS_HPP
#define IMPORT3DS_HPP

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>



#define  MAX_NUM_3DS_TRIANGLES	32000
#define  MAX_NUM_3DS_VERTICES	32000
#define  MAX_NUM_3DS_FACES		32000
#define  MAX_NAME_LENGTH		256
#define  MAX_NUM_3DS_TEXTURES	100
#define  MAX_NUM_3DS_FRAMES		50
#define  MAX_NUM_3DS_OBJECTS    32000


#define MAIN3DS							0x4D4D	
#define M3DS_VERSION					0x0002
#define MESH_VERSION					0x3D3E

#define	EDIT3DS							0x3D3D  
#define	  NAMED_OBJECT					0x4000	
#define		TRIANGLE_MESH				0x4100	
#define		  TRIANGLE_VERTEXLIST		0x4110	
#define		  TRIANGLE_VERTEXOPTIONS	0x4111	
#define		  TRIANGLE_MAPPINGCOORS		0x4140
#define		  TRIANGLE_MAPPINGSTANDARD	0x4170 
#define		  TRIANGLE_FACELIST			0x4120
#define			TRIANGLE_SMOOTH         0x4150   
#define			TRIANGLE_MATERIAL       0x4130  
#define		  TRI_LOCAL					0x4160	
#define		  TRI_VISIBLE				0x4165
  
#define  	INT_PERCENTAGE				0x0030 
#define  	MASTER_SCALE				0x0100 

#define	  EDIT_MATERIAL					0xAFFF
#define		MAT_NAME01					0xA000 

#define		  TEXTURE_MAP				0xA200 
#define			MAPPING_NAME			0xA300 

 
#define         MATERIAL_AMBIENT		0xA010
#define			MATERIAL_DIFFUSE		0xA020
#define			MATERIAL_SPECULAR		0xA030
#define			MATERIAL_SHININESS		0xA040
#define			MATERIAL_SHIN_STRENGTH	0xA041

#define			MAPPING_PARAMETERS		0xA351
#define			BLUR_PERCENTAGE			0xA353

#define			TRANS_PERCENT			0xA050
#define			TRANS_FALLOFF_PERCENT	0xA052
#define			REFLECTION_BLUR_PER		0xA053
#define			RENDER_TYPE				0xA100

#define			SELF_ILLUM				0xA084
#define			WIRE_THICKNESS			0xA087
#define			IN_TRANC				0xA08A
#define			SOFTEN					0xA08C
     
#define	KEYFRAME						0xB000	
#define	  KEYFRAME_MESH_INFO			0xB002
#define   KEYFRAME_START_AND_END		0xB008
#define	  KEYFRAME_HEADER				0xb00a



#define  	POS_TRACK_TAG	0xb020 
#define  	ROT_TRACK_TAG	0xb021 
#define  	SCL_TRACK_TAG	0xb022 
#define		FOV_TRACK_TAG	0xb023
#define		ROLL_TRACK_TAG	0xb024	
#define		COL_TRACK_TAG  	0xb025
#define		MORPH_TRACK_TAG 0xb026
#define		HOT_TRACK_TAG  	0xb027
#define		FALL_TRACK_TAG  0xb028
#define		HIDE_TRACK_TAG  0xb029
#define  	PIVOT			0xb013 
#define  	NODE_HDR		0xb010 
#define  	NODE_ID			0xb030 
#define  	KFCURTIME		0xb009 


typedef struct 
{
	float x;
	float y;
	float z;
	
} VERT3DS;

typedef struct 
{
	short indices[4];
	int tex;
	
} FACE3DS;

typedef struct 
{
	float x;
	float y;

} MAPPING_COORDINATES;



typedef struct 
{
	VERT3DS *verts;
	FACE3DS *faces;
	MAPPING_COORDINATES *texverts;
	short numverts;
	short numfaces;
	short numtexverts;
	char texturename[10];
	char object_name[10];

} OBJECT3DS;


typedef struct
{		
	OBJECT3DS *Objects;
	int NumObjects;
	int NumTextures;
	
} k3DSMODEL;


class C3DS
{

public:

	C3DS();

	int total_num_objects;
	int num_materials;
	int num_maps;
	int total_num_faces;	
	int total_num_verts;
	int total_num_mcoords;
	int total_num_indices;

	BOOL Load3dsModel(k3DSMODEL *model, char *filename);

	BOOL Render3DSModel( LPDIRECT3DDEVICE7 m_pd3dDevice, 
						   float wx, float wy, float wz, 
						   float rotation_angle,
						   int frame_num,
						   k3DSMODEL *model );

	void Release3DSModel(k3DSMODEL* model);

	BOOL ProcessVertexData(FILE *fp);
	BOOL ProcessFaceData(FILE *fp);
	void ProcessTriLocalData(FILE *fp);
	void ProcessTriSmoothData(FILE *fp);
	void ProcessMappingData(FILE *fp);
	void ProcessTriangleMaterialData(FILE *fp);
	void AddMaterialName(FILE *fp);
	void AddMapName(FILE *fp);

	void ProcessNodeHeader(FILE *fp);
	void ProcessNodeId(FILE *fp);

	void ProcessMasterScale(FILE *fp);
	void Process3DSVersion(FILE *fp);

	void PrintLogFile(FILE *logfile, char *commmand);
	void Write_k3dsmodel_debugfile(k3DSMODEL *model);
	void ReleaseTempMemory();
};

#endif // IMPORT3DS_HPP