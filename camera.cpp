#include <windows.h>
#include <Fl/gl.h>
#include <gl/glu.h>
#include <math.h>
#include "camera.h"
#include "modelerglobals.h"
#include "modelerapp.h"

#include <iostream>
using namespace std;

#pragma warning(push)
#pragma warning(disable : 4244)

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif 

const float kMouseRotationSensitivity		= 1.0f/90.0f;
const float kMouseTranslationXSensitivity	= 0.03f;
const float kMouseTranslationYSensitivity	= 0.03f;
const float kMouseZoomSensitivity			= 0.08f;

void MakeDiagonal(Mat4f &m, float k)
{
	register int i,j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			m[i][j] = (i==j) ? k : 0.0f;
}

void MakeHScale(Mat4f &m, const Vec3f &s)	
{
	MakeDiagonal(m,1.0f);
	m[0][0] = s[0]; m[1][1] = s[1];	m[2][2] = s[2];
}

void MakeHTrans(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m,1.0f);
	m[0][3] = s[0]; m[1][3] = s[1]; m[2][3] = s[2];
}

void MakeHRotX(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[1][1] = cosTheta;
	m[1][2] = -sinTheta;
	m[2][1] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotY(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[2][0] = -sinTheta;
	m[0][2] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotZ(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[0][1] = -sinTheta;
	m[1][0] = sinTheta;
	m[1][1] = cosTheta;
}



void Camera::calculateViewingTransformParameters() 
{
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	MakeHTrans(dollyXform, Vec3f(0,0,mDolly));
	MakeHRotY(azimXform, mAzimuth);
	MakeHRotX(elevXform, mElevation);
	MakeDiagonal(twistXform, 1.0f);
	MakeHTrans(originXform, mLookAt);
	
	//mPosition = Vec3f(0, 8, -10);
	mPosition = Vec3f(0, 0, 0);

	// grouped for (mat4 * vec3) ops instead of (mat4 * mat4) ops
	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));

	if ( fmod((double)mElevation, 2.0*M_PI) < 3*M_PI/2 && fmod((double)mElevation, 2.0*M_PI) > M_PI/2 )
		mUpVector= Vec3f(0,-1,0);
	else
		mUpVector= Vec3f(0,1,0);

	mDirtyTransform = false;
}

Camera::Camera() 
{
	mElevation = mAzimuth = mTwist = 0.0f;
	mDolly = -20.0f;
	mElevation = 0.2f;
	mAzimuth = (float)M_PI;
	mTwist = 0;

	//mLookAt = Vec3f( 0, 3, 0 );
	mLookAt = Vec3f(0, 0, 0);
	mCurrentMouseAction = kActionNone;

	calculateViewingTransformParameters();
}

void Camera::clickMouse( MouseAction_t action, int x, int y )
{
	mCurrentMouseAction = action;
	mLastMousePosition[0] = x;
	mLastMousePosition[1] = y;
}

void Camera::wheelMove(int dy)
{
	mTwist = 5.0 * dy * M_PI / 180;
	Vec3f forward = mLookAt - mPosition;
	forward.normalize();
	float x = forward[0];
	float y = forward[1];
	float z = forward[2];
	double c = cos(mTwist);
	double s = sin(mTwist);
	Mat3f rotation(1, x * y * (1 - c) - z * s, x * z * (1 - c) + y * s,
		y * x * (1 - c) + z * s, 1, y * z * (1 - c) - x * s,
		x * z * (1 - c) - y * s, y * z * (1 - c) + x * s, 1
		);
	//cout << "original mUpVector: " << mUpVector[0] << ' ' << mUpVector[1] << ' ' << mUpVector[2] << endl;
	mUpVector = rotation * mUpVector;
	//cout << "mUpVector: " << mUpVector[0] << ' ' << mUpVector[1] << ' ' << mUpVector[2] << endl;
}

