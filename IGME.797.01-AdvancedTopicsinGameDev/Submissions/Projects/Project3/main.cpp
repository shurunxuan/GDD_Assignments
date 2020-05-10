#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/constants.hpp>

#include "Camera.h"
#include "Text.h"
#include "Mesh.h"
#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth = 2560;
int g_winHeight = 1440;

Camera g_cam;
Text g_text;
unsigned char g_keyStates[256];

GLfloat light0_pos[] = { 0.0f, 5.0f, 5.0f, 0.0f };
GLfloat light0_Amb[] = { 0.4f, 0.3f, 0.3f, 1.0f };
GLfloat light0_Diff[] = { 0.8f, 0.8f, 0.7f, 1.0f };
GLfloat light0_Spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };

const char meshFile[128] = "Mesh/bunny2k.obj";
Mesh mesh;


void initialization()
{
	g_cam.set(3.0f, 4.0f, 3.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight);

	g_text.setColor(0.0f, 0.0f, 0.0f);

	// loading the input mesh and setting its properties
	mesh.loadFromOBJFile(meshFile);
	mesh.diff = vec3(0.5f, 0.5f, 0.2f);

	// add any stuff you want to initialize ...
	mesh.calculateCollapseVertex();

}

/****** GL callbacks ******/
void initialGL()
{
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_Amb);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_Diff);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_Spec);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_FILL);


	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
	// add any stuff to update at runtime ....

	g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);
}

void display()
{
	//glFinish();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); // commenting out this line to make object always lit up in front of the cam. 

	// adding any stuff you'd like to draw
	mesh.draw();

	// displaying the camera
	g_cam.drawGrid();
	g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
	g_cam.drawCoordinate();

	// displaying the text
	if (g_cam.isFocusMode()) {
		string str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}
	else if (g_cam.isFPMode()) {
		string str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}

	char s[128];
	sprintf_s(s, "vNum:%d, tNum:%d", mesh.vertices.size(), mesh.triangles.size());
	g_text.draw(10, 50, s, g_winWidth, g_winHeight);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProj(g_winWidth, g_winHeight);
	g_cam.setModelView();
	glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
	g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);

}

void motion(int x, int y)
{
	g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
}

void keyup(unsigned char key, int x, int y)
{
	g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
	g_keyStates[key] = true;
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'c': // switch cam control mode
		g_cam.switchCamMode();
		glutPostRedisplay();
		break;
	case ' ':
		g_cam.PrintProperty();
		break;

	case 'v': // change view mode of mesh

		if (mesh.viewMode == WIREFRAME)
			mesh.viewMode = SHADED;
		else if (mesh.viewMode == SHADED)
			mesh.viewMode = WIREFRAME;
		glutPostRedisplay();
		break;

	case'p': // perform a single step of mesh simplification 
		// add your code here ...
		mesh.collapse();
		mesh.calculateCollapseVertex();
		glutPostRedisplay();
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(g_winWidth, g_winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("IGME.797.01 Project 3        Author: Victor Shu");

	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardUpFunc(keyup);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initialization();

	glutMainLoop();
	return EXIT_SUCCESS;
}