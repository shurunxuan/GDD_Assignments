
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <vector>

float canvasSize[] = { 10.0f, 10.0f };
int rasterSize[] = { 800, 600 };

float mousePos[2];

enum CommandType
{
	OGL_BEGIN,
	OGL_END,
	OGL_VERTEX_2_F,
	OGL_COLOR_3_F,
	OGL_POINT_SIZE,
	OGL_LINE_WIDTH,
};

union CommandData
{
	char buffer[12];
	float f[3];
	int i[3];
	unsigned int ui[3];

	CommandData()
	{
		memset(buffer, 0, sizeof(char) * 12);
	}
	~CommandData() {}

	CommandData(const CommandData& v)
	{
		memcpy(buffer, v.buffer, sizeof(char) * 12);
	}
};

struct Command
{
	CommandType type;
	CommandData data;

	Command()
	{
		type = OGL_END;
	}
	
	void Play() const;
};

typedef std::vector<Command> CommandBuffer;


#define MENU_CLEAR					0b00000000
#define MENU_OBJECTS				0b00000001
#define MENU_COLORS					0b00000010
#define MENU_SIZES					0b00000011
#define MENU_QUIT					0b00000100
#define MENU_OBJECTS_POINT			0b00001001
#define MENU_OBJECTS_LINE			0b00010001
#define MENU_OBJECTS_TRIANGLE		0b00011001
#define MENU_OBJECTS_QUAD			0b00100001
#define MENU_OBJECTS_POLYGON		0b00101001
#define MENU_COLORS_RED				0b00001010
#define MENU_COLORS_GREEN			0b00010010
#define MENU_COLORS_BLUE			0b00011010
#define MENU_COLORS_BLACK			0b00100010
#define MENU_SIZES_SMALL			0b00001011
#define MENU_SIZES_MEDIUM			0b00010011
#define MENU_SIZES_LARGE			0b00011011

enum PrimitiveType
{
	PRIMITIVE_NULL,
	PRIMITIVE_POINT,
	PRIMITIVE_LINE,
	PRIMITIVE_TRIANGLE,
	PRIMITIVE_QUAD,
	PRIMITIVE_POLYGON
};

PrimitiveType currentPrimitive = PRIMITIVE_NULL;
float currentColor[3] = { 0.0f, 0.0f, 0.0f };
float currentSize = 1.0f;

// For realtime drawing update
// This only contains commands with type of OGL_VERTEX_2_F
CommandBuffer currentCommandBuffer;
// For drawing all finalized shapes
// Contains all types of commands. Commands that configures size / color
// and begin / end are presented for each single finalized shape.
// Thus, this is not fully optimized.
CommandBuffer primaryCommandBuffer;

void Command::Play() const
{
	switch (type)
	{
	case OGL_BEGIN:
		glBegin(data.ui[0]);
		break;
	case OGL_END:
		glEnd();
		break;
	case OGL_VERTEX_2_F:
		glVertex2f(data.f[0], data.f[1]);
		break;
	case OGL_COLOR_3_F:
		glColor3f(data.f[0], data.f[1], data.f[2]);
		break;
	case OGL_POINT_SIZE:
		glPointSize(data.f[0]);
		break;
	case OGL_LINE_WIDTH:
		glLineWidth(data.f[0]);
		break;
	default:;
	}
}

GLenum Primitive(PrimitiveType type)
{
	switch (currentPrimitive)
	{
	case PRIMITIVE_NULL:
	case PRIMITIVE_POINT:
		return GL_POINTS;
	case PRIMITIVE_LINE:
		return GL_LINE_STRIP;
	case PRIMITIVE_TRIANGLE:
		return GL_TRIANGLES;
	case PRIMITIVE_QUAD:
		return GL_QUADS;
	case PRIMITIVE_POLYGON:
		return GL_POLYGON;
	default:;
		return GL_POINTS;
	}
}

void init(void)
{
	mousePos[0] = mousePos[1] = 0.0f;
}

