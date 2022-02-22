﻿#include <GL/glut.h>
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
    LIFT_FRONT_RIGHT_LEG,
    STAND_ON_BACK_LEGS,
    BEND_NECK_AND_HEAD,
    BACK_TO_NORMAL,
    WALK,
    NOT_MOVING,
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

Mode motionMode = NOT_MOVING;
bool keepMoving = false;
GLint dir = 1;
GLint motionFrames = 0;
GLfloat dTorso[3] = { 0.0f,0.0,0.0 };

typedef struct treenode {
    GLfloat m[16];
    void(*f);
    struct treenode* sibling;
    struct treenode* child;
} treenode;


//nodes' dimensions
const GLfloat TORSO_RADIUS = 35.0f;
const GLfloat TORSO_HEIGHT = 190.0f;

const GLfloat HEAD_RADIUS = 25.0f;
const GLfloat NECK_RADIUS = 15.0f;
const GLfloat NECK_HEIGHT = 55.0f;

const GLfloat FRONT_RIGHT_UPPER_RADIUS = 12.0f;
const GLfloat FRONT_RIGHT_UPPER_HEIGHT = 45.0f;
const GLfloat FRONT_RIGHT_LOWER_RADIUS = 8.0f;
const GLfloat FRONT_RIGHT_LOWER_HEIGHT = 35.0f;
const GLfloat FRONT_RIGHT_FOOT_RADIUS = 5.0f;
const GLfloat FRONT_RIGHT_FOOT_HEIGHT = 35.0f;

const GLfloat FRONT_LEFT_UPPER_RADIUS = 12.0f;
const GLfloat FRONT_LEFT_UPPER_HEIGHT = 45.0f;
const GLfloat FRONT_LEFT_LOWER_RADIUS = 8.0f;
const GLfloat FRONT_LEFT_LOWER_HEIGHT = 35.0f;
const GLfloat FRONT_LEFT_FOOT_RADIUS = 5.0f;
const GLfloat FRONT_LEFT_FOOT_HEIGHT = 35.0f;

const GLfloat BACK_RIGHT_UPPER_RADIUS = 12.0f;
const GLfloat BACK_RIGHT_UPPER_HEIGHT = 45.0f;
const GLfloat BACK_RIGHT_LOWER_RADIUS = 8.0f;
const GLfloat BACK_RIGHT_LOWER_HEIGHT = 35.0f;
const GLfloat BACK_RIGHT_FOOT_RADIUS = 5.0f;
const GLfloat BACK_RIGHT_FOOT_HEIGHT = 35.0f;

const GLfloat BACK_LEFT_UPPER_RADIUS = 12.0f;
const GLfloat BACK_LEFT_UPPER_HEIGHT = 45.0f;
const GLfloat BACK_LEFT_LOWER_RADIUS = 8.0f;
const GLfloat BACK_LEFT_LOWER_HEIGHT = 35.0f;
const GLfloat BACK_LEFT_FOOT_RADIUS = 5.0f;
const GLfloat BACK_LEFT_FOOT_HEIGHT = 35.0f;

//nodes' rotation thetas
GLfloat theta1[5][3] = {
    {  0.0, 0.0,  0.0 },       //torso, neck, head
    { 90.0, 0.0, 90.0 },        //fru, frl, frf
    { 90.0, 0.0, 90.0 },
    { 90.0, 0.0, 90.0 },
    { 90.0, 0.0, 90.0 },
};

treenode torso_node, head_node, neck_node, fru_node, frl_node, frf_node, flu_node, fll_node, flf_node, bru_node, brl_node, brf_node, blu_node, bll_node, blf_node;

void torso(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, TORSO_RADIUS, TORSO_RADIUS, TORSO_HEIGHT, 200, 200);
}

void neck() {
    glColor3f(0.7, 0.2, 0.1);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, NECK_RADIUS, NECK_RADIUS, NECK_HEIGHT, 200, 200);
}

void head() {
    glColor3f(0.8, 0.8, 0.3);

    GLUquadric* q = gluNewQuadric();
    gluSphere(q, HEAD_RADIUS, 200, 200);
}

