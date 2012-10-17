#include "gl_xenos.h"

static GLenum gl_cull_mode = 0;
static GLboolean gl_cull_enable = GL_FALSE;
static GLenum gl_front_face = 0;

void glShadeModel (GLenum mode)
{
	
}

void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
	Xe_SetScissor(xe, 1, x, y, x+width, y+height);
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
 * Cull
 ***********************************************************************/
static void updateCullMode()
{
#if 1
	if (gl_cull_enable == GL_FALSE)
	{
		// disable culling
		Xe_SetCullMode(xe, XE_CULL_NONE);
		return;
	}

	if (gl_front_face == GL_CCW)
	{
		if (gl_cull_mode == GL_BACK)
			Xe_SetCullMode(xe, XE_CULL_CW);
		else if (gl_cull_mode == GL_FRONT)
			Xe_SetCullMode(xe, XE_CULL_CCW);
		else if (gl_cull_mode == GL_FRONT_AND_BACK)
			; // do nothing; we cull in software in GL_SubmitVertexes instead
		else xe_gl_error ("GL_UpdateCull: illegal glCullFace\n");
	}
	else if (gl_front_face == GL_CW)
	{
		if (gl_cull_mode == GL_BACK)
			Xe_SetCullMode(xe, XE_CULL_CCW);
		else if (gl_cull_mode == GL_FRONT)
			Xe_SetCullMode(xe, XE_CULL_CW);
		else if (gl_cull_mode == GL_FRONT_AND_BACK)
			; // do nothing; we cull in software in GL_SubmitVertexes instead
		else xe_gl_error ("GL_UpdateCull: illegal glCullFace\n");
	}
	else xe_gl_error ("GL_UpdateCull: illegal glFrontFace\n");
	Xe_SetCullMode(xe, gl_cull_mode);
#else 
	if (gl_cull_enable == GL_FALSE)
	{
		// disable culling
		Xe_SetCullMode(xe, XE_CULL_NONE);
		return;
	}
	if (gl_front_face == GL_CCW) {
		Xe_SetCullMode(xe, XE_CULL_CW);
	} else {
		Xe_SetCullMode(xe, XE_CULL_CCW);
	}
#endif
}
void glFrontFace (GLenum mode)
{
	gl_front_face = mode;
	updateCullMode();
}
void glCullFace (GLenum mode)
{
	gl_cull_mode = mode;
	updateCullMode();
}
/***********************************************************************
 * Depth
 ***********************************************************************/
int Gl_ZCmp_2_Xe(GLenum mode)
{
	// xenos use reverse Z cmp
	int cmp = 0;
	switch (mode)
	{
	case GL_NEVER:
		cmp = XE_CMP_NEVER;
		break;

	case GL_LESS:
		// cmp = XE_CMP_LESS;
		cmp = XE_CMP_GREATER;
		break;

	case GL_LEQUAL:
		//cmp = XE_CMP_LESSEQUAL;
		cmp = XE_CMP_GREATEREQUAL;
		break;

	case GL_EQUAL:
		cmp = XE_CMP_EQUAL;
		break;

	case GL_GREATER:
		// cmp = XE_CMP_GREATER;
		cmp = XE_CMP_LESS;
		break;

	case GL_NOTEQUAL:
		cmp = XE_CMP_NOTEQUAL;
		break;

	case GL_GEQUAL:
		//cmp = XE_CMP_GREATEREQUAL;
		cmp = XE_CMP_LESSEQUAL;
		break;

	case GL_ALWAYS:
	default:
		cmp = XE_CMP_ALWAYS;
		break;
	}
}
 
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
	// use reverse Z
	Xe_SetZFunc(xe, Gl_Cmp_2_Xe(func));
}

void glDepthMask (GLboolean flag)
{
	Xe_SetZEnable(xe, flag == GL_TRUE ? 1 : 0);
}

void glAlphaFunc (GLenum func, GLclampf ref)
{
	//Xe_SetAlphaFunc(xe, Gl_Cmp_2_Xe(func));
	//Xe_SetAlphaRef(xe, ref * 255);
}
void glDepthRange (GLclampd zNear, GLclampd zFar)
{
	//xe_gl_log("glDepthRange Not implemented\n");
}
void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
	//xe_gl_log("glViewport Not implemented\n");
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

static int old_src_blend = XE_BLEND_ONE;
static int old_dst_blend = XE_BLEND_ZERO;
static int old_blend_op = XE_BLENDOP_ADD;

void glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	old_src_blend = Gl_Blend_2_Xe(sfactor);
	old_dst_blend = Gl_Blend_2_Xe(dfactor);
	
	Xe_SetSrcBlend(xe, old_src_blend);
	Xe_SetDestBlend(xe, old_dst_blend);
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
		else {
			Xe_SetSrcBlend(xe, old_src_blend);
			Xe_SetDestBlend(xe, old_dst_blend);
			Xe_SetBlendOp(xe, old_blend_op);
		}
		break;

	case GL_ALPHA_TEST:
		Xe_SetAlphaTestEnable(xe, enable?0:1);
		break;

	case GL_TEXTURE_2D:
		xeTmus[xeCurrentTMU].enabled = enable;
		return;

	case GL_CULL_FACE:
		if (!enable)
			gl_cull_enable = GL_FALSE;
		updateCullMode();
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


/***********************************************************************
 * Misc
 ***********************************************************************/
static int fill_back = 0;
static int fill_front = 0;
void glPolygonMode (GLenum face, GLenum mode)
{
	int xmode = 0;
	
	if (mode == GL_LINE)
		xmode = XE_FILL_WIREFRAME;
	else if (mode == GL_POINT)
		xmode = XE_FILL_POINT;
	else 
		xmode = XE_FILL_SOLID;
	
	if (face == GL_FRONT)
		fill_front = xmode;
	else if (face == GL_BACK)
		fill_back = xmode;
	else
		fill_back = fill_front = xmode;
	
	Xe_SetFillMode(xe, fill_front, fill_back);
}
