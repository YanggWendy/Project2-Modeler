#include "modelerdraw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include "modelerglobals.h"
#include "modelerapp.h"
#include <cstdio>
#include <math.h>
#include <iostream>
using namespace std;

static int right_wing_angle = 0;
static bool right_wing_flag = false;
static int left_wing_angle = 0;
static bool left_wing_flag = false;

static int right_leg_angle = 0;
static bool right_leg_flag = false;
static int left_leg_angle = 0;
static bool left_leg_flag = false;

static int head_angle = 0;
static bool head_flag = false;

static int tail_angle = 0;
static bool tail_flag = false;

// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; position=(0,0.8,5); direction=(0,-0.8,-5); }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-2,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}

void draw_Head()
{

    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glScaled(1.5, 1.2, 1.2);
 //draw head
    drawSphere(1);
   
//draw eyes
    setDiffuseColor(0.93, 0.91, 0.84);
    glTranslated(0, 0, 0.45);
    drawSphere(0.6);
    glTranslated(0, 0, -0.9);
    drawSphere(0.6);
    setDiffuseColor(0.11, 0.11, 0.11);
    glTranslated(0.14, 0,-0.45);
    drawSphere(0.2);
    glTranslated(0, 0, 1.8);
    drawSphere(0.2);
    glPopMatrix();


}

void draw_UpperMouth()
{
    glPushMatrix();
    setDiffuseColor(VAL(MOUTH_R), VAL(MOUTH_G), VAL(MOUTH_B));
    glScaled(1, 1, 2);
    drawTriangle(0, 0.3, -0.1, 0, 0.3, 0.1, 1.9, 0, 0);
    drawTriangle(0, 0.3, -0.1, 0, 0.2, -0.2, 1.9, 0, 0);
    drawTriangle(0, 0.3, 0.1, 0, 0.2, 0.2, 1.9, 0, 0);

    drawTriangle(0, 0.2, -0.2, 0, 0, -0.3, 1.9, 0, 0);
    drawTriangle(0, 0.2, 0.2, 0, 0, 0.3, 1.9, 0, 0);

    drawTriangle(0, 0.3, -0.1, 0, 0.3, 0.1, 0, 0, 0);
    drawTriangle(0, 0.3, -0.1, 0, 0.2, -0.2, 0, 0, 0);
    drawTriangle(0, 0.3, 0.1, 0, 0.2, 0.2, 0, 0, 0);

    drawTriangle(0, 0.2, -0.2, 0, 0, -0.3, 0, 0, 0);
    drawTriangle(0, 0.2, 0.2, 0, 0, 0.3, 0, 0, 0);
    glPopMatrix();
}

void draw_LowerMouth()
{
    glPushMatrix();
    setDiffuseColor(VAL(MOUTH_R), VAL(MOUTH_G), VAL(MOUTH_B));
    glScaled(1, 1, 2);
    drawTriangle(0, -0.3, -0.1, 0, -0.3, 0.1, 1.9, 0, 0);
    drawTriangle(0, -0.3, -0.1, 0, -0.2, -0.2, 1.9, 0, 0);
    drawTriangle(0, -0.3, 0.1, 0,-0.2, 0.2, 1.9, 0, 0);

    drawTriangle(0, -0.2, -0.2, 0, 0, -0.3, 1.9, 0, 0);
    drawTriangle(0, -0.2, 0.2, 0, 0, 0.3, 1.9, 0, 0);

    drawTriangle(0, -0.3, -0.1, 0, -0.3, 0.1, 0, 0, 0);
    drawTriangle(0, -0.3, -0.1, 0, -0.2, -0.2, 0, 0, 0);
    drawTriangle(0, -0.3, 0.1, 0, -0.2, 0.2, 0, 0, 0);

    drawTriangle(0, -0.2, -0.2, 0, 0, -0.3, 0, 0, 0);
    drawTriangle(0, -0.2, 0.2, 0, 0, 0.3, 0, 0, 0);
    glPopMatrix();
}




