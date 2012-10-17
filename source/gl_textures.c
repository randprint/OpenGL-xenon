#include "gl_xenos.h"
#include <string.h>
#include <malloc.h>
#include <debug.h>

#define XE_MAX_TEXTURE 50000

glXeSurface_t * glXeSurfaces = NULL;

static inline void handle_small_surface(struct XenosSurface * surf, void * buffer){
	int width;
	int height;
	int wpitch;
	int hpitch;
	int yp,xp,y,x;
	uint32_t * surf_data;
	uint32_t * data;
	uint32_t * src;	

	// don't handle big texture
	if( surf->width>128 && surf->height>32) {
		return;
	}	

	width = surf->width;
	height = surf->height;
	wpitch = surf->wpitch / 4;
	hpitch = surf->hpitch;	

	if(buffer)
        surf_data = (uint32_t *)buffer;
    else
        surf_data = (uint32_t *)Xe_Surface_LockRect(xe, surf, 0, 0, 0, 0, XE_LOCK_WRITE);

	src = data = surf_data;

	for(yp=0; yp<hpitch;yp+=height) {
		int max_h = height;
		if (yp + height> hpitch)
				max_h = hpitch % height;
		for(y = 0; y<max_h; y++){
			//x order
			for(xp = 0;xp<wpitch;xp+=width) {
				int max_w = width;
				if (xp + width> wpitch)
					max_w = wpitch % width;

				for(x = 0; x<max_w; x++) {
					data[x+xp + ((y+yp)*wpitch)]=src[x+ (y*wpitch)];
				}
			}
		}
	}

    if(!buffer)
        Xe_Surface_Unlock(xe, surf);
} 

/***********************************************************************
 * Texture environnement
 ***********************************************************************/

void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	if (target != GL_TEXTURE_ENV)
		xe_gl_error("glTexEnvf: unimplemented target\n");
	
	if (!xeTmus[xeCurrentTMU].boundtexture) return;
	if (!xeTmus[xeCurrentTMU].boundtexture->teximg ) return;
	if (target != GL_TEXTURE_2D) return;
	
	/** Do shader work here !! **/
	switch (pname)
	{
		case GL_TEXTURE_ENV_MODE:
		switch ((GLint)param)
		{
			case GL_REPLACE:
			xeTmus[xeCurrentTMU].texture_env_mode = (int)GL_REPLACE;
			break;

			case GL_MODULATE:
			xeTmus[xeCurrentTMU].texture_env_mode = (int)GL_MODULATE;			
			break;
			
		}
	}
}

void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	
}

/***********************************************************************
 * Create/Gen/Delete images
 ***********************************************************************/
void XeGLInitTextures()
{
	glXeSurfaces = (glXeSurface_t *)malloc(sizeof(glXeSurface_t) * XE_MAX_TEXTURE);
	memset(glXeSurfaces, 0, sizeof(glXeSurface_t) * XE_MAX_TEXTURE);
}
 
 
static int d3d_TextureExtensionNumber = 1;
int xeCurrentTMU = 0;

static void Xe_InitTexture(glXeSurface_t *tex)
{
	tex->glnum = 0;

	if (tex->teximg)
	{
		Xe_DestroyTexture(xe, tex->teximg);
		tex->teximg = NULL;
	}
}

static glXeSurface_t *Xe_AllocTexture(void)
{
	int i = 0;
	glXeSurface_t *tex;

	// find a free texture
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		// if either of these are 0 (or NULL) we just reuse it
		if (!tex->teximg)
		{
			Xe_InitTexture(tex);
			return tex;
		}

		if (!tex->glnum)
		{
			Xe_InitTexture(tex);
			return tex;
		}
	}

	xe_gl_error("Xe_AllocTexture: out of textures!!!\n");

	return NULL;
}


static void Xe_ReleaseTextures (void)
{
	glXeSurface_t *tex;
	int i;

	// explicitly NULL all textures and force texparams to dirty
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		Xe_InitTexture(tex);
	}
} 
 
void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	int i;
	glXeSurface_t *tex;

	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		for (i = 0; i < n; i++) {
			if (tex->glnum == textures[i]) {
				Xe_InitTexture(tex);
				break;
			}
		}
	}
}

void glGenTextures(GLsizei n, GLuint *textures)
{
	int i;
	
	for(i = 0; i < n; i++)
	{
		glXeSurface_t *tex = Xe_AllocTexture();
		tex->glnum = textures[i] = d3d_TextureExtensionNumber;
		d3d_TextureExtensionNumber++;
	}
}

