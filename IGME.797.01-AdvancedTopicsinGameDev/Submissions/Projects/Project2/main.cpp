//Usage:
//Hold down the number keys , 1-7, to select one or multiple circles.
//While circle(s) are selected, use the left mouse button to translate and use the right mouse button to rotate.

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

int win_width = 1200, win_height = 800;

// Node of a tree
struct node
{
	node* parent;
	std::vector<node*> children;

	float translation[2];
	float rotation;

	float center[2];
	float extent[2];

	bool selected;

	void add_child(node* child)
	{
		children.push_back(child);
		child->parent = this;
	}
};

// Currently selected node and its position in children array
node* current_selected = nullptr;
int current_selected_in_child = 0;

// Draw a rectangle based on center and extent
void drawRect(float centerX, float centerY, float extentX, float extentY)
{
	float left = centerX - extentX;
	float right = centerX + extentX;
	float top = centerY - extentY;
	float bottom = centerY + extentY;

	glBegin(GL_LINE_LOOP);
	glVertex2f(left, top);
	glVertex2f(right, top);
	glVertex2f(right, bottom);
	glVertex2f(left, bottom);
	glEnd();
}

// Draw the whole / sub tree of root
void drawTree(node* root)
{
	glPushMatrix();
	
	glTranslatef(root->translation[0], root->translation[1], 0.0f);
	glRotatef(root->rotation, 0.0f, 0.0f, 1.0f);


	if (root->selected)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(10.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(3.0f);
	}
	
	drawRect(root->center[0], root->center[1], root->extent[0], root->extent[1]);
	for (auto& i : root->children)
	{
		drawTree(i);
	}
	
	glPopMatrix();
}

// Geometries
node torso = {};
node chest = {};
node neck = {};
node head = {};
node arm_r = {};
node forearm_r = {};
node hand_r = {};
node arm_l = {};
node forearm_l = {};
node hand_l = {};
node thigh_r = {};
node leg_r = {};
node foot_r = {};
node thigh_l = {};
node leg_l = {};
node foot_l = {};