void draw_left_wings() 
{
    //left wing
    //first y-axis feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(3,0.2,0);
    glRotated(90,1,0,0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 3, 3);
    glPopMatrix();

    //second feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(10,0,1,0);
    glTranslated(2.8,0.2,0);
    glRotated(90,1,0,0);
    glScaled(1,0.2,1);
    drawCylinder(0.2,2.8,2.8);
    glPopMatrix();

    //third feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(20, 0, 1, 0);
    glTranslated(2.6, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.6, 2.6);
    glPopMatrix();

    //4th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(30, 0, 1, 0);
    glTranslated(2.4, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.4, 2.4);
    glPopMatrix();

    //5th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(40, 0, 1, 0);
    glTranslated(2.2, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.2, 2.2);
    glPopMatrix();

    //6th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(50, 0, 1, 0);
    glTranslated(2, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2, 2);
    glPopMatrix();
    

}

void draw_right_wings() 
{
    //right wing
    //first y-axis feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-3, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 3, 3);
    glPopMatrix();

    //2nd feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-10, 0, 1, 0);
    glTranslated(-2.8, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.8, 2.8);
    glPopMatrix();

    //3rd feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-20, 0, 1, 0);
    glTranslated(-2.6, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.6, 2.6);
    glPopMatrix();

    //4th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-30, 0, 1, 0);
    glTranslated(-2.4, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.4, 2.4);
    glPopMatrix();

    //5th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-40, 0, 1, 0);
    glTranslated(-2.2, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2.2, 2.2);
    glPopMatrix();

    //6th feather
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-50, 0, 1, 0);
    glTranslated(-2, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.2, 2, 2);
    glPopMatrix();
    
}

void draw_body() 
{
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(-20, 1, 0, 0);
    glScaled(1, 1, 1.2);
    drawSphere(2);
    glPopMatrix();
}


void draw_leftupperLeg()
{
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    drawCylinder(1.4, 0.2, 0.5);
    glPopMatrix();
}

void draw_rightupperLeg()
{
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    drawCylinder(1.4, 0.2, 0.5);
    glPopMatrix();
}


void draw_leftlowerLeg()
{
    glPushMatrix();
    setDiffuseColor(VAL(LEG_R), VAL(LEG_G), VAL(LEG_B));
    drawCylinder(1.3, 0.15, 0.1);
    glPopMatrix();
}

void draw_rightlowerLeg()
{
    glPushMatrix();
    setDiffuseColor(VAL(LEG_R), VAL(LEG_G), VAL(LEG_B));
    drawCylinder(1.3, 0.15, 0.1);
    glPopMatrix();
}

void draw_righttoes()
{
    glPushMatrix();
    glRotated(20- VAL(RIGHT_TOES), 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30 , 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30 , 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30, 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(110, 0, 1, 0);
    drawCylinder(0.5, 0.1, 0.08);

    glPopMatrix();
}

void draw_lefttoes()
{
    glPushMatrix();
    glRotated(20 - VAL(LEFT_TOES), 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30, 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30, 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(30, 0, 1, 0);
    drawCylinder(0.9, 0.1, 0.08);
    glRotated(110, 0, 1, 0);
    drawCylinder(0.5, 0.1, 0.08);

    glPopMatrix();
}

void draw_tail()
{
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(90 + 7, 0, 1, 0);
    glTranslated(1, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.3, 1, 1);
    glPopMatrix();

    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(90 + 20, 0, 1, 0);
    glTranslated(0.8, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.3, 0.8, 0.8);
    glPopMatrix();

    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(90 - 7, 0, 1, 0);
    glTranslated(1, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.3, 1, 1);
    glPopMatrix();

    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glRotated(90 - 20, 0, 1, 0);
    glTranslated(0.8, 0.2, 0);
    glRotated(90, 1, 0, 0);
    glScaled(1, 0.2, 1);
    drawCylinder(0.3, 0.8, 0.8);
    glPopMatrix();
}

void draw_connection()
{
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -1.6);
    glRotated(180, 1, 0, 0);
    drawCylinder(1, 1.5, 1.1);
    glPopMatrix();

    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -1.6 - 1);
    glRotated(180, 1, 0, 0);
    drawCylinder(1, 1.1, 0.6);
    glPopMatrix();

    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -1.6 - 2);
    glRotated(180, 1, 0, 0);
    drawCylinder(1, 0.6, 0.2);
    glPopMatrix();
}


