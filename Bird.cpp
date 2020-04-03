// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include "modelerglobals.h"
#include "bitmap.h"
#include "modelerui.h"



// To make a BirdModel, we inherit off of ModelerView
class BirdModel : public ModelerView
{
private:
	int textureWidth;
	int textureHeight;
	GLuint textureName;
	GLubyte texture[256][256][4];
	bool isTextureLoaded;



public:
	BirdModel(int x, int y, int w, int h, char* label)
		: ModelerView(x, y, w, h, label) 
	{
		isTextureLoaded = false;
	}

	

	virtual void draw();
	void initTexture();
	void drawTexture();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createBirdModel(int x, int y, int w, int h, char* label)
{
	return new BirdModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out BirdModel
void BirdModel::draw()
{
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	
	// Interpolated values from the vertex shaders



	if (VAL(TURNONLIGHT)) {
		GLfloat pos[4];
		pos[0] = VAL(LIGHTX);
		pos[1] = VAL(LIGHTY);
		pos[2] = VAL(LIGHTZ);
		pos[3] = 0.0f;
		glLightfv(GL_LIGHT1, GL_POSITION, pos);
		glPushMatrix();
		glTranslated(VAL(LIGHTX), VAL(LIGHTY), VAL(LIGHTZ));
		setDiffuseColor(1.0f, 1.0f, 1.0f);
		drawBox(0.2, 0.2, 0.2);
		glPopMatrix();
	}
	

	int isAnimationOn = ModelerUserInterface::m_controlsAnimOnMenu->value();

	glPushMatrix();
	//test
	point p1(2, 2, 3 );
	point p2( 2.5, 2, 3);
	point p3 = { 2.6, 2.1, 3 };
	point p4 = { 2.8, 2.3, 3 };
	point p5 = { 3, 2, 3 };
	point p6 = { 3.1, 2.1, 3 };
	point p7 = { 3.2, 2, 3 };


	point p8 = { 2, 2, 3.1 };
	point p9 = { 2.1, 2, 3.3 };
	point p10 = { 2.2, 2, 3.4 };
	point p11= { 2.3, 2, 3.6 };
	point p12 = { 2.2, 2, 4 };

	//curve c1 = {new point[5],5 };

	//curve c2= { new point[7],7 };
	curve c1(5);
	curve c2(7);

	c1.points[0] = p8;
	c1.points[1] = p9;
	c1.points[2] = p10;
	c1.points[3] = p11;
	c1.points[4] = p12;

	c2.points[0] = p1;
	c2.points[1] = p2;
	c2.points[2] = p3;
	c2.points[3] = p4;
	c2.points[4] = p5;
	c2.points[3] = p6;
	c2.points[4] = p7;



	glScaled(3, 3, 3);
	draw_surface(c1, c2);

	glTranslated(4, 2, 3);

	glScaled(15, 15, 15);
	draw_pixl();
	draw_pixl1();

	glPopMatrix();

	// draw the floor
	float R = VAL(DEFAULT_LIGHT_R) / 255;
	float G = VAL(DEFAULT_LIGHT_G) / 255;
	float B = VAL(DEFAULT_LIGHT_B) / 255;

	setAmbientColor(R,G,B);
	setDiffuseColor(0.42,0.56,0.14);
	glPushMatrix();
	glTranslated(-7.5, 0, -7.5);
	drawBox(15, 0.1f, 15);
	glPopMatrix();
	

	if (VAL(MODE_ON) == 1)
	{
		int mode = VAL(MODE);
		switch (mode)
		{
		case 0:
			draw_mode0();
			break;
		case 1:
			draw_mode1();
			break;
		case 2:
			draw_mode2();
			break;
		case 3:
			draw_mode3();
			break;
		case 4:
			draw_mode4();
			break;

		}
	}
	else
	{
		if (VAL(LEVEL) == 0)
		{
			//draw_constrain
			/*initTexture();
			drawTexture();*/
			if (isAnimationOn) { draw_level0_animation(); }
			else {
				draw_level0();
				//draw_mode4();
			}
		}
		else if (VAL(LEVEL) == 1)
		{
			draw_level1();
		}
		else if (VAL(LEVEL) == 2)
		{
			draw_level2();
		}
		else if (VAL(LEVEL) == 3)
		{
			draw_level3();
		}
		else if (VAL(LEVEL) == 4)
		{
			draw_level4();
		}
		else if (VAL(LEVEL) == 5)
		{
			draw_level5();
		}
	}

	glPopMatrix();
}


int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	
	controls[XPOS] = ModelerControl("Move X", -5, 5, 1, 0);
	controls[YPOS] = ModelerControl("Move Y", 0, 5, 1, 0);
	controls[ZPOS] = ModelerControl("Move Z", -5, 5, 1, 0);

	controls[HEADYPOS] = ModelerControl("Head Y Rotate", -20, 20, 1, 0);
	controls[HEADZPOS] = ModelerControl("Head X Rotate", -20, 20, 1, 0);
	controls[MOUTHZPOS] = ModelerControl("Mouth Rotate", 0, 40, 1, 0);
	controls[NECK_LENTH] = ModelerControl("Neck Length", 1, 1.6, 0.1f, 1);

	controls[UPPER_RIGHT_LEG] = ModelerControl("upper right leg", -20, 20, 1, 0);
	controls[UPPER_LEFT_LEG] = ModelerControl("upper left leg", -20, 20, 1, 0);
	controls[LOWER_RIGHT_LEG] = ModelerControl("lower right leg", -20, 20, 1, 0);
	controls[LOWER_LEFT_LEG] = ModelerControl("lower left leg", -20, 20, 1, 0);
	controls[LEFT_TOES] = ModelerControl("left toes", -20, 20, 1, 0);
	controls[RIGHT_TOES] = ModelerControl("right toes", -20, 20, 1, 0);

	controls[RIGHT_WING] = ModelerControl("right wing", -20, 20, 1, 0);
	controls[LEFT_WING] = ModelerControl("left wing", -20, 20, 1, 0);

	controls[TAIL] = ModelerControl("tail", -20, 20, 1, 0);

	controls[DEFAULT_LIGHT_R] = ModelerControl("Default Light R", 0, 255, 1, 25.5);
	controls[DEFAULT_LIGHT_G] = ModelerControl("Default Light G", 0, 255, 1, 25.5);
	controls[DEFAULT_LIGHT_B] = ModelerControl("Default Light B", 0, 255, 1, 25.5);

	//1, 0.5, 0
	controls[BODY_R] = ModelerControl("Body R", 0, 1, 0.01, 1);
	controls[BODY_G] = ModelerControl("Body G", 0, 1, 0.01, 0.5);
	controls[BODY_B] = ModelerControl("Body B", 0, 1, 0.01, 0);

	//0.33, 0.55, 0.33
	controls[MOUTH_R] = ModelerControl("Mouth R", 0, 1, 0.01, 0.33);
	controls[MOUTH_G] = ModelerControl("Mouth G", 0, 1, 0.01, 0.55);
	controls[MOUTH_B] = ModelerControl("Mouth B", 0, 1, 0.01, 0.33);

	//0.56, 0.11, 0
	controls[LEG_R] = ModelerControl("Leg R", 0, 1, 0.01, 0.56);
	controls[LEG_G] = ModelerControl("Leg G", 0, 1, 0.01, 0.11);
	controls[LEG_B] = ModelerControl("Leg B", 0, 1, 0.01, 0.0);

	controls[EYEBROW_R] = ModelerControl("Eyebrows R", 0, 1, 0.01, 0.55);
	controls[EYEBROW_G] = ModelerControl("Eyebrows G", 0, 1, 0.01, 0.27);
	controls[EYEBROW_B] = ModelerControl("Eyebrows B", 0, 1, 0.01, 0.07);

	controls[LEVEL] = ModelerControl("Body Level", 0, 5, 1, 0.0);

	controls[TURNONLIGHT] = ModelerControl("Custom Light Source", 0, 1, 1, 0);
	controls[LIGHTX] = ModelerControl("Light X", -5, 5, 0.1f, 0);
	controls[LIGHTY] = ModelerControl("Light Y", 0, 5, 0.1f, 0);
	controls[LIGHTZ] = ModelerControl("Light Z", -5, 5, 0.1f, 0);

	controls[FRAME_ALL] = ModelerControl("Fram all", 0, 1, 1, 0);

	controls[CAM_XPOS] = ModelerControl("Camera Move X", -20, 20, 1, 0);
	controls[CAM_YPOS] = ModelerControl("Camera Move Y", -20, 20, 1, 0);
	controls[CAM_ZPOS] = ModelerControl("Camera Move Z", -20, 20, 1, 0);


	controls[MODE_ON] = ModelerControl("Mode on", 0, 1, 1, 0.0);
	controls[MODE] = ModelerControl("Mode", 0, 4, 1, 0.0);



	ModelerApplication::Instance()->Init(&createBirdModel, controls, NUMCONTROLS);
	
	
	

	return ModelerApplication::Instance()->Run();
}


void BirdModel::initTexture() {

	/*texture* some = new texture;
	GLuint Texture = some->loadBMP_custom("./my_texture.bmp");*/
	if (!isTextureLoaded) {
		char* name = "./my_texture.bmp";

		unsigned char* temp = readBMP(name, textureWidth, textureHeight);
		/*
		texture = new GLubyte**[textureHeight];
		for (int i = 0; i < textureHeight; i++) {
			texture[i] = new GLubyte*[textureWidth];
			for (int j = 0; j < textureWidth; j++)
				texture[i][j] = new GLubyte[4];
		}
		*/
		//cout << "width:" << textureWidth << ",height:" << textureHeight;
		for (int row = 0; row < textureHeight; row++) {
			for (int col = 0; col < textureWidth; col++) {
				for (int i = 0; i < 3; i++)
					texture[row][col][i] = temp[(row * textureWidth + col) * 3 + i];
				//cout << "("<<texture[row][col][0] << "," << texture[row][col][1] << "," << texture[row][col][2]<<")";
				texture[row][col][3] = 255;
			}
		}
	}

	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &textureName);
	glBindTexture(GL_TEXTURE_2D, textureName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, textureName);

}

void BirdModel::drawTexture() {


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 0.0, 0.0);


	glEnd();

	/*
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5, 0.5); glVertex3f(0.0, 0.0, 0.0);
	glEnd();
	glFlush();
	*/
	glDisable(GL_TEXTURE_2D);

}