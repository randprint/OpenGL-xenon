#pragma once

#include "gl.h"
#include <xenos/xe.h>
#include "Vec.h"
/** based on 
 * http://quakeone.com/mh/gl_fakegl.c
 * http://fossies.org/dox/wine-1.4.1/d3dx9__36_2math_8c_source.html
 **/ 

/***********************************************************************
 * Vertices
 ***********************************************************************/
 
#define XE_MAX_VERTICES	16*1024 // 16Mo
 
typedef struct {	
    float x, y, z, w;
    // texture 0
    float u0, v0;
    // texture 1    
    float u1, v1;
    // color
    unsigned int color;
} glVerticesFormat_t;

typedef struct {
	float u, v;
} glTextCoord_t; 

typedef struct {
	union {
		unsigned char a, r, g, b;
	} u8;
	unsigned int u32;
} glColor_t; 

glVerticesFormat_t * xe_Vertices;
int xe_NumVerts;
int xe_PrevNumVerts;
glColor_t xe_CurrentColor;
glTextCoord_t xe_TextCoord[2];

GLenum xe_PrimitiveMode;

unsigned int Gl_Color_2_Xe (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

/***********************************************************************
 * Textures
 ***********************************************************************/
typedef struct glXeSurface_s{
	// OpenGL texture id
	int glnum;
	// Used by glTexSubImage2D
	GLenum internalformat;
	
	struct XenosSurface * teximg;
	
	struct glXeSurface_s * next;
} glXeSurface_t;

glXeSurface_t * glXeSurfaces;

/***********************************************************************
 * Matrices
 ***********************************************************************/
 #define MAX_MATRIX_STACK 32
 
typedef enum{
	MATMODELVIEW	= 0, // register see opengl.hlsl
	MATPROJECTION	= 8,
} TRANSFORM_TYPE;

typedef struct
{
	int dirty;
	int stackdepth;
	TRANSFORM_TYPE usage;
	matrix4x4 stack[MAX_MATRIX_STACK];
} xe_matrix_t;

xe_matrix_t projection_matrix;
xe_matrix_t modelview_matrix;

void XeGlInitializeMatrix(xe_matrix_t *m);
void XeGlCheckDirtyMatrix(xe_matrix_t *m);

/***********************************************************************
 * Global
 ***********************************************************************/
struct XenosDevice _xe, *xe;
struct XenosShader * pVertexShader;
struct XenosShader * pPixelShader;
struct XenosVertexBuffer * pVbGL;

/***********************************************************************
 * Utils
 ***********************************************************************/
void xe_gl_error(const char * format, ...);
void xe_gl_log(const char * format, ...);
void XenonGLInit();
void XenonGLDisplay();