void display(void)
{
	// Initialize
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw whatever primitives that has been recorded and finalized
	for (const Command& command : primaryCommandBuffer)
	{
		command.Play();
	}

	// Realtime update the primitive being drawn currently
	// Set current color - this allows the user to change the color while drawing.
	glColor3fv(currentColor);
	// If we are drawing lines, set the width
	if (currentPrimitive == PRIMITIVE_LINE)
		glLineWidth(currentSize);
	// We don't need to set point size because the point data will be finalized
	// into the primaryCommandBuffer as soon as it was drawn thus it will be handled
	// by the primaryCommandBuffer

	// If we are drawing some closed shapes, we draw wire frame first before it is finalized
	if (!currentCommandBuffer.empty() && 
		(currentPrimitive == PRIMITIVE_TRIANGLE || currentPrimitive == PRIMITIVE_POLYGON || currentPrimitive == PRIMITIVE_QUAD)
		)
	{
		glLineWidth(1.0f);
	}
	glBegin(GL_LINE_STRIP);
	// Play the commands that has been recorded
	for (const Command& command : currentCommandBuffer)
	{
		command.Play();
	}
	// We want to use the mouse position as the finish vertex / next vertex of the shape
	if (!currentCommandBuffer.empty())
	{
		if (currentPrimitive == PRIMITIVE_QUAD)
		{
			// If we are handling quad, we need another two vertices
			glVertex2f(mousePos[0], currentCommandBuffer[0].data.f[1]);
			glVertex2fv(mousePos);
			glVertex2f(currentCommandBuffer[0].data.f[0], mousePos[1]);
		}
		else
			// Otherwise, just use mouse position
			glVertex2fv(mousePos);
		// Close the shape if we are not drawing line strips
		if (currentPrimitive != PRIMITIVE_LINE)
			currentCommandBuffer[0].Play();
	}
	glEnd();

	// Finalize
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	rasterSize[0] = w;
	rasterSize[1] = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, canvasSize[0], 0.0, canvasSize[1]);
	glViewport(0, 0, rasterSize[0], rasterSize[1]);

	glutPostRedisplay();
}

void finalizeBuffer()
{
	// glBegin
	Command beginCommand;
	beginCommand.type = OGL_BEGIN;
	beginCommand.data.ui[0] = Primitive(currentPrimitive);

	// glColor3f
	Command colorCommand;
	colorCommand.type = OGL_COLOR_3_F;
	colorCommand.data.f[0] = currentColor[0];
	colorCommand.data.f[1] = currentColor[1];
	colorCommand.data.f[2] = currentColor[2];

	// glPointSize / glLineWidth
	Command sizeCommand;
	if (currentPrimitive == PRIMITIVE_POINT)
		sizeCommand.type = OGL_POINT_SIZE;
	else
		sizeCommand.type = OGL_LINE_WIDTH;
	sizeCommand.data.f[0] = currentSize;

	// glEnd
	Command endCommand;
	endCommand.type = OGL_END;

	primaryCommandBuffer.push_back(colorCommand);
	primaryCommandBuffer.push_back(sizeCommand);
	primaryCommandBuffer.push_back(beginCommand);

	// Copy commands to primaryCommandBuffer
	if (currentPrimitive != PRIMITIVE_QUAD)
	{
		// If not quad, just copy all
		for (const Command& cmd : currentCommandBuffer)
			primaryCommandBuffer.push_back(cmd);
	}
	else
	{
		// If quad, we need another two vertices
		primaryCommandBuffer.push_back(currentCommandBuffer[0]);

		Command additionalVert0;
		additionalVert0.type = OGL_VERTEX_2_F;
		additionalVert0.data.f[0] = currentCommandBuffer[0].data.f[0];
		additionalVert0.data.f[1] = currentCommandBuffer[1].data.f[1];

		primaryCommandBuffer.push_back(additionalVert0);
		
		primaryCommandBuffer.push_back(currentCommandBuffer[1]);
		
		Command additionalVert1;
		additionalVert1.type = OGL_VERTEX_2_F;
		additionalVert1.data.f[0] = currentCommandBuffer[1].data.f[0];
		additionalVert1.data.f[1] = currentCommandBuffer[0].data.f[1];

		primaryCommandBuffer.push_back(additionalVert1);

	}

	primaryCommandBuffer.push_back(endCommand);

	// Since the commands are copied to primaryCommandBuffer, clear the currentCommandBuffer
	currentCommandBuffer.clear();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// We do nothing when the app starts and user hasn't choose any shape
		if (currentPrimitive == PRIMITIVE_NULL) return;

		// Record the vertex command
		Command newCommand;
		newCommand.type = OGL_VERTEX_2_F;
		newCommand.data.f[0] = mousePos[0];
		newCommand.data.f[1] = mousePos[1];
		currentCommandBuffer.push_back(newCommand);

		// Finalize the command buffer if we have enough vertices
		if (currentPrimitive == PRIMITIVE_POINT ||
			(currentPrimitive == PRIMITIVE_TRIANGLE && currentCommandBuffer.size() == 3) ||
			(currentPrimitive == PRIMITIVE_QUAD && currentCommandBuffer.size() == 2))
		{
			finalizeBuffer();
		}
		
		glutPostRedisplay();
	}
}

