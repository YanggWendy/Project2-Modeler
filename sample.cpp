// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "modelerglobals.h"

// To make a SampleModel, we inherit off of ModelerView
class SampleModel : public ModelerView
{
public:
	SampleModel(int x, int y, int w, int h, char* label)
		: ModelerView(x, y, w, h, label) { }

	virtual void draw();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char* label)
{
	return new SampleModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void SampleModel::draw()
{
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	// draw the floor
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-5, 0, -5);
	drawBox(10, 0.01f, 10);
	glPopMatrix();

	//draw the sample model
	/*setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(COLOR_GREEN);
	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

		glPushMatrix();
		glTranslated(-1.5, 0, -2);
		glScaled(3, 1, 4);
		drawBox(1,1,1);
		glPopMatrix();*/

		// draw cannon
		/*glPushMatrix();
		glTranslated(5, 0, 0);
		glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
		glRotated(-90, 1.0, 0.0, 0.0);
		drawCylinder(VAL(HEIGHT), 0.1, 0.1);

		glTranslated(0.0, 0.0, VAL(HEIGHT));
		drawCylinder(1, 1.0, 0.9);

		glTranslated(0.0, 0.0, 0.5);
		glRotated(90, 1.0, 0.0, 0.0);
		drawCylinder(4, 0.1, 0.2);
		glPopMatrix();


	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(0.6,0.8,0);
	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));*/


	//Neck part
	glPushMatrix();
	setDiffuseColor(0.93, 0.46, 0);
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
	glTranslated(0, 2.8, 1.2);
	glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
	glRotated(-90, 0.0, 1.0, 0.0);
	//upper mouth
	glPushMatrix();
	glRotated(VAL(MOUTHZPOS), 1, 0, 0);
	glRotated(90, 0.0, 1.0, 0.0);
	glTranslated(0, -VAL(NECK_LENTH) + 1, -0.2 * VAL(NECK_LENTH));
	glTranslated(0, -2.8, -1.2);
	draw_UpperMouth();
	glPopMatrix();

	//lower mouth
	glPushMatrix();
	glRotated(-VAL(MOUTHZPOS), 1, 0, 0);
	glRotated(90, 0.0, 1.0, 0.0);
	glTranslated(0, -2.8, -1.2);
	glTranslated(0, -VAL(NECK_LENTH) + 1, -0.2 * VAL(NECK_LENTH));
	draw_LowerMouth();
	glPopMatrix();

	glPopMatrix();

	//upper mouth
	/*glPushMatrix();
	glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
	glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
	glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
	glTranslated(0.0, 2.8, 2.6);
	glRotated(-90, 0.0, 1.0, 0.0);
	glRotated(VAL(MOUTHZPOS), 0, 0, 1);
	glScaled(1, 1, 2);
	draw_UpperMouth();
	/*glTranslated(0.4, 0, -0.3);
	glScaled(0.8, 1, 0.7);
	drawSphere(0.4);
	glPopMatrix();

	//lower mouth
	glPushMatrix();
	glRotated(VAL(HEADYPOS), 0.0, 1.0, 0.0);
	glRotated(VAL(HEADZPOS), 0.0, 0.0, 1.0);
	glTranslated(0, VAL(NECK_LENTH) - 1, 0.2 * VAL(NECK_LENTH));
	glTranslated(0.0, 2.8, 2.6);
	glRotated(-90, 0.0, 1.0, 0.0);
	glRotated(-VAL(MOUTHZPOS), 0, 0, 1);
	glScaled(1, 1, 2);
	draw_LowerMouth();
	glPopMatrix();*/

	//left wing
	glPushMatrix();
	glTranslated(1.5, 0, 0);
	draw_left_wings();
	glPopMatrix();

	//right wing
	glPushMatrix();
	glTranslated(-1.5, 0, 0);
	draw_right_wings();
	glPopMatrix();

	//body
	glPushMatrix();
	draw_body();
	glPopMatrix();
	/*
	// draw cannon
	glPushMatrix();
	glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawCylinder(VAL(HEIGHT), 0.1, 0.1);

	glTranslated(0.0, 0.0, VAL(HEIGHT));
	drawCylinder(1, 1.0, 0.9);

	glTranslated(0.0, 0.0, 0.5);
	glRotated(90, 1.0, 0.0, 0.0);
	drawCylinder(4, 0.1, 0.2);
	glPopMatrix();
	*/

	glPopMatrix();
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[HEIGHT] = ModelerControl("Height", 1, 2.5, 0.1f, 1);
	controls[ROTATE] = ModelerControl("Rotate", -135, 135, 1, 0);
	controls[HEADYPOS] = ModelerControl("Head Y Rotate", -20, 20, 1, 0);
	controls[HEADZPOS] = ModelerControl("Head Z Rotate", -20, 20, 1, 0);
	controls[MOUTHZPOS] = ModelerControl("Mouth Rotate", 0, 40, 1, 0);
	controls[NECK_LENTH] = ModelerControl("Neck Length", 1, 1.6, 0.1f, 1);


	ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}
