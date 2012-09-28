#include "gl_xenos.h"

void glShadeModel (GLenum mode)
{
	
}

/***********************************************************************
 * Clear
 ***********************************************************************/
void glClear (GLbitfield mask)
{
	unsigned int flags = 0;

	if (mask & GL_COLOR_BUFFER_BIT) flags |= XE_CLEAR_COLOR;
	if (mask & GL_DEPTH_BUFFER_BIT) flags |= XE_CLEAR_DS;
	if (mask & GL_STENCIL_BUFFER_BIT) flags |= XE_CLEAR_DS;

	Xe_Clear(xe, flags);
}

void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	Xe_SetClearColor(xe, Gl_Color_2_Xe (red, green, blue, alpha));
}

void glClearDepth(GLclampd depth)
{
	
}

/***********************************************************************
 * Depth
 ***********************************************************************/
int Gl_Cmp_2_Xe(GLenum mode)
{
	int cmp = 0;
	switch (mode)
	{
	case GL_NEVER:
		cmp = XE_CMP_NEVER;
		break;

	case GL_LESS:
		cmp = XE_CMP_LESS;
		break;

	case GL_LEQUAL:
		cmp = XE_CMP_LESSEQUAL;
		break;

	case GL_EQUAL:
		cmp = XE_CMP_EQUAL;
		break;

	case GL_GREATER:
		cmp = XE_CMP_GREATER;
		break;

	case GL_NOTEQUAL:
		cmp = XE_CMP_NOTEQUAL;
		break;

	case GL_GEQUAL:
		cmp = XE_CMP_GREATEREQUAL;
		break;

	case GL_ALWAYS:
	default:
		cmp = XE_CMP_ALWAYS;
		break;
	}
}
 
void glDepthFunc (GLenum func)
{
	Xe_SetZFunc(xe, Gl_Cmp_2_Xe(func));
}

void glAlphaFunc (GLenum func, GLclampf ref)
{
	Xe_SetAlphaFunc(xe, Gl_Cmp_2_Xe(func));
	Xe_SetAlphaRef(xe, ref);
}

/***********************************************************************
 * Blend
 ***********************************************************************/
 int Gl_Blend_2_Xe(GLenum factor)
{
	int blend = XE_BLEND_ONE;

	switch (factor)
	{
	case GL_ZERO:
		blend = XE_BLEND_ZERO;
		break;

	case GL_ONE:
		blend = XE_BLEND_ONE;
		break;

	case GL_SRC_COLOR:
		blend = XE_BLEND_SRCCOLOR;
		break;

	case GL_ONE_MINUS_SRC_COLOR:
		blend = XE_BLEND_INVSRCCOLOR;
		break;

	case GL_DST_COLOR:
		blend = XE_BLEND_DESTCOLOR;
		break;

	case GL_ONE_MINUS_DST_COLOR:
		blend = XE_BLEND_INVDESTCOLOR;
		break;

	case GL_SRC_ALPHA:
		blend = XE_BLEND_SRCALPHA;
		break;

	case GL_ONE_MINUS_SRC_ALPHA:
		blend = XE_BLEND_INVSRCALPHA;
		break;

	case GL_DST_ALPHA:
		blend = XE_BLEND_DESTALPHA;
		break;

	case GL_ONE_MINUS_DST_ALPHA:
		blend = XE_BLEND_INVDESTALPHA;
		break;

	case GL_SRC_ALPHA_SATURATE:
		blend = XE_BLEND_SRCALPHASAT;
		break;

	default:
		xe_gl_error("glBlendFunc: unknown factor");
	}
	return blend;
}
 
void glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	Xe_SetSrcBlend(xe, Gl_Blend_2_Xe(sfactor));
	Xe_SetDestBlend(xe, Gl_Blend_2_Xe(dfactor));
}
/***********************************************************************
 * Blend
 ***********************************************************************/
void GlEnableDisable(GLenum cap, int enable)
{
	switch (cap)
	{
	case GL_SCISSOR_TEST:		
		break;

	case GL_BLEND:
		if (!enable) {
			Xe_SetSrcBlend(xe, XE_BLEND_ONE);
			Xe_SetDestBlend(xe, XE_BLEND_ZERO);
			Xe_SetBlendOp(xe, XE_BLENDOP_ADD);
		}
		break;

	case GL_ALPHA_TEST:
		Xe_SetAlphaTestEnable(xe, enable);
		break;

	case GL_TEXTURE_2D:
		return;

	case GL_CULL_FACE:		
		return;

	case GL_FOG:
		break;

	case GL_DEPTH_TEST:
		Xe_SetZEnable(xe, enable);
		break;

	case GL_POLYGON_OFFSET_FILL:
	case GL_POLYGON_OFFSET_LINE:
		return;

	default:
		return;
	}
}

void glEnable(GLenum cap)
{
	GlEnableDisable(cap, 1);
}

void glDisable(GLenum cap)
{
	GlEnableDisable(cap, 0);
}
