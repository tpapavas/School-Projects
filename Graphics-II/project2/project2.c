#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define _USE_MATH_DEFINES

#include <math.h>

GLfloat Dy = 10.0f;

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


//nodes' dimensions
const GLfloat TORSO_RADIUS = 60.0f;
const GLfloat TORSO_HEIGHT = 200.0f;

const GLfloat HEAD_RADIUS = 20.0f;
const GLfloat NECK_RADIUS = 20.0f;
const GLfloat NECK_HEIGHT = 50.0f;

const GLfloat FRONT_RIGHT_UPPER_RADIUS = 15.0f;
const GLfloat FRONT_RIGHT_UPPER_HEIGHT = 45.0f;
const GLfloat FRONT_RIGHT_LOWER_RADIUS = 10.0f;
const GLfloat FRONT_RIGHT_LOWER_HEIGHT = 20.0f;
const GLfloat FRONT_RIGHT_FOOT_RADIUS = 10.0f;
const GLfloat FRONT_RIGHT_FOOT_HEIGHT = 20.0f;

const GLfloat FRONT_LEFT_UPPER_RADIUS = 15.0f;
const GLfloat FRONT_LEFT_UPPER_HEIGHT = 45.0f;
const GLfloat FRONT_LEFT_LOWER_RADIUS = 10.0f;
const GLfloat FRONT_LEFT_LOWER_HEIGHT = 20.0f;
const GLfloat FRONT_LEFT_FOOT_RADIUS = 10.0f;
const GLfloat FRONT_LEFT_FOOT_HEIGHT = 20.0f;

const GLfloat BACK_RIGHT_UPPER_RADIUS = 10.0f;
const GLfloat BACK_RIGHT_UPPER_HEIGHT = 20.0f;
const GLfloat BACK_RIGHT_LOWER_RADIUS = 10.0f;
const GLfloat BACK_RIGHT_LOWER_HEIGHT = 20.0f;
const GLfloat BACK_RIGHT_FOOT_RADIUS = 10.0f;
const GLfloat BACK_RIGHT_FOOT_HEIGHT = 20.0f;

const GLfloat BACK_LEFT_UPPER_RADIUS = 10.0f;
const GLfloat BACK_LEFT_UPPER_HEIGHT = 20.0f;
const GLfloat BACK_LEFT_LOWER_RADIUS = 10.0f;
const GLfloat BACK_LEFT_LOWER_HEIGHT = 20.0f;
const GLfloat BACK_LEFT_FOOT_RADIUS = 10.0f;
const GLfloat BACK_LEFT_FOOT_HEIGHT = 20.0f;


GLfloat theta1[15] = { 0.0f, 0.0f, 0.0f, 90.0f};

treenode torso_node, head_node, neck_node, fru_node, frl_node, frf_node, flu_node, fll_node, flf_node, bru_node, brl_node, brf_node, blu_node, bll_node, blf_node;

void torso(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, TORSO_RADIUS, TORSO_RADIUS, TORSO_HEIGHT, 1000, 1000);
}

void neck() {
    glColor3f(0.7, 0.2, 0.1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, NECK_RADIUS, NECK_RADIUS, NECK_HEIGHT, 1000, 1000);
}

void head() {}

void fru() {
    glColor3f(0.5, 0.8, 0.1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_RIGHT_UPPER_RADIUS, FRONT_RIGHT_UPPER_RADIUS, FRONT_RIGHT_UPPER_HEIGHT, 1000, 1000);
}

void frl() {
    glColor3f(0.8, 0.5, 0.4);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_HEIGHT, 1000, 1000);
}
void frf() {}
void flu() {
    glColor3f(0.5, 0.8, 0.1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_LEFT_UPPER_RADIUS, FRONT_LEFT_UPPER_RADIUS, FRONT_LEFT_UPPER_HEIGHT, 1000, 1000);
}
void fll() {
    glColor3f(0.8, 0.5, 0.4);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_HEIGHT, 1000, 1000);
}
void flf() {}
void bru() {}
void brl() {}
void brf() {}
void blu() {}
void bll() {}
void blf() {}

void traverse(treenode* root) {
    if (root == NULL) {
        return;
    }
    glPushMatrix();  //save current state
    glMultMatrixf(root->m);
    void (*fPtr)(void) = root->f;
    (*fPtr)();
    //printf("tes ");
    if (root->child != NULL) {
        traverse(root->child);
    }
    glPopMatrix();  //return to state before root
    if (root->sibling != NULL) {
        traverse(root->sibling);
    }
}

void moveBullet() {
    glColor3f(0, 0, 0);

    GLfloat DDx = TORSO_HEIGHT - FRONT_RIGHT_UPPER_RADIUS;
    GLfloat DDy = FRONT_RIGHT_UPPER_HEIGHT + FRONT_RIGHT_LOWER_HEIGHT;
    GLfloat DDz = TORSO_RADIUS;

    glBegin(GL_LINES);
    glVertex3f(DDx, -(DDy + Dy), DDz);
    glVertex3f(DDx, -(DDy + Dy + 30), DDz);
    glEnd();

    Dy += 80.0f;
    if (Dy > 500) {
        Dy = 10;
    }
  
    glutPostRedisplay();
}

void myinit(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MAP1_VERTEX_3);

    glMatrixMode(GL_MODELVIEW);

    //setup treenodes for dog    
    //torso
    glLoadIdentity();
    //glTranslatef();
    glRotatef(theta1[0], 0.0, 1.0, 0.0);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);
    torso_node.f = torso;
    torso_node.sibling = NULL;
    torso_node.child = &neck_node;

    //neck
    glLoadIdentity();
    glTranslatef(0.0, 0.0, (TORSO_HEIGHT));
    glRotatef(theta1[0], 0.0, 0.0, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, neck_node.m);
    neck_node.f = neck;
    neck_node.sibling = &fru_node;
    neck_node.child = &head_node;

    //head
    glLoadIdentity();
    glTranslatef((NECK_HEIGHT + HEAD_RADIUS) / 2, 0.0, 0.0);
    glRotatef(theta1[0], 0.0, 0.0, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, head_node.m);
    head_node.f = head;
    head_node.sibling = NULL;
    head_node.child = NULL;

    //front right leg
    //front right upper
    glLoadIdentity();
    glTranslatef(-TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT-FRONT_RIGHT_UPPER_RADIUS);
    glRotatef(theta1[3], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, fru_node.m);
    fru_node.f = fru;
    fru_node.sibling = &flu_node;  //pending//
    fru_node.child = &frl_node;

    //front right lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_RIGHT_UPPER_HEIGHT);
    glGetFloatv(GL_MODELVIEW_MATRIX, frl_node.m);
    frl_node.f = frl;
    frl_node.sibling = NULL;
    frl_node.child = NULL;  //pending

    //front left upper
    glLoadIdentity();
    glTranslatef(TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT - FRONT_LEFT_UPPER_RADIUS);
    glRotatef(theta1[3], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, flu_node.m);
    flu_node.f = flu;
    flu_node.sibling = NULL;  //pending//
    flu_node.child = &fll_node;

    //front left lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_LEFT_UPPER_HEIGHT);
    glGetFloatv(GL_MODELVIEW_MATRIX, fll_node.m);
    fll_node.f = fll;
    fll_node.sibling = NULL;
    fll_node.child = NULL;


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


    glTranslatef(200, 200, 0);

    glColor3f(0.5, 0.5, 0.5);
    traverse(&torso_node);

    //moveBullet();
    
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