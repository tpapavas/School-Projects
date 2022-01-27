#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define _USE_MATH_DEFINES

#include <math.h>

#define MAX_DISTANCE 15.0f

//for view (gluLookAt)
GLfloat xViewer = 0.0f, yViewer = 0.0f, zViewer = 70.0f;
GLfloat xref = 0.0, yref = 0.0, zref = 0.0;
GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0;
GLint theta = 0;
GLdouble ratio = 2 * M_PI / 360;

GLint windowId;
GLint maxWindowX = 640;
GLint maxWindowY = 480;

typedef enum {
    CUBIC_CURVES,
    CUBIC_SURFACE,
    BEZIER_ORDER_7,
    CUBIC_BEZIER,
    SHOW_CONVEX,
    EXIT
} Mode;

//state of the program
struct State {
    bool pointsSet;
    GLint setPointsLeft;
    Mode mode;
    GLint numOfPoints;
    bool showConvex;
    GLint lim1;     //for convex hull (bezier only)
    GLint lim2;     //for convex hull (bezier only)
} currState;

bool leftMouseClicked;
bool movePoint;  //a point is currently moving
GLint mvPntInd;  //index of point that has been moved
GLint clickX;
GLint clickY;

typedef struct treenode {
    GLfloat m[16];
    void(*f);
    struct treenode* sibling;
    struct treenode* child;
} treenode;


GLfloat theta1[10] = { 0.0f };

treenode torso_node, head_node, neck_node;

void torso() {}

void myinit(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MAP1_VERTEX_3);

    glMatrixMode(GL_MODELVIEW);

    //setup treenodes for dog    
    //torso
    glLoadIdentity();
    glRotatef(theta1[0], 0.0, 1.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);
    torso_node.f = torso;
    torso_node.sibling = NULL;
    torso_node.child = &neck_node;


/*  glLoadIdentity();
    gluLookAt(xViewer, yViewer, zViewer, xref, yref, zref, Vx, Vy, Vz);

    */

    glClearColor(1.0, 1.0, 1.0, 0.0);

    //set up viewing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, maxWindowX, 0, maxWindowY, -500, 500);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //for camera
    GLfloat xr = xViewer * cos(theta * ratio) - zViewer * sin(theta * ratio);
    GLfloat zr = xViewer * sin(theta * ratio) + zViewer * cos(theta * ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xr, yViewer, zr, xref, yref, zref, Vx, Vy, Vz);

    glutSwapBuffers();
}


void mouse_callback_func(int button, int state, int x, int y) {
    //mod mouse x,y to match with world coordinates
    clickX = x;
    clickY = maxWindowY - y;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            leftMouseClicked = true;
        }
        else if (state == GLUT_UP && leftMouseClicked) {
            leftMouseClicked = false;
            movePoint = false;
        }
    }
}

void mouse_motion_callback_func(int x, int y) {
    if (leftMouseClicked) {

        y = maxWindowY - y;

        printf("(%d, %d)\n", x, y);

        clickX += x - clickX;
        clickY += y - clickY;

        glutPostRedisplay();
    }
}

void menu(int id) {
    switch (id) {
    case EXIT:
        glutDestroyWindow(windowId);
        exit(0);

    default:
        break;
    }

    glutPostRedisplay();
}


void keyboard_callback_func(unsigned char key, int x, int y) {
    if (key == 'w') {
        yViewer += 10;
    }
    else if (key == 'd') {
        theta -= 10;
    }
    else if (key == 'a') {
        theta += 10;
    }
    else if (key == 's') {
        yViewer -= 10;
    }
    else if (key == 'r') {
        theta = 0;
        yViewer = 0;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(maxWindowX, maxWindowY);
    glutInitWindowPosition(10, 10);
    windowId = glutCreateWindow("project1");
    glutDisplayFunc(display);

    glutMouseFunc(mouse_callback_func);
    glutMotionFunc(mouse_motion_callback_func);

    GLint bezierMenu = glutCreateMenu(menu);
    glutAddMenuEntry("• Bezier Order 7", BEZIER_ORDER_7);
    glutAddMenuEntry("• Cubic Bezier", CUBIC_BEZIER);
    glutAddMenuEntry("Show/Hide Convex Hull", SHOW_CONVEX);
    glutCreateMenu(menu);
    glutAddMenuEntry("• Cubic Curves", CUBIC_CURVES);
    glutAddSubMenu("- Bezier", bezierMenu);
    glutAddMenuEntry("• Cubic Surface", CUBIC_SURFACE);
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutKeyboardFunc(keyboard_callback_func);

    myinit();
    glutMainLoop();

    return 0;
}