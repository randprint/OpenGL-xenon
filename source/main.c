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

#if 0
// lesson 2

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
#endif

#if 1
// lesson 5

/* rotation angle for the triangle. */
float rtri = 0.0f;

/* rotation angle for the quadrilateral. */
float rquad = 0.0f;

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);			        // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);		        // Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}


/* The main drawing function. */
void DrawGLScene()
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
  glLoadIdentity();				// Reset The View

  glTranslatef(-1.5f,0.0f,-6.0f);		// Move Left 1.5 Units And Into The Screen 6.0
	
  glRotatef(rtri,0.0f,1.0f,0.0f);		// Rotate The Pyramid On The Y axis 

  // draw a pyramid (in smooth coloring mode)
  glBegin(GL_POLYGON);				// start drawing a pyramid

  // front face of pyramid
  glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
  glVertex3f(0.0f, 1.0f, 0.0f);		        // Top of triangle (front)
  glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
  glVertex3f(-1.0f,-1.0f, 1.0f);		// left of triangle (front)
  glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
  glVertex3f(1.0f,-1.0f, 1.0f);		        // right of traingle (front)	

  // right face of pyramid
  glColor3f(1.0f,0.0f,0.0f);			// Red
  glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Right)
  glColor3f(0.0f,0.0f,1.0f);			// Blue
  glVertex3f( 1.0f,-1.0f, 1.0f);		// Left Of Triangle (Right)
  glColor3f(0.0f,1.0f,0.0f);			// Green
  glVertex3f( 1.0f,-1.0f, -1.0f);		// Right Of Triangle (Right)

  // back face of pyramid
  glColor3f(1.0f,0.0f,0.0f);			// Red
  glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Back)
  glColor3f(0.0f,1.0f,0.0f);			// Green
  glVertex3f( 1.0f,-1.0f, -1.0f);		// Left Of Triangle (Back)
  glColor3f(0.0f,0.0f,1.0f);			// Blue
  glVertex3f(-1.0f,-1.0f, -1.0f);		// Right Of Triangle (Back)

  // left face of pyramid.
  glColor3f(1.0f,0.0f,0.0f);			// Red
  glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Left)
  glColor3f(0.0f,0.0f,1.0f);			// Blue
  glVertex3f(-1.0f,-1.0f,-1.0f);		// Left Of Triangle (Left)
  glColor3f(0.0f,1.0f,0.0f);			// Green
  glVertex3f(-1.0f,-1.0f, 1.0f);		// Right Of Triangle (Left)

  glEnd();					// Done Drawing The Pyramid

  glLoadIdentity();				// make sure we're no longer rotated.
  glTranslatef(1.5f,0.0f,-7.0f);		// Move Right 3 Units, and back into the screen 7
	
  glRotatef(rquad,1.0f,1.0f,1.0f);		// Rotate The Cube On X, Y, and Z

  // draw a cube (6 quadrilaterals)
  glBegin(GL_QUADS);				// start drawing the cube.
  
  // top of cube
  glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Blue
  glVertex3f( 1.0f, 1.0f,-1.0f);		// Top Right Of The Quad (Top)
  glVertex3f(-1.0f, 1.0f,-1.0f);		// Top Left Of The Quad (Top)
  glVertex3f(-1.0f, 1.0f, 1.0f);		// Bottom Left Of The Quad (Top)
  glVertex3f( 1.0f, 1.0f, 1.0f);		// Bottom Right Of The Quad (Top)

  // bottom of cube
  glColor3f(1.0f,0.5f,0.0f);			// Set The Color To Orange
  glVertex3f( 1.0f,-1.0f, 1.0f);		// Top Right Of The Quad (Bottom)
  glVertex3f(-1.0f,-1.0f, 1.0f);		// Top Left Of The Quad (Bottom)
  glVertex3f(-1.0f,-1.0f,-1.0f);		// Bottom Left Of The Quad (Bottom)
  glVertex3f( 1.0f,-1.0f,-1.0f);		// Bottom Right Of The Quad (Bottom)

  // front of cube
  glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
  glVertex3f( 1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Front)
  glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Front)
  glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Front)
  glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Front)

  // back of cube.
  glColor3f(1.0f,1.0f,0.0f);			// Set The Color To Yellow
  glVertex3f( 1.0f,-1.0f,-1.0f);		// Top Right Of The Quad (Back)
  glVertex3f(-1.0f,-1.0f,-1.0f);		// Top Left Of The Quad (Back)
  glVertex3f(-1.0f, 1.0f,-1.0f);		// Bottom Left Of The Quad (Back)
  glVertex3f( 1.0f, 1.0f,-1.0f);		// Bottom Right Of The Quad (Back)

  // left of cube
  glColor3f(0.0f,0.0f,1.0f);			// Blue
  glVertex3f(-1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Left)
  glVertex3f(-1.0f, 1.0f,-1.0f);		// Top Left Of The Quad (Left)
  glVertex3f(-1.0f,-1.0f,-1.0f);		// Bottom Left Of The Quad (Left)
  glVertex3f(-1.0f,-1.0f, 1.0f);		// Bottom Right Of The Quad (Left)

  // Right of cube
  glColor3f(1.0f,0.0f,1.0f);			// Set The Color To Violet
  glVertex3f( 1.0f, 1.0f,-1.0f);	        // Top Right Of The Quad (Right)
  glVertex3f( 1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Right)
  glVertex3f( 1.0f,-1.0f, 1.0f);		// Bottom Left Of The Quad (Right)
  glVertex3f( 1.0f,-1.0f,-1.0f);		// Bottom Right Of The Quad (Right)
  glEnd();					// Done Drawing The Cube

  rtri+=15.0f;					// Increase The Rotation Variable For The Pyramid
  rquad-=15.0f;					// Decrease The Rotation Variable For The Cube

  // swap the buffers to display, since double buffering is used.
  XenonGLDisplay();
}

#endif


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