void draw_constrain()
{


    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);
    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    //tail
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -4.5);
    drawSphere(0.2);
    glRotated(VAL(TAIL), 1.0, 0.0, 0);
    draw_tail();
    glPopMatrix();


    //rightupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_RIGHT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawSphere(0.2);
    draw_rightupperLeg();
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_RIGHT_LEG), 1.0, 0.0, 0);
    draw_rightlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_righttoes();
    glPopMatrix();

    //lefttupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_LEFT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    draw_leftupperLeg();
    drawSphere(0.2);
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_LEFT_LEG), 1.0, 0.0, 0.0);
    draw_leftlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_lefttoes();
    glPopMatrix();


}


void draw_level0_animation()
{


    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    if (head_angle == 70)
    {
        head_flag = true;
    }
    if (head_angle == -70)
    {
        head_flag = false;
    }
    if (head_flag == false)
    { 
        head_angle++;
        int some = head_angle / 2;
        glRotated(some, 0.0, 0.0, 1); 
    }
    else 
    { 
        head_angle--;
        int some = head_angle / 2;
        glRotated(some, 0.0, 0.0, 1);
    }
    draw_Head();

    glTranslated(1.35, 0, 0);
    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    if (left_wing_angle == 55)
    {
        left_wing_flag = true;
    }
    if (left_wing_angle == -55)
    {
        left_wing_flag = false;
    }
    if (left_wing_flag == false) { glRotated(left_wing_angle++, 0.0, 0.0, 1); }
    else { glRotated(left_wing_angle--, 0.0, 0.0, 1); }
    
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    if (right_wing_angle == 55)
    {
        right_wing_flag = true;
    }
    if (right_wing_angle == -55)
    {
        right_wing_flag = false;
    }
    if (right_wing_flag == false) { glRotated(-(right_wing_angle++), 0.0, 0.0, 1); }
    else { glRotated(-(right_wing_angle--), 0.0, 0.0, 1); }
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    //tail
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -4.5);
    drawSphere(0.2);
    glRotated(VAL(TAIL), 1.0, 0.0, 0);
    if (tail_angle == 70)
    {
        tail_flag = true;
    }
    if (tail_angle == -70)
    {
        tail_flag = false;
    }
    if (tail_flag == false)
    {
        tail_angle++;
        int some = tail_angle / 2;
        glRotated(some, 1.0, 0.0, 0);
    }
    else
    {
        tail_angle--;
        int some = tail_angle / 2;
        glRotated(some, 1.0, 0.0, 0);
    }
    draw_tail();
    glPopMatrix();


    //rightupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_RIGHT_LEG), 1.0, 0.0, 0);
    if (right_leg_angle == 70)
    {
        right_leg_flag = true;
    }
    if (right_leg_angle == -70)
    {
        right_leg_flag = false;
    }
    if (right_leg_flag == false)
    {
        right_leg_angle++;
        int some = right_leg_angle / 2;
        glRotated(-some, 1.0, 0.0, 0);
    }
    else
    {
        right_leg_angle--;
        int some = right_leg_angle / 2;
        glRotated(-some, 1.0, 0.0, 0);
    }
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawSphere(0.2);
    draw_rightupperLeg();
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_RIGHT_LEG), 1.0, 0.0, 0);
    draw_rightlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_righttoes();
    glPopMatrix();

    //lefttupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_LEFT_LEG), 1.0, 0.0, 0);
    if (left_leg_angle == 70)
    {
        left_leg_flag = true;
    }
    if (left_leg_angle == -70)
    {
        left_leg_flag = false;
    }
    if (left_leg_flag == false)
    {
        left_leg_angle++;
        int some = left_leg_angle / 2;
        glRotated(some, 1.0, 0.0, 0);
    }
    else
    {
        left_leg_angle--;
        int some = left_leg_angle / 2;
        glRotated(some, 1.0, 0.0, 0);
    }
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    draw_leftupperLeg();
    drawSphere(0.2);
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_LEFT_LEG), 1.0, 0.0, 0.0);
    draw_leftlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_lefttoes();
    glPopMatrix();


}


