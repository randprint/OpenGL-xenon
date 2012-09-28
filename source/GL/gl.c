#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ppc/timebase.h>
#include <xenos/edram.h>
#include "gl_xenos.h"

typedef unsigned int DWORD;
#include "shaders/opengl.ps.h"
#include "shaders/opengl.vs.h"

void XenonGLInit(){
	// init video
	xe=&_xe;
	
	Xe_Init(xe);
	
	edram_init(xe);
	Xe_SetRenderTarget(xe, Xe_GetFramebufferSurface(xe));
	static const struct XenosVBFFormat vbf = {
		4,
		{
			{XE_USAGE_POSITION, 0, XE_TYPE_FLOAT4},
			{XE_USAGE_TEXCOORD, 0, XE_TYPE_FLOAT2},			
			{XE_USAGE_TEXCOORD, 1, XE_TYPE_FLOAT2},
			{XE_USAGE_COLOR, 0, XE_TYPE_UBYTE4},
		}
	};
	
	pPixelShader = Xe_LoadShaderFromMemory(xe, (void*) g_xps_ps_main);
	Xe_InstantiateShader(xe, pPixelShader, 0);

	pVertexShader = Xe_LoadShaderFromMemory(xe, (void*) g_xvs_vs_main);
	Xe_InstantiateShader(xe, pVertexShader, 0);
	Xe_ShaderApplyVFetchPatches(xe, pVertexShader, 0, &vbf);
	
	// Create vb
	pVbGL = Xe_CreateVertexBuffer(xe, XE_MAX_VERTICES);
	xe_Vertices = Xe_VB_Lock(xe, pVbGL, 0, XE_MAX_VERTICES, XE_LOCK_WRITE);
	Xe_VB_Unlock(xe, pVbGL);
	
	// init matrices
	XeGlInitializeMatrix(&projection_matrix);
	XeGlInitializeMatrix(&modelview_matrix);
	
	// init vertices
	xe_NumVerts = 0;
	xe_CurrentColor.u32 = 0xFFFFFFFF;
	
	// init textures
	// not yet ...
	Xe_InvalidateState(xe);
	Xe_SetClearColor(xe, 0);
	
}

static void ShowFPS() {
	static unsigned long lastTick = 0;
	static int frames = 0;
	unsigned long nowTick;
	frames++;
	nowTick = mftb() / (PPC_TIMEBASE_FREQ / 1000);
	if (lastTick + 1000 <= nowTick) {

		printf("SWimp_EndFrame %d fps\r\n", frames);

		frames = 0;
		lastTick = nowTick;
	}
}

void XenonGLDisplay()
{
    Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
    Xe_SetSrcBlend(xe, XE_BLEND_SRCALPHA);
    Xe_SetDestBlend(xe, XE_BLEND_INVSRCALPHA);
    
    // update vb cache !!!
    Xe_VB_Lock(xe, pVbGL, 0, XE_MAX_VERTICES, XE_LOCK_WRITE);
	Xe_VB_Unlock(xe, pVbGL);    
    
/*
	// Refresh texture cash
    unsigned int * pBitmap = Xe_Surface_LockRect(xe, pVideoSurface, 0, 0, 0, 0, XE_LOCK_WRITE);
    int i;
    for(i = 0;i<(vid.width*vid.height);i++)
	{
		//Make an ARGB bitmap
		unsigned int c = (pQ2Palette[vid.buffer[i]]) >> 8 | ( 0xFF << 24 );
		pBitmap[i] = c;
	}
    
    Xe_Surface_Unlock(xe, pVideoSurface);

    // Select stream and shaders
    Xe_SetTexture(xe, 0, pVideoSurface);
    Xe_SetCullMode(xe, XE_CULL_NONE);
    Xe_SetStreamSource(xe, 0, pVBSw, 0, 10);
    Xe_SetShader(xe, SHADER_TYPE_PIXEL, pPixelShader, 0);
    Xe_SetShader(xe, SHADER_TYPE_VERTEX, pVertexShader, 0);
   
    // Draw
    Xe_DrawPrimitive(xe, XE_PRIMTYPE_RECTLIST, 0, 1);
*/    
    // Resolve
    Xe_Resolve(xe);
    while(!Xe_IsVBlank(xe));
    Xe_Sync(xe);
    
    // Reset states
    Xe_InvalidateState(xe);
    Xe_SetClearColor(xe, 0);
    
    xe_NumVerts = xe_PrevNumVerts = 0;
 
	ShowFPS();
}

void xe_gl_error(const char * format, ...)
{
	printf(format);
};
void xe_gl_log(const char * format, ...)
{
	printf(format);
};