////FRONT RIGHT LEG////
void fru() {
    glColor3f(0.5, 0.8, 0.1);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_RIGHT_UPPER_RADIUS, FRONT_RIGHT_UPPER_RADIUS, FRONT_RIGHT_UPPER_HEIGHT, 200, 200);
}
void frl() {
    glColor3f(0.8, 0.5, 0.4);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_HEIGHT, 200, 200);
}
void frf() {
    glColor3f(0.1, 0.7, 0.7);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_RIGHT_FOOT_RADIUS, FRONT_RIGHT_FOOT_RADIUS, FRONT_RIGHT_FOOT_HEIGHT, 200, 200);
}

////FRONT LEFT LEG////
void flu() {
    glColor3f(0.3, 0.6, 0.1);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_LEFT_UPPER_RADIUS, FRONT_LEFT_UPPER_RADIUS, FRONT_LEFT_UPPER_HEIGHT, 200, 200);
}
void fll() {
    glColor3f(0.6, 0.5, 0.3);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_HEIGHT, 200, 200);
}
void flf() {
    glColor3f(0.1, 0.5, 0.6);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, FRONT_LEFT_FOOT_RADIUS, FRONT_LEFT_FOOT_RADIUS, FRONT_LEFT_FOOT_HEIGHT, 200, 200);
}


////BACK RIGHT LEG////
void bru() {
    glColor3f(0.5, 0.8, 0.1);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_RIGHT_UPPER_RADIUS, BACK_RIGHT_UPPER_RADIUS, BACK_RIGHT_UPPER_HEIGHT, 200, 200);
}
void brl() {
    glColor3f(0.8, 0.5, 0.4);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_RIGHT_LOWER_RADIUS, BACK_RIGHT_LOWER_RADIUS, BACK_RIGHT_LOWER_HEIGHT, 200, 200);
}
void brf() {
    glColor3f(0.1, 0.7, 0.7);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_RIGHT_FOOT_RADIUS, BACK_RIGHT_FOOT_RADIUS, BACK_RIGHT_FOOT_HEIGHT, 200, 200);
}


////BACK LEFT LEG////
void blu() {
    glColor3f(0.5, 0.8, 0.1);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_LEFT_UPPER_RADIUS, BACK_LEFT_UPPER_RADIUS, BACK_LEFT_UPPER_HEIGHT, 200, 200);
}
void bll() {
    glColor3f(0.8, 0.5, 0.4);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_LEFT_LOWER_RADIUS, BACK_LEFT_LOWER_RADIUS, BACK_LEFT_LOWER_HEIGHT, 200, 200);
}
void blf() {
    glColor3f(0.1, 0.7, 0.7);

    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, BACK_LEFT_FOOT_RADIUS, BACK_LEFT_FOOT_RADIUS, BACK_LEFT_FOOT_HEIGHT, 200, 200);
}

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