void draw_level0()
{
    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);
    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    //tail
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -4.5);
    drawSphere(0.2);
    glRotated(VAL(TAIL), 1.0, 0.0, 0);
    draw_tail();
    glPopMatrix();


    //rightupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_RIGHT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawSphere(0.2);
    draw_rightupperLeg();
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_RIGHT_LEG), 1.0, 0.0, 0);
    draw_rightlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_righttoes();
    glPopMatrix();

    //lefttupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_LEFT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    draw_leftupperLeg();
    drawSphere(0.2);
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_LEFT_LEG), 1.0, 0.0, 0.0);
    draw_leftlowerLeg();
    glTranslated(0, -0.1, 1.1);
    glRotated(-20, 1.0, 0.0, 0.0);
    glRotated(-40, 0.0, 1.0, 0.0);
    drawSphere(0.1);
    draw_lefttoes();
    glPopMatrix();
}

void draw_level1()
{

    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);
    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    //tail
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -4.5);
    drawSphere(0.2);
    glRotated(VAL(TAIL), 1.0, 0.0, 0);
    draw_tail();
    glPopMatrix();


    //rightupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_RIGHT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawSphere(0.2);
    draw_rightupperLeg();
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_RIGHT_LEG), 1.0, 0.0, 0);
    draw_rightlowerLeg();
    glPopMatrix();

    //lefttupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_LEFT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    draw_leftupperLeg();
    drawSphere(0.2);
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(VAL(LOWER_LEFT_LEG), 1.0, 0.0, 0.0);
    draw_leftlowerLeg();
    glPopMatrix();


}
void draw_level2()
{
    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);

    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    //tail
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glTranslated(0, 0, -4.5);
    drawSphere(0.2);
    glRotated(VAL(TAIL), 1.0, 0.0, 0);
    draw_tail();
    glPopMatrix();


    //rightupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_RIGHT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawSphere(0.2);
    draw_rightupperLeg();
    glPopMatrix();

    //lefttupper leg
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(-0.9, -1.7, -0.3);
    glRotated(VAL(UPPER_LEFT_LEG), 1.0, 0.0, 0);
    drawSphere(0.55);
    glTranslated(0, -1.5, -0.5);
    glRotated(-70, 1.0, 0.0, 0.0);
    draw_leftupperLeg();
    glPopMatrix();

}
void draw_level3()
{
    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);

    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();


    //left wing
    glPushMatrix();
    glTranslated(1.5, 0, 0);
    glRotated(VAL(LEFT_WING), 0.0, 0.0, 1);
    draw_left_wings();
    glPopMatrix();

    //right wing
    glPushMatrix();
    glTranslated(-1.5, 0, 0);
    glRotated(VAL(RIGHT_WING), 0.0, 0.0, 1);
    draw_right_wings();
    glPopMatrix();

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

    

}
void draw_level4()
{
    //Neck part
    glPushMatrix();
    setDiffuseColor(VAL(BODY_R), VAL(BODY_G), VAL(BODY_B));
    glTranslated(0, 1.5, 0.6);
    glRotated(-70, 1.0, 0.0, 0.0);
    drawCylinder(VAL(NECK_LENTH), 0.7, 0.5);
    glPopMatrix();

    //head
    glPushMatrix();
    glTranslated(0, 2.8, 1.2);
    glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
    glRotated(-90, 0.0, 1.0, 0.0);
    glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
    glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
    draw_Head();

    glTranslated(1.35, 0, 0);

    //upper mouth
    glPushMatrix();
    glRotated(VAL(MOUTHZPOS), 0, 0, 1);
    draw_UpperMouth();
    glPopMatrix();

    //lower mouth
    glPushMatrix();
    glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
    draw_LowerMouth();
    glPopMatrix();

    glPopMatrix();



    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();

    //connection
    glPushMatrix();
    draw_connection();
    glPopMatrix();

   
}

void draw_level5()
{

    //body
    glPushMatrix();
    draw_body();
    glPopMatrix();


}