void init(void)
{
	// Transformation Relationship
	torso.add_child(&chest);
	torso.add_child(&thigh_r);
	torso.add_child(&thigh_l);

	chest.add_child(&neck);
	chest.add_child(&arm_r);
	chest.add_child(&arm_l);

	neck.add_child(&head);

	arm_r.add_child(&forearm_r);

	forearm_r.add_child(&hand_r);

	arm_l.add_child(&forearm_l);

	forearm_l.add_child(&hand_l);

	thigh_r.add_child(&leg_r);

	leg_r.add_child(&foot_r);

	thigh_l.add_child(&leg_l);

	leg_l.add_child(&foot_l);

	// Geometrical Data
	torso.center[0] = 0.0f;
	torso.center[1] = 0.0f;
	torso.extent[0] = 60.0f;
	torso.extent[1] = 40.0f;
	torso.translation[0] = float(win_width) / 2.0f;
	torso.translation[1] = float(win_height) / 2.0f;
	torso.rotation = 0.0f;
	torso.selected = true;
	current_selected = &torso;

	chest.center[0] = 0.0f;
	chest.center[1] = 50.0f;
	chest.extent[0] = 80.0f;
	chest.extent[1] = 50.0f;
	chest.translation[0] = 0.0f;
	chest.translation[1] = 40.0f;
	chest.rotation = 0.0f;

	neck.center[0] = 0.0f;
	neck.center[1] = 25.0f;
	neck.extent[0] = 10.0f;
	neck.extent[1] = 25.0f;
	neck.translation[0] = 0.0f;
	neck.translation[1] = 100.0f;
	neck.rotation = 0.0f;

	head.center[0] = 0.0f;
	head.center[1] = 50.0f;
	head.extent[0] = 50.0f;
	head.extent[1] = 50.0f;
	head.translation[0] = 0.0f;
	head.translation[1] = 50.0f;
	head.rotation = 0.0f;

	arm_r.center[0] = -40.0f;
	arm_r.center[1] = 0.0f;
	arm_r.extent[0] = 40.0f;
	arm_r.extent[1] = 20.0f;
	arm_r.translation[0] = -80.0f;
	arm_r.translation[1] = 50.0f;
	arm_r.rotation = 0.0f;

	forearm_r.center[0] = -40.0f;
	forearm_r.center[1] = 0.0f;
	forearm_r.extent[0] = 40.0f;
	forearm_r.extent[1] = 20.0f;
	forearm_r.translation[0] = -80.0f;
	forearm_r.translation[1] = 0.0f;
	forearm_r.rotation = 0.0f;

	hand_r.center[0] = -35.0f;
	hand_r.center[1] = 0.0f;
	hand_r.extent[0] = 35.0f;
	hand_r.extent[1] = 40.0f;
	hand_r.translation[0] = -80.0f;
	hand_r.translation[1] = 0.0f;
	hand_r.rotation = 0.0f;

	arm_l.center[0] = 40.0f;
	arm_l.center[1] = 0.0f;
	arm_l.extent[0] = 40.0f;
	arm_l.extent[1] = 20.0f;
	arm_l.translation[0] = 80.0f;
	arm_l.translation[1] = 50.0f;
	arm_l.rotation = 0.0f;

	forearm_l.center[0] = 40.0f;
	forearm_l.center[1] = 0.0f;
	forearm_l.extent[0] = 40.0f;
	forearm_l.extent[1] = 20.0f;
	forearm_l.translation[0] = 80.0f;
	forearm_l.translation[1] = 0.0f;
	forearm_l.rotation = 0.0f;

	hand_l.center[0] = 35.0f;
	hand_l.center[1] = 0.0f;
	hand_l.extent[0] = 35.0f;
	hand_l.extent[1] = 40.0f;
	hand_l.translation[0] = 80.0f;
	hand_l.translation[1] = 0.0f;
	hand_l.rotation = 0.0f;

	thigh_r.center[0] = 0.0f;
	thigh_r.center[1] = -50.0f;
	thigh_r.extent[0] = 20.0f;
	thigh_r.extent[1] = 50.0f;
	thigh_r.translation[0] = -40.0f;
	thigh_r.translation[1] = -40.0f;
	thigh_r.rotation = 0.0f;

	leg_r.center[0] = 0.0f;
	leg_r.center[1] = -50.0f;
	leg_r.extent[0] = 20.0f;
	leg_r.extent[1] = 50.0f;
	leg_r.translation[0] = 0.0f;
	leg_r.translation[1] = -100.0f;
	leg_r.rotation = 0.0f;

	foot_r.center[0] = -30.0f;
	foot_r.center[1] = -25.0f;
	foot_r.extent[0] = 50.0f;
	foot_r.extent[1] = 25.0f;
	foot_r.translation[0] = 0.0f;
	foot_r.translation[1] = -100.0f;
	foot_r.rotation = 0.0f;

	thigh_l.center[0] = 0.0f;
	thigh_l.center[1] = -50.0f;
	thigh_l.extent[0] = 20.0f;
	thigh_l.extent[1] = 50.0f;
	thigh_l.translation[0] = 40.0f;
	thigh_l.translation[1] = -40.0f;
	thigh_l.rotation = 0.0f;

	leg_l.center[0] = 0.0f;
	leg_l.center[1] = -50.0f;
	leg_l.extent[0] = 20.0f;
	leg_l.extent[1] = 50.0f;
	leg_l.translation[0] = 0.0f;
	leg_l.translation[1] = -100.0f;
	leg_l.rotation = 0.0f;

	foot_l.center[0] = 30.0f;
	foot_l.center[1] = -25.0f;
	foot_l.extent[0] = 50.0f;
	foot_l.extent[1] = 25.0f;
	foot_l.translation[0] = 0.0f;
	foot_l.translation[1] = -100.0f;
	foot_l.rotation = 0.0f;
}

void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	drawTree(&torso);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	win_width = w;
	win_height = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, win_width, 0.0f, win_height);
	glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);

	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) // 'esc' key
		exit(0);

	if (key == 'a')
	{
		current_selected->rotation += 10.0f;
	}
	else if (key == 'd')
	{
		current_selected->rotation -= 10.0f;
	}

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		if (current_selected->parent != nullptr)
		{
			current_selected->selected = false;
			current_selected = current_selected->parent;
			current_selected->selected = true;
			current_selected_in_child = 0;
		}
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (!current_selected->children.empty())
		{
			current_selected->selected = false;
			current_selected = current_selected->children[0];
			current_selected->selected = true;
			current_selected_in_child = 0;
		}
	}
	else if (key == GLUT_KEY_LEFT)
	{
		if (current_selected->parent != nullptr)
		{
			node* parent = current_selected->parent;
			current_selected->selected = false;
			current_selected_in_child = (current_selected_in_child + 1) % int(parent->children.size());
			current_selected = parent->children[current_selected_in_child];
			current_selected->selected = true;
		}
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		if (current_selected->parent != nullptr)
		{
			node* parent = current_selected->parent;
			current_selected->selected = false;
			--current_selected_in_child;
			if (current_selected_in_child < 0) current_selected_in_child += parent->children.size();
			current_selected = parent->children[current_selected_in_child];
			current_selected->selected = true;
		}
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("IGME797.01 Project 2: Hierarchical Transformations      Author: Victor Shu");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMainLoop();
	return 0;

}