void glBindTexture(GLenum target, GLuint texture)
{
	int i;
	
	glXeSurface_t *tex;
	if (target != GL_TEXTURE_2D) 
		return;
		
	// no texture
	if (texture == 0) {
		xeTmus[xeCurrentTMU].boundtexture = NULL;
		return;
	}
	xeTmus[xeCurrentTMU].boundtexture = NULL;
	
	// find a texture
	for (i = 0; i< XE_MAX_TEXTURE; i++)
	{
		tex = &glXeSurfaces[i];
		
		if (tex && tex->glnum && tex->glnum == texture)
		{
			xeTmus[xeCurrentTMU].boundtexture = tex;
			break;
		}
	}
	
	// did we find it?
	if (!xeTmus[xeCurrentTMU].boundtexture)
	{
		// nope, so fill in a new one (this will make it work with texture_extension_number)
		// (i don't know if the spec formally allows this but id seem to have gotten away with it...)
		xeTmus[xeCurrentTMU].boundtexture = Xe_AllocTexture();

		// reserve this slot
		xeTmus[xeCurrentTMU].boundtexture->glnum = texture;

		// ensure that it won't be reused
		if (texture > d3d_TextureExtensionNumber)
			d3d_TextureExtensionNumber = texture;
	}
	// this should never happen
	if (!xeTmus[xeCurrentTMU].boundtexture) 
		xe_gl_error("glBindTexture: out of textures!!!\n");
		
	// dirty the params
	xeTmus[xeCurrentTMU].texparamdirty = TRUE;
}

/***********************************************************************
 * Images manipulation
 ***********************************************************************/

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
	if (level > 0)
		return;
	struct XenosSurface * surf = NULL;
	
	if (xeTmus[xeCurrentTMU].boundtexture && xeTmus[xeCurrentTMU].boundtexture->teximg ) {
		surf = xeTmus[xeCurrentTMU].boundtexture->teximg;
	}
	
	if (surf) {
		int srcbytes = 0;
		int dstbytes = surf->bypp;
		
		switch (format)
		{
		case 1:
		case GL_LUMINANCE:
			srcbytes = 1;
			break;
		case 3:
		case GL_RGB:
			srcbytes = 3;
			break;
		case 4:
		case GL_RGBA:
			srcbytes = 4;
			break;
		default:
			xe_gl_error ("invalid texture internal format\n");
			return;
		}
		
		uint8_t * surfbuf = (uint8_t*) Xe_Surface_LockRect(xe, surf, 0, 0, 0, 0, XE_LOCK_WRITE);
		uint8_t * srcdata = (uint8_t*) pixels;
		uint8_t * dstdata = surfbuf;
		
		int y, x;

		int pitch = (width * dstbytes);
		int offset = 0;

		for (y = yoffset; y < (yoffset + height); y++) {
			offset = (y * pitch)+(xoffset * dstbytes);

			dstdata = surfbuf + offset;
			for (x = xoffset; x < (xoffset + width); x++) {
				if (srcbytes == 4 && dstbytes == 4) {
					dstdata[0] = srcdata[3];
					dstdata[3] = srcdata[2];
					dstdata[2] = srcdata[1];
					dstdata[1] = srcdata[0];

					srcdata += srcbytes;
					dstdata += dstbytes;
				}
				if (srcbytes == 3 && dstbytes == 4) {
					dstdata[0] = 0xff;
					dstdata[3] = srcdata[2];
					dstdata[2] = srcdata[1];
					dstdata[1] = srcdata[0];

					srcdata += srcbytes;
					dstdata += dstbytes;
				}
				if (srcbytes == 1 && dstbytes == 1) {				
					dstdata[0] = srcdata[0];				
					srcdata += srcbytes;
					dstdata += dstbytes;
				}
			}
		}
		Xe_Surface_Unlock(xe, surf);
		
		handle_small_surface(surf, NULL);
	}
}

