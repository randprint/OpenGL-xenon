#include "gl_xenos.h"
#include <string.h>
#include <malloc.h>

glXeSurface_t * glXeSurfaces = NULL;

/***********************************************************************
 * Texture environnement
 ***********************************************************************/
void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	/** Do shader work here !! **/
	
}

void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	
}

/***********************************************************************
 * Create/Gen/Delete images
 ***********************************************************************/
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
	glXeSurface_t *tex;

	// find a free texture
	for (tex = glXeSurfaces; tex; tex = tex->next)
	{
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

	// nothing to reuse so create a new one
	// clear to 0 is required so that D3D_SAFE_RELEASE is valid
	tex = (glXeSurface_t *) malloc (sizeof (glXeSurface_t));
	memset (tex, 0, sizeof (glXeSurface_t));
	Xe_InitTexture(tex);

	// link in
	tex->next = glXeSurfaces;
	glXeSurfaces = tex;

	// return the new one
	return tex;
}


static void Xe_ReleaseTextures (void)
{
	glXeSurface_t *tex;

	// explicitly NULL all textures and force texparams to dirty
	for (tex = glXeSurfaces; tex; tex = tex->next)
	{
		Xe_InitTexture(tex);
	}
} 
 
void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	int i;
	glXeSurface_t *tex;

	for (tex = glXeSurfaces; tex; tex = tex->next) {
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
	for (tex = glXeSurfaces; tex; tex = tex->next)
	{
		if (tex->glnum == texture)
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
		xe_gl_error("glBindTexture: out of textures!!!");

	// dirty the params
	xeTmus[xeCurrentTMU].texparamdirty = TRUE;
}

/***********************************************************************
 * Images manipulation
 ***********************************************************************/

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
	int srcbpp = 4;
	int dstbpp = 4;
	struct XenosSurface * surf = NULL;
	
	if (xeTmus[xeCurrentTMU].boundtexture && xeTmus[xeCurrentTMU].boundtexture->teximg ) {
		surf = xeTmus[xeCurrentTMU].boundtexture->teximg;
	}
	
	if (surf) {
		uint8_t * surfbuf = (uint8_t*) Xe_Surface_LockRect(xe, surf, 0, 0, 0, 0, XE_LOCK_WRITE);
		uint8_t * srcdata = (uint8_t*) pixels;
		uint8_t * dstdata = surfbuf;
		int srcbytes = srcbpp;
		int dstbytes = dstbpp;
		int y, x;

		int pitch = (width * dstbytes);
		int offset = 0;

		for (y = yoffset; y < (yoffset + height); y++) {
			offset = (y * pitch)+(xoffset * dstbytes);

			dstdata = surfbuf + offset;
			for (x = xoffset; x < (xoffset + width); x++) {
				if (srcbpp == 4 && dstbytes == 4) {
					dstdata[0] = srcdata[0];
					dstdata[1] = srcdata[1];
					dstdata[2] = srcdata[2];
					dstdata[3] = srcdata[3];

					srcdata += srcbytes;
					dstdata += dstbytes;
				}
			}
		}

		Xe_Surface_Unlock(xe, surf);
	}
}

void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum glformat, GLenum type, const GLvoid *pixels)
{
	int format;
	if (type != GL_UNSIGNED_BYTE) {
		xe_gl_error("glTexImage2D: Unrecognised pixel format\n");
	}
	
	if (target != GL_TEXTURE_2D)
		return;
		
	if (!xeTmus[xeCurrentTMU].boundtexture)
		return;
		
	// validate format
	switch (glformat)
	{
	case 1:
	case GL_LUMINANCE:
		format = XE_FMT_8;
		break;
	case 4:
	case GL_RGBA:
		format = XE_FMT_ARGB|XE_FMT_8888;
		break;
	default:
		xe_gl_error ("invalid texture internal format\n");
	}
		
	struct XenosSurface * surf = Xe_CreateTexture(xe, width, height, 0, format, 0);
	
	xeTmus[xeCurrentTMU].boundtexture->teximg = surf;
	
	uint8_t * surfbuf = (uint8_t*) Xe_Surface_LockRect(xe, surf, 0, 0, 0, 0, XE_LOCK_WRITE);
	uint8_t * srcdata = (uint8_t*) pixels;
	uint8_t * dstdata = surfbuf;
	int srcbytes = 4;
	int dstbytes = 4;
	int y, x;

	for (y = 0; y <height; y++) {
		dstdata = surfbuf + (y * (width * dstbytes));
		for (x = 0; x < width; x++) {
			if (srcbytes == 4 && dstbytes == 4) {
				
				dstdata[0] = srcdata[0];
				dstdata[1] = srcdata[1];
				dstdata[2] = srcdata[2];
				dstdata[3] = srcdata[3];
				
				srcdata += srcbytes;
				dstdata += dstbytes;
			}
		}
	}

	Xe_Surface_Unlock(xe, surf);
}

/** Not used in QII */
void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
	
}

/***********************************************************************
 * Images parameters
 ***********************************************************************/
void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
	
}

void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
	
}


void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
	
}


