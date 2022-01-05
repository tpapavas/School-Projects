#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define _USE_MATH_DEFINES

#include <math.h>

GLint windowWidth = 500;
GLint windowHeight = 500;

GLint numOfPoints = 10000;
GLint numOfVertices = 5;
GLfloat ratio = 1 / 3.0;

GLdouble rad2deg = M_PI / 180.0;

/* define a point data type */
typedef GLfloat point2[2];

point2* vertices;

bool leftMouseClicked;
bool multiColor;
bool shift;

GLint xStart, yStart;
GLint xCurr, yCurr = 0;

GLint windowId;

enum MenuEntry {
	POINTS8000,
	POINTS10000,
	PENTAGON38,
	PENTAGON13,
	EXIT
};

void findPolygonVerteces(int n) {
	GLfloat R = (4 / 7.0) * windowHeight;
	point2 C = {(4 / 7.0) * windowWidth, (4 / 7.0) * windowHeight};

	GLdouble theta = 360.0 / n;

	point2 prevVertex;
	prevVertex[0] = 0.0;
	prevVertex[1] = R;

	vertices[0][0] = C[0] + prevVertex[0];
	vertices[0][1] = C[1] + prevVertex[1];


	for (int i = 0; i < n - 1; i++) {
		point2 vertex;
		vertex[0] = cos(theta * rad2deg) * prevVertex[0] - sin(theta * rad2deg) * prevVertex[1];
		vertex[1] = sin(theta * rad2deg) * prevVertex[0] + cos(theta * rad2deg) * prevVertex[1];

		prevVertex[0] = vertex[0];
		prevVertex[1] = vertex[1];

		vertices[i + 1][0] = C[0] + vertex[0];
		vertices[i + 1][1] = C[1] + vertex[1];
	}
}

void myinit(void) {
	/* attributes */
	glEnable(GL_BLEND);
	glClearColor(1.0, 1.0, 1.0, 0.0); /* white background */
	glColor3f(1.0, 0.0, 0.0); /* draw in red */

	/* set up viewing */
	/* 500 x 500 window with origin lower left */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void display(void) {
	findPolygonVerteces(numOfVertices);

	int i, j, k;
	//long rand();       /* standard random number generator */
	point2 p = {75.0, 50.0};  /* An srbitrary initial point */

	glClear(GL_COLOR_BUFFER_BIT);  /*clear the window */

	/* computes and plots 5000 new points */
	for (k = 0; k < numOfPoints; k++) {
		j = rand() % numOfVertices; /* pick a vertex at random */

		/* Compute point halfway between vertex and old point */
		p[0] = (p[0] + vertices[j][0]) * ratio;
		p[1] = (p[1] + vertices[j][1]) * ratio;

		if (multiColor) {
			GLfloat red = (float)rand() / RAND_MAX;
			GLfloat green = (float)rand() / RAND_MAX;
			GLfloat blue = (float)rand() / RAND_MAX;

			glColor3f(red, green, blue);
		}

		/* plot new point */
		glBegin(GL_POINTS);
		glVertex2fv(p);
		glEnd();
	}

	glutSwapBuffers();
}

void shiftWindow(int x, int y) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat left = 0.0 - xCurr - x;
	GLfloat right = 500.0 - xCurr - x;
	GLfloat bottom = 0.0 + yCurr + y;
	GLfloat top = 500.0 + yCurr + y;

	gluOrtho2D(left, right, bottom, top);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void mouse_callback_func(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			leftMouseClicked = true;
			xStart = x;
			yStart = y;
		}
		else if (state == GLUT_UP && leftMouseClicked) {
			leftMouseClicked = false;
			xCurr += x - xStart;
			yCurr += y - yStart;
		}
	}
}

void mouse_motion_callback_func(int x, int y) {
	if (leftMouseClicked) {
		shiftWindow(x - xStart, y - yStart);
	}
}

void menu(int id) {
	switch (id) {
	case POINTS8000:
		numOfPoints = 8000;

		GLfloat red = (float)rand() / RAND_MAX;
		GLfloat green = (float)rand() / RAND_MAX;
		GLfloat blue = (float)rand() / RAND_MAX;

		glColor3f(red, green, blue);
		multiColor = false;
		break;

	case POINTS10000:
		numOfPoints = 10000;
		multiColor = true;
		break;

	case PENTAGON38:
		ratio = 3 / 8.0;
		break;

	case PENTAGON13:
		ratio = 1 / 3.0;
		break;

	case EXIT:
		glutDestroyWindow(windowId);
		free(vertices);
		exit(0);

	default:
		break;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	srand(time(0));
	vertices = (point2*) malloc(sizeof(point2) * numOfVertices);

	/* Standard GLUT initialization */

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); /* default, not needed */
	glutInitWindowSize(500, 500); /* 500 x 500 pixel window */
	glutInitWindowPosition(0, 0); /* place window top left on display */
	windowId = glutCreateWindow("Sierpinski Gasket"); /* window title */

	//glViewport(0, 0, 500, 500);
	glutMouseFunc(mouse_callback_func);
	glutMotionFunc(mouse_motion_callback_func);

	glutCreateMenu(menu);
	glutAddMenuEntry("8000 points", POINTS8000);
	glutAddMenuEntry("10000 points, random color", POINTS10000);
	glutAddMenuEntry("Pentagon r=3/8", PENTAGON38);
	glutAddMenuEntry("Pentagon r=1/3", PENTAGON13);
	glutAddMenuEntry("Exit", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(display); /* display callback invoked when window opened */

	myinit(); /* set attributes */

	glutMainLoop(); /* enter event loop */
}