void inverse_kinematics(double posX,double posY,double posZ) 
{
    double positionX = posX, positionY=posY, positionZ=posZ;
    double kneeCons[3]{0.9,-3.2,-0.8};
    double ankleCons[3]{0.9,-3.3,0.3};
    double error[4]{INT_MAX,INT_MAX,INT_MAX,INT_MAX};

    double len1 = sqrt(1.5*1.5+0.5*0.5);
    double len2 = sqrt(0.1*0.1+1.1*1.1);
    double len3 = 0.9;

    double x = positionX - 0.9;
    double y = positionY - (-1.7);
    double z = positionZ - (-0.3);

    double theta[2];
    double alpha[4];
    double beta[4];

    theta[0] = asin( x / len3);
    theta[1] = (theta[0] > 0) ? (M_PI - theta[0]) : (-M_PI - theta[0]);

    for (int i = 0; i < 2; i++) {
        double y_prime = y;
        double z_prime = z - len3 * cos(theta[i]);

        double temp = acos( (pow(y_prime,2)+pow(z_prime,2)-pow(len1,2)-pow(len2,2)) / (2*len1*len2));
        if (i==0) {
            beta[0] = temp;
            beta[1] = -1 * temp;

            for (int j = 0; j < 2; j++) {
                double coeff1 = z_prime + len2 * y_prime * sin(beta[j]) / (len1 + len2 * cos(beta[j]));
                double coeff2 = len1 + len2 * cos(beta[j]) + pow(len2, 2) * pow(sin(beta[j]), 2) / (len1 + len2 * cos(beta[j]));
                alpha[j] = acos(coeff1 / coeff2);
            }
        }
        else {
            beta[2] = temp;
            beta[3] = -1 * temp;

            for (int j = 2; j < 4; j++) {
                double coeff1 = z_prime + len2 * y_prime * sin(beta[j]) / (len1 + len2 * cos(beta[j]));
                double coeff2 = len1 + len2 * cos(beta[j]) + pow(len2, 2) * pow(sin(beta[j]), 2) / (len1 + len2 * cos(beta[j]));
                alpha[j] = acos(coeff1 / coeff2);
            }
        }
    }

    double min_error = error[0];
    int index = -1;
    
    for (int i = 0; i < 4; i++) {
        double real_knee[3];
        real_knee[0] = 0;
        real_knee[1] = len1 * sin(alpha[i]);
        real_knee[2] = len1 * cos(alpha[i]);

        double real_ankle[3];
        real_ankle[0] = 0;
        real_ankle[1] = y;
        real_ankle[2] = z - len3 * cos(theta[i/2]);

        error[i] = pow(real_knee[0] - kneeCons[0], 2) + pow(real_knee[1] - kneeCons[1], 2) + pow(real_knee[2] - kneeCons[2], 2)
            + pow(real_ankle[0] - ankleCons[0], 2) + pow(real_ankle[1] - ankleCons[1], 2) + pow(real_ankle[2] - ankleCons[2], 2);

        if (min_error > error[i]) {
            min_error = error[i];
            index = i;
        }
    }
    
    if (index != -1) {
        double temp0 = atan(1.5 / 0.5) * 180 / M_PI;
        //cout << temp0 << ' ';
        double temp1 = -1*alpha[index]*180/M_PI - (180-temp0);
        //cout << "alpha: " << -1 * alpha[index] * 180 / M_PI << ' ';
        ModelerApplication::Instance()->SetControlValue(UPPER_RIGHT_LEG,temp1);
        //cout << "upper: " << temp1;
        temp0 = atan(0.1 / 1.1) * 180 / M_PI + temp0;
        //cout <<" hh "<< temp0 << ' ';
        temp1 = 180 - temp0 - beta[index]*180/M_PI;
        //cout << "beta: " << -1*beta[index] * 180 / M_PI;
        ModelerApplication::Instance()->SetControlValue(LOWER_RIGHT_LEG,temp1 );
        //cout << " lower: " << temp1;
        temp1 = 0;
        temp1 = 10 - theta[index/2] * 180 / M_PI;
        ModelerApplication::Instance()->SetControlValue(RIGHT_TOES, temp1);
        //cout << " toe: " << temp1<<endl;
    }
    
}