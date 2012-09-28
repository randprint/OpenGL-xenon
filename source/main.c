#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <gl_xenos.h>
#include <console/console.h>
#include <xenos/xenos.h>
#include <xenon_soc/xenon_power.h>

void gluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan( fovy * M_PI / 360.0 );
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)
{
	
  glClearColor(0.5f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer

  glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
  glLoadIdentity();				// Reset The View

  glTranslatef(-1.5f,0.0f,-6.0f);		// Move Left 1.5 Units And Into The Screen 6.0
	
  // draw a triangle
  glBegin(GL_TRIANGLES);				// start drawing a polygon
  glVertex3f( 0.0f, 1.0f, 0.0f);		// Top
  glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
  glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left	
  glEnd();					// we're done with the polygon

  glTranslatef(3.0f,0.0f,0.0f);		        // Move Right 3 Units
	
  // draw a square (quadrilateral)
  glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
  glVertex3f(-1.0f, 1.0f, 0.0f);		// Top Left
  glVertex3f( 1.0f, 1.0f, 0.0f);		// Top Right
  glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
  glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left	
  glEnd();					// done with the polygon

  // swap buffers to display, since we're double buffered.
  XenonGLDisplay();
}

int main(int argc, char **argv) 
{  
	xenos_init(VIDEO_MODE_AUTO);
	console_init();

	xenon_make_it_faster(XENON_SPEED_FULL);
	
	XenonGLInit();
	InitGL(1280, 720);
	while(1) {
		DrawGLScene();
	}

  return 1;
}