void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum pixelformat, GLenum type, const GLvoid *pixels)
{
	int format;
	if (type != GL_UNSIGNED_BYTE) {
		xe_gl_error("glTexImage2D: Unrecognised pixel format\n");
	}
	
	if (target != GL_TEXTURE_2D)
		return;
		
	if (!xeTmus[xeCurrentTMU].boundtexture)
		return;
		
	if (level > 0)
		return;
		
	int srcbytes = 4;
	int dstbytes = 4;
	
	// validate format
	switch (internalformat)
	{
	case 1:
	case GL_LUMINANCE:
		dstbytes = 1;
		format = XE_FMT_8;
		break;
	case 3:
	case GL_RGB:
		dstbytes = 4;
		format = XE_FMT_ARGB|XE_FMT_8888;
		break;
	case 4:
	case GL_RGBA:
		dstbytes = 4;
		format = XE_FMT_ARGB|XE_FMT_8888;
		break;
	default:
		xe_gl_error ("invalid texture internal format\n");
		return;
	}
	switch (pixelformat)
	{
	case 1:
	case GL_LUMINANCE:
		srcbytes = 1;
		break;
	case 3:
	case GL_RGB:
		srcbytes = 3;
		break;
	case 4:
	case GL_RGBA:
		srcbytes = 4;
		break;
	default:
		xe_gl_error ("invalid texture internal format\n");
	}
		
	struct XenosSurface * surf = Xe_CreateTexture(xe, width, height, 0, format, 0);
	
	xeTmus[xeCurrentTMU].boundtexture->teximg = surf;
		
	uint8_t * surfbuf = (uint8_t*) Xe_Surface_LockRect(xe, surf, 0, 0, 0, 0, XE_LOCK_WRITE);
	uint8_t * srcdata = (uint8_t*) pixels;
	uint8_t * dstdata = surfbuf;
	int y, x;

	for (y = 0; y <height; y++) {
		dstdata = surfbuf + (y * (width * dstbytes));
		for (x = 0; x < width; x++) {
			if (srcbytes == 4 && dstbytes == 4) {
				
				dstdata[0] = srcdata[3];
				dstdata[3] = srcdata[2];
				dstdata[2] = srcdata[1];
				dstdata[1] = srcdata[0];
				
				srcdata += srcbytes;
				dstdata += dstbytes;
			}
			if (srcbytes == 3 && dstbytes == 4) {
				
				dstdata[0] = 0xff;
				dstdata[3] = srcdata[2];
				dstdata[2] = srcdata[1];
				dstdata[1] = srcdata[0];
				
				srcdata += srcbytes;
				dstdata += dstbytes;
			}
			if (srcbytes == 1 && dstbytes == 1) {
				
				dstdata[0] = srcdata[0];				
				srcdata += srcbytes;
				dstdata += dstbytes;
			}
		}
	}

	Xe_Surface_Unlock(xe, surf);
	handle_small_surface(surf, NULL);
}

/** Not used in QII */
void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
	
}

/***********************************************************************
 * Images parameters
 ***********************************************************************/
 
#define XE_TEXF_NONE	2
#define XE_TEXF_POINT 0
#define XE_TEXF_LINEAR 1
#define XE_TEXF_ANISOTROPIC = 4

void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	if (!xeTmus[xeCurrentTMU].boundtexture) return;
	if (!xeTmus[xeCurrentTMU].boundtexture->teximg ) return;
	if (target != GL_TEXTURE_2D) return;

	xeTmus[xeCurrentTMU].texparamdirty = TRUE;

	switch (pname)
	{
	case GL_TEXTURE_MIN_FILTER:
		if ((int) param == GL_NEAREST_MIPMAP_NEAREST)
		{
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
		}
		else if ((int) param == GL_LINEAR_MIPMAP_NEAREST)
		{
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
		}
		else if ((int) param == GL_NEAREST_MIPMAP_LINEAR)
		{
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
		}
		else if ((int) param == GL_LINEAR_MIPMAP_LINEAR)
		{
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
		}
		else if ((int) param == GL_LINEAR)
		{
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_NONE;
		}
		else
		{
			// GL_NEAREST
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_NONE;
		}
		break;

	case GL_TEXTURE_MAG_FILTER:
		if ((int) param == GL_LINEAR)
			xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_LINEAR;
		else xeTmus[xeCurrentTMU].boundtexture->teximg->use_filtering = XE_TEXF_POINT;
		break;

	case GL_TEXTURE_WRAP_S:
		if ((int) param == GL_CLAMP)
			xeTmus[xeCurrentTMU].boundtexture->teximg->u_addressing = XE_TEXADDR_CLAMP;
		else xeTmus[xeCurrentTMU].boundtexture->teximg->u_addressing = XE_TEXADDR_WRAP;
		break;

	case GL_TEXTURE_WRAP_T:
		if ((int) param == GL_CLAMP)
			xeTmus[xeCurrentTMU].boundtexture->teximg->v_addressing = XE_TEXADDR_CLAMP;
		else xeTmus[xeCurrentTMU].boundtexture->teximg->v_addressing = XE_TEXADDR_WRAP;
		break;

	default:
		break;
	}
}

 
void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	glTexParameterf (target, pname, param);
}

void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
	
}