void drawDog() {
    glMatrixMode(GL_MODELVIEW);
  
    //torso
    glLoadIdentity();
    glTranslatef(dTorso[0], dTorso[1], dTorso[2]);
    glRotatef(theta1[0][0], 0.0, 0.0f, 1.0f);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);
    torso_node.f = torso;

    //neck
    glLoadIdentity();
    glTranslatef(0.0, 0.0, (TORSO_HEIGHT));
    glRotatef(theta1[0][1] - 30, 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, neck_node.m);
    neck_node.f = neck;

    //head
    glLoadIdentity();
    glTranslatef(0.0, 0.0, NECK_HEIGHT + HEAD_RADIUS / 2);
    glRotatef(theta1[0][2], 0.0, 0.0, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, head_node.m);
    head_node.f = head;

    ////FRONT RIGHT LEG////
    //front right upper
    glLoadIdentity();
    glTranslatef(-TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT - FRONT_RIGHT_UPPER_RADIUS);
    glRotatef(theta1[1][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, fru_node.m);
    fru_node.f = fru;

    //front right lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_RIGHT_UPPER_HEIGHT);
    glRotatef(theta1[1][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, frl_node.m);
    frl_node.f = frl;

    //front right foot
    glLoadIdentity();
    glTranslatef(0.0, -FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_HEIGHT + FRONT_RIGHT_FOOT_RADIUS);
    glRotatef(-theta1[1][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, frf_node.m);
    frf_node.f = frf;


    ////FRONT LEFT LEG////
    //front left upper
    glLoadIdentity();
    glTranslatef(TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT - FRONT_LEFT_UPPER_RADIUS);
    glRotatef(theta1[2][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, flu_node.m);
    flu_node.f = flu;

    //front left lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_LEFT_UPPER_HEIGHT);
    glRotatef(theta1[2][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, fll_node.m);
    fll_node.f = fll;

    //front left foot
    glLoadIdentity();
    glTranslatef(0.0, -FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_HEIGHT + FRONT_LEFT_FOOT_RADIUS);
    glRotatef(-theta1[2][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, flf_node.m);
    flf_node.f = flf;


    ////BACK RIGHT LEG////
    //back right upper
    glLoadIdentity();
    glTranslatef(-TORSO_RADIUS, -TORSO_RADIUS, BACK_RIGHT_UPPER_RADIUS);
    glRotatef(theta1[3][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, bru_node.m);
    bru_node.f = bru;

    //back right lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, BACK_RIGHT_UPPER_HEIGHT);
    glRotatef(theta1[3][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, brl_node.m);
    brl_node.f = brl;

    //back right foot
    glLoadIdentity();
    glTranslatef(0.0, -BACK_RIGHT_LOWER_RADIUS, BACK_RIGHT_LOWER_HEIGHT + BACK_RIGHT_FOOT_RADIUS);
    glRotatef(-theta1[3][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, brf_node.m);
    brf_node.f = brf;


    ////BACK LEFT LEG////
    //back left upper
    glLoadIdentity();
    glTranslatef(TORSO_RADIUS, -TORSO_RADIUS, BACK_LEFT_UPPER_RADIUS);
    glRotatef(theta1[4][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, blu_node.m);
    blu_node.f = blu;

    //back left lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, BACK_LEFT_UPPER_HEIGHT);
    glRotatef(theta1[4][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, bll_node.m);
    bll_node.f = bll;

    //front left foot
    glLoadIdentity();
    glTranslatef(0.0, -BACK_LEFT_LOWER_RADIUS, BACK_LEFT_LOWER_HEIGHT + BACK_LEFT_FOOT_RADIUS);
    glRotatef(-theta1[4][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, blf_node.m);
    blf_node.f = blf;
}

void liftFrontRightLeg() {
    theta1[1][0] -= 5 * dir;
    theta1[1][1] += 5 * dir;
    theta1[1][2] -= 5 * dir;

    //if (theta1[1][0] < 45 || theta1[1][1] > 45 || theta1[1][2] < 45) {
        //theta1[1][0] = 90.0f;
}

void standOnBackLegs() {
    //torso
    theta1[0][0] += 3 * dir;

    //front right leg
    theta1[1][0] -= 5 * dir;
    theta1[1][1] += 5 * dir;
    theta1[1][2] -= 5 * dir;

    //front left leg
    theta1[2][0] -= 5 * dir;
    theta1[2][1] += 5 * dir;
    theta1[2][2] -= 5 * dir;

    //back legs
    theta1[3][0] += 3 * dir;
    theta1[4][0] += 3 * dir;
}

void bendNeckAndHead() {
    theta1[0][1] += 5 * dir;
}

void walk() {
    GLint frame = motionFrames % 25;
    
    if (frame < 6) {
        theta1[1][0] -= 7 * dir;
        theta1[1][1] += 7 * dir;
        theta1[1][2] -= 7 * dir;
    } else if (frame < 12 && frame < 18) {
        theta1[1][0] -= 7 * dir;
        theta1[1][1] += 7 * dir;
        theta1[1][2] -= 7 * dir;

        theta1[4][0] -= 7 * (-dir);
        theta1[4][1] += 7 * (-dir);
        theta1[4][2] -= 7 * (-dir);
    } if (frame < 18) {
        theta1[4][0] -= 7 * (-dir);
        theta1[4][1] += 7 * (-dir);
        theta1[4][2] -= 7 * (-dir);

        theta1[2][0] -= 7 * dir;
        theta1[2][1] += 7 * dir;
        theta1[2][2] -= 7 * dir;
    } 
    if (frame < 24) {
        theta1[2][0] -= 7 * dir;
        theta1[2][1] += 7 * dir;
        theta1[2][2] -= 7 * dir;

        theta1[3][0] -= 7 * dir;
        theta1[3][1] += 7 * dir;
        theta1[3][2] -= 7 * dir;
    }
    else {
        theta1[3][0] -= 7 * dir;
        theta1[3][1] += 7 * dir;
        theta1[3][2] -= 7 * dir;
    }

    dTorso[0] += 0.4;
    dTorso[1] += 1.4 * dir;
    
    if (!(motionFrames % 5)) {
        dir = -dir;
    }
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
    glRotatef(theta1[0][0], 0.0f, 0.0f, 1.0f);
    glRotatef(90.0, 0.0f, 1.0f, 0.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);
    torso_node.f = torso;
    torso_node.sibling = NULL;
    torso_node.child = &neck_node;

    //neck
    glLoadIdentity();
    glTranslatef(0.0, 0.0, (TORSO_HEIGHT));
    glRotatef(theta1[0][1] - 30, 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, neck_node.m);
    neck_node.f = neck;
    neck_node.sibling = &fru_node;
    neck_node.child = &head_node;

    //head
    glLoadIdentity();
    glTranslatef(0.0, 0.0, NECK_HEIGHT + HEAD_RADIUS/2);
    glRotatef(theta1[0][2], 0.0, 0.0, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, head_node.m);
    head_node.f = head;
    head_node.sibling = NULL;
    head_node.child = NULL;

    ////FRONT RIGHT LEG////
    //front right upper
    glLoadIdentity();
    glTranslatef(-TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT-FRONT_RIGHT_UPPER_RADIUS);
    glRotatef(theta1[1][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, fru_node.m);
    fru_node.f = fru;
    fru_node.sibling = &flu_node;  //pending//
    fru_node.child = &frl_node;

    //front right lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_RIGHT_UPPER_HEIGHT);
    glRotatef(theta1[1][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, frl_node.m);
    frl_node.f = frl;
    frl_node.sibling = NULL;
    frl_node.child = &frf_node; 

    //front right foot
    glLoadIdentity();
    glTranslatef(0.0, -FRONT_RIGHT_LOWER_RADIUS, FRONT_RIGHT_LOWER_HEIGHT+FRONT_RIGHT_FOOT_RADIUS);
    glRotatef(-theta1[1][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, frf_node.m);
    frf_node.f = frf;
    frf_node.sibling = NULL;
    frf_node.child = NULL;  //pending


    ////FRONT LEFT LEG////
    //front left upper
    glLoadIdentity();
    glTranslatef(TORSO_RADIUS, -TORSO_RADIUS, TORSO_HEIGHT - FRONT_LEFT_UPPER_RADIUS);
    glRotatef(theta1[2][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, flu_node.m);
    flu_node.f = flu;
    flu_node.sibling = &bru_node;  //pending//
    flu_node.child = &fll_node;

    //front left lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, FRONT_LEFT_UPPER_HEIGHT);
    glRotatef(theta1[2][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, fll_node.m);
    fll_node.f = fll;
    fll_node.sibling = NULL;
    fll_node.child = &flf_node;

    //front left foot
    glLoadIdentity();
    glTranslatef(0.0, -FRONT_LEFT_LOWER_RADIUS, FRONT_LEFT_LOWER_HEIGHT + FRONT_LEFT_FOOT_RADIUS);
    glRotatef(-theta1[2][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, flf_node.m);
    flf_node.f = flf;
    flf_node.sibling = NULL;
    flf_node.child = NULL;


    ////BACK RIGHT LEG////
    //back right upper
    glLoadIdentity();
    glTranslatef(-TORSO_RADIUS, -TORSO_RADIUS, BACK_RIGHT_UPPER_RADIUS);
    glRotatef(theta1[3][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, bru_node.m);
    bru_node.f = bru;
    bru_node.sibling = &blu_node;  //pending//
    bru_node.child = &brl_node;

    //back right lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, BACK_RIGHT_UPPER_HEIGHT);
    glRotatef(theta1[3][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, brl_node.m);
    brl_node.f = brl;
    brl_node.sibling = NULL;
    brl_node.child = &brf_node;

    //back right foot
    glLoadIdentity();
    glTranslatef(0.0, -BACK_RIGHT_LOWER_RADIUS, BACK_RIGHT_LOWER_HEIGHT + BACK_RIGHT_FOOT_RADIUS);
    glRotatef(-theta1[3][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, brf_node.m);
    brf_node.f = brf;
    brf_node.sibling = NULL;
    brf_node.child = NULL;  //pending


    ////BACK LEFT LEG////
    //back left upper
    glLoadIdentity();
    glTranslatef(TORSO_RADIUS, -TORSO_RADIUS, BACK_LEFT_UPPER_RADIUS);
    glRotatef(theta1[4][0], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, blu_node.m);
    blu_node.f = blu;
    blu_node.sibling = NULL;
    blu_node.child = &bll_node;

    //back left lower
    glLoadIdentity();
    glTranslatef(0.0, 0.0, BACK_LEFT_UPPER_HEIGHT);
    glRotatef(theta1[4][1], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, bll_node.m);
    bll_node.f = bll;
    bll_node.sibling = NULL;
    bll_node.child = &blf_node;

    //front left foot
    glLoadIdentity();
    glTranslatef(0.0, -BACK_LEFT_LOWER_RADIUS, BACK_LEFT_LOWER_HEIGHT + BACK_LEFT_FOOT_RADIUS);
    glRotatef(-theta1[4][2], 1.0, 0.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, blf_node.m);
    blf_node.f = blf;
    blf_node.sibling = NULL;
    blf_node.child = NULL;






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


    glTranslatef(250, 300, 0);

    glColor3f(0.5, 0.5, 0.5);

    if (keepMoving) {
        switch (motionMode) {
            case LIFT_FRONT_RIGHT_LEG:
                liftFrontRightLeg();
                break;

            case STAND_ON_BACK_LEGS:
                standOnBackLegs();
                break;

            case BEND_NECK_AND_HEAD:
                bendNeckAndHead();
                break;

            case WALK:
                walk();
                break;

            default:
                break;
        }
        motionFrames--;
    }

    glPushMatrix();
    drawDog();
    glPopMatrix();

    traverse(&torso_node);
    //moveBullet();
    
    glutSwapBuffers();

    if (keepMoving) {
        if (motionFrames <= 0) {
            keepMoving = false;
        }

        glutPostRedisplay();
    }
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
    case LIFT_FRONT_RIGHT_LEG:
        motionMode = LIFT_FRONT_RIGHT_LEG;
        keepMoving = true;
        motionFrames = 10;
        dir = 1;

        break;

    case STAND_ON_BACK_LEGS:
        motionMode = STAND_ON_BACK_LEGS;
        keepMoving = true;
        motionFrames = 10;
        dir = 1;
        
        break;

    case BEND_NECK_AND_HEAD:
        motionMode = BEND_NECK_AND_HEAD;
        keepMoving = true;
        motionFrames = 10;
        dir = 1;

        break;

    case WALK:
        motionMode = WALK;
        keepMoving = true;
        motionFrames = 399;
        dir = 1;

        break;

    case BACK_TO_NORMAL:
        dir = -dir;
        keepMoving = true;
        motionFrames = 10;

        break;

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
    glutAddMenuEntry("• Lift front right leg", LIFT_FRONT_RIGHT_LEG);
    glutAddMenuEntry("• Stand on back legs", STAND_ON_BACK_LEGS);
    glutAddMenuEntry("• Bend neck and head", BEND_NECK_AND_HEAD);
    glutAddMenuEntry("• Walk", WALK);
    glutAddMenuEntry("• Go back to normal", BACK_TO_NORMAL);
   /* glutCreateMenu(menu);
    glutAddMenuEntry("• Cubic Curves", CUBIC_CURVES);
    glutAddSubMenu("- Bezier", bezierMenu);
    glutAddMenuEntry("• Cubic Surface", CUBIC_SURFACE);*/
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutKeyboardFunc(keyboard_callback_func);

    myinit();
    glutMainLoop();

    return 0;
}