void Camera::dragMouse( int x, int y )
{
	Vec3f mouseDelta   = Vec3f(x,y,0.0f) - mLastMousePosition;
	mLastMousePosition = Vec3f(x,y,0.0f);

	switch(mCurrentMouseAction)
	{
	case kActionTranslate:
		{
			calculateViewingTransformParameters();

			double xTrack =  -mouseDelta[0] * kMouseTranslationXSensitivity;
			double yTrack =  mouseDelta[1] * kMouseTranslationYSensitivity;

			Vec3f transXAxis = mUpVector ^ (mPosition - mLookAt);
			transXAxis /= sqrt((transXAxis*transXAxis));
			Vec3f transYAxis = (mPosition - mLookAt) ^ transXAxis;
			transYAxis /= sqrt((transYAxis*transYAxis));

			setLookAt(getLookAt() + transXAxis*xTrack + transYAxis*yTrack);
			
			break;
		}
	case kActionRotate:
		{
			float dAzimuth		=   -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation	=   mouseDelta[1] * kMouseRotationSensitivity;
			
			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);
			
			break;
		}
	case kActionZoom:
		{
			float dDolly = -mouseDelta[1] * kMouseZoomSensitivity;
			setDolly(getDolly() + dDolly);
			break;
		}
	case kActionTwist:
		// Not implemented
	default:
		break;
	}

}

void Camera::releaseMouse( int x, int y )
{
	mCurrentMouseAction = kActionNone;
}


void Camera::applyViewingTransform() {
	if( mDirtyTransform )
		calculateViewingTransformParameters();

	// Place the camera at mPosition, aim the camera at
	// mLookAt, and twist the camera such that mUpVector is up
	/*
	gluLookAt(	mPosition[0], mPosition[1], mPosition[2],
				mLookAt[0],   mLookAt[1],   mLookAt[2],
				mUpVector[0], mUpVector[1], mUpVector[2]);
	*/
	lookAt(mPosition,mLookAt, mUpVector);
}

void Camera::lookAt(Vec3f eye, Vec3f at, Vec3f up) 
{
	GLdouble temp[16];
	Vec3d forward(at[0] - eye[0], at[1] - eye[1], at[2] - eye[2]);
	forward.normalize();
	Vec3d upward(up[0],up[1],up[2]);
	upward.normalize();
	Vec3d x_axis(forward[1]*upward[2]-upward[1]*forward[2], forward[2]*upward[0]-upward[2]*forward[0], forward[0]*upward[1]-upward[0]*forward[1]);
	x_axis.normalize();

	upward[0] = x_axis[1]*forward[2]-forward[1]*x_axis[2];
	upward[1] = x_axis[2] * forward[0] - forward[2] * x_axis[0];
	upward[2] = x_axis[0] * forward[1] - forward[0] * x_axis[1];


	temp[0] = x_axis[0];
	temp[4] = x_axis[1];
	temp[8] = x_axis[2];
	temp[12] = 0;

	temp[1] = upward[0];
	temp[5] = upward[1];
	temp[9] = upward[2];
	temp[13] = 0;

	temp[2] = 0- forward[0];
	temp[6] = 0- forward[1];
	temp[10] = 0- forward[2];
	temp[14] = 0;

	temp[3] = 0;
	temp[7] = 0;
	temp[11] = 0;
	temp[15] = 1;

	glLoadIdentity();
	glMultMatrixd(temp);
	glTranslated(-1*eye[0], -1*eye[1], -1*eye[2]);
}

void Camera::set_Camera_Position(float x, float y, float z)
{
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	MakeHTrans(dollyXform, Vec3f(0, 0, mDolly));
	MakeHRotY(azimXform, mAzimuth);
	MakeHRotX(elevXform, mElevation);
	MakeDiagonal(twistXform, 1.0f);
	MakeHTrans(originXform, mLookAt);


	mPosition = Vec3f(x, y, z);
	

	// grouped for (mat4 * vec3) ops instead of (mat4 * mat4) ops
	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));
	/*
	if (fmod((double)mElevation, 2.0 * M_PI) < 3 * M_PI / 2 && fmod((double)mElevation, 2.0 * M_PI) > M_PI / 2)
		mUpVector = Vec3f(0, -1, 0);
	else
		mUpVector = Vec3f(0, 1, 0);
	*/
	mDirtyTransform = false;
}

#pragma warning(pop)