// modelerdraw.h

// Contains object and routines related to rendering things

#ifndef MODELERDRAW_H
#define MODELERDRAW_H

#include <FL/gl.h>
#include <cstdio>

#include "modelerglobals.h"

class point
{
public:
	point() {};
	point(float a, float b, float c) 
	{
		x = a;
		y = b;
		z = c;
	};

	float x;
	float y;
	float z;
};

class curve
{

public:
	curve(int a) 
	{
		point_num = a;
		points = new point[point_num];
	}
	
	int point_num;
	point* points;
};



enum DrawModeSetting_t 
{ NONE=0, NORMAL, WIREFRAME, FLATSHADE, };

enum QualitySetting_t 
{ HIGH, MEDIUM, LOW, POOR, };

// Ignore this; the ModelerDrawState just keeps 
// information about the current color, etc, etc.
class ModelerDrawState
{
public: 

	static ModelerDrawState* Instance();

	FILE* m_rayFile;

	DrawModeSetting_t m_drawMode;
	QualitySetting_t  m_quality;

	GLfloat m_ambientColor[4];
	GLfloat m_diffuseColor[4];
	GLfloat m_specularColor[4];
	GLfloat m_shininess;

private:
	ModelerDrawState();
	ModelerDrawState(const ModelerDrawState &) {}
	ModelerDrawState& operator=(const ModelerDrawState&) {}

	static ModelerDrawState *m_instance;
};

// ****************************************************************************
// DRAWING FUNCTIONS
//
// The following functions are for your use in Modeler.  You don't *need* to
// use these functions; however, if you desire to output your model for
// the raytracer project (in .ray file format), you must either call these
// functions or implement the appropriate functionality so that the raytracer
// can handle it.
//
// Note:  Depending on whether a ray file is open or closed, these functions
//        will either output to a ray file or make OpenGL calls.
// ****************************************************************************

// Set the current material properties
void setAmbientColor(float r, float g, float b);
void setDiffuseColor(float r, float g, float b);
void setSpecularColor(float r, float g, float b);
void setShininess(float s);

// Set the current draw mode (see DrawModeSetting_t for valid values
void setDrawMode(DrawModeSetting_t drawMode);

// Set the current quality mode (See QualityModeSetting_t for valid values
void setQuality(QualitySetting_t quality);

// Opens a .ray file for writing, returns false on error
bool openRayFile(const char rayFileName[]);
// Closes the current .ray file if one exists
void closeRayFile();

/////////////////////////////
// Raytraceable Primitives //
/////////////////////////////

// Draw a sphere of radius r
void drawSphere(double r);

// Draw an axis-aligned box from origin to (x,y,z)
void drawBox( double x, double y, double z );

// Draw an axis-aligned texture box from origin to (x,y,z)
void drawTextureBox( double x, double y, double z );

// Draw a cylinder from z=0 to z=h with radius r1 at origin and r2 at z=h
void drawCylinder( double h, double r1, double r2 );

// Driangle with three given vertices.  Specify in counterclockwise direction
void drawTriangle( double x1, double y1, double z1,
			       double x2, double y2, double z2,
			       double x3, double y3, double z3 );

void draw_Head();
void draw_UpperMouth();
void draw_LowerMouth();
void draw_left_wings();
void draw_right_wings();
void draw_body();
void draw_tail();
void draw_connection();
void draw_rightupperLeg();
void draw_leftlowerLeg();
void draw_leftupperLeg();
void draw_rightlowerLeg();
void draw_righttoes();
void draw_lefttoes();

void draw_level0();
void draw_level1();
void draw_level2();
void draw_level3();
void draw_level4();
void draw_level5();
void draw_level0_animation();
void inverse_kinematics(double, double, double);
void draw_flower();

void draw_mode0();
void draw_mode1();
void draw_mode2();
void draw_mode3();
void draw_mode4();

void draw_righteyebrow();
void draw_lefteyebrow();
void draw_surface(curve a, curve b);
void draw_pixl();
void draw_pixl1();
float calculate_point_distance(point a, point b);

#endif