void motion(int x, int y)
{
	// mouse events are handled by OS, eventually. When using mouse in the raster window, it assumes top-left is the origin.
	// Note: the raster window created by GLUT assumes bottom-left is the origin.
	mousePos[0] = (float)x / rasterSize[0] * canvasSize[0];
	mousePos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27: // ESC
		exit(0);
		break;
	case 13: // ENTER
		// Finalize the command buffer as user requires when handling line strips and polygons
		if (currentPrimitive == PRIMITIVE_LINE || currentPrimitive == PRIMITIVE_POLYGON)
			finalizeBuffer();
		glutPostRedisplay();
		break;
	}
}

void menu(int value)
{
	switch (value) {
	case MENU_CLEAR:
		// Clear both command buffers
		currentCommandBuffer.clear();
		primaryCommandBuffer.clear();
		break;
	case MENU_QUIT:
		exit(0);
	case MENU_COLORS_RED:
		currentColor[0] = 1.0f;
		currentColor[1] = 0.0f;
		currentColor[2] = 0.0f;
		break;
	case MENU_COLORS_GREEN:
		currentColor[0] = 0.0f;
		currentColor[1] = 1.0f;
		currentColor[2] = 0.0f;
		break;
	case MENU_COLORS_BLUE:
		currentColor[0] = 0.0f;
		currentColor[1] = 0.0f;
		currentColor[2] = 1.0f;
		break;
	case MENU_COLORS_BLACK:
		currentColor[0] = 0.0f;
		currentColor[1] = 0.0f;
		currentColor[2] = 0.0f;
		break;
	case MENU_SIZES_SMALL:
		currentSize = 1.0f;
		break;
	case MENU_SIZES_MEDIUM:
		currentSize = 5.0f;
		break;
	case MENU_SIZES_LARGE:
		currentSize = 10.0f;
		break;
	case MENU_OBJECTS_POINT:
		currentPrimitive = PRIMITIVE_POINT;
		break;
	case MENU_OBJECTS_LINE:
		currentPrimitive = PRIMITIVE_LINE;
		break;
	case MENU_OBJECTS_TRIANGLE:
		currentPrimitive = PRIMITIVE_TRIANGLE;
		break;
	case MENU_OBJECTS_QUAD:
		currentPrimitive = PRIMITIVE_QUAD;
		break;
	case MENU_OBJECTS_POLYGON:
		currentPrimitive = PRIMITIVE_POLYGON;
		break;
	default:
		break;
	}
	// Clear the currentCommandBuffer if user changes the shape while drawing
	if ((value & 0b00000111) == MENU_OBJECTS)
	{
		currentCommandBuffer.clear();
	}
	glutPostRedisplay();
}
void createMenu()
{
	int objectsMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Point", MENU_OBJECTS_POINT);
	glutAddMenuEntry("Line", MENU_OBJECTS_LINE);
	glutAddMenuEntry("Triangle", MENU_OBJECTS_TRIANGLE);
	glutAddMenuEntry("Quad", MENU_OBJECTS_QUAD);
	glutAddMenuEntry("Polygon", MENU_OBJECTS_POLYGON);

	int colorMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Red", MENU_COLORS_RED);
	glutAddMenuEntry("Green", MENU_COLORS_GREEN);
	glutAddMenuEntry("Blue", MENU_COLORS_BLUE);
	glutAddMenuEntry("Black", MENU_COLORS_BLACK);

	int sizeMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Small", MENU_SIZES_SMALL);
	glutAddMenuEntry("Medium", MENU_SIZES_MEDIUM);
	glutAddMenuEntry("Large", MENU_SIZES_LARGE);

	glutCreateMenu(menu);
	glutAddMenuEntry("Clear", MENU_CLEAR);
	glutAddSubMenu("Objects", objectsMenu);
	glutAddSubMenu("Colors", colorMenu);
	glutAddSubMenu("Sizes", sizeMenu);
	glutAddMenuEntry("Quit", MENU_QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(rasterSize[0], rasterSize[1]);
	glutCreateWindow("IGME.797.01 Project 1: Interactive 2D Graphics      Author: Victor Shu");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motion);
	createMenu();
	glutMainLoop();
	return 0;


}