#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{ 
	XPOS, YPOS, ZPOS, HEADYPOS, HEADZPOS, MOUTHZPOS, NECK_LENTH, UPPER_RIGHT_LEG,
	UPPER_LEFT_LEG, LOWER_RIGHT_LEG, LOWER_LEFT_LEG, LEFT_TOES, RIGHT_TOES, RIGHT_WING,
	LEFT_WING, TAIL, DEFAULT_LIGHT_R, DEFAULT_LIGHT_G, DEFAULT_LIGHT_B, BODY_R, BODY_G, BODY_B,
	MOUTH_R, MOUTH_G, MOUTH_B, LEG_R, LEG_G, LEG_B, EYEBROW_R, EYEBROW_G, EYEBROW_B, LEVEL, TURNONLIGHT, LIGHTX, LIGHTY, LIGHTZ, 
	TARGETX, TARGETY,TARGETZ, IK_ON,
	FRAME_ALL, CAM_XPOS, CAM_YPOS, CAM_ZPOS, MODE_ON, MODE, NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

#endif