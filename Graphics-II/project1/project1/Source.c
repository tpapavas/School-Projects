#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define MAX_DISTANCE 15.0f

GLubyte my_texels[512][512][3];


bool leftMouseClicked;
bool movePoint;
GLint mvPntInd;
GLint clickX;
GLint clickY;

GLint maxWindowX = 640;
GLint maxWindowY = 480;

GLfloat M1[4][4] = {
    {     1,     0,     0,     0},
    {  -5.5,     9,  -4.5,     1},
    {     9, -22.5,    18,  -4.5},
    {  -4.5,  13.5, -13.5,   4.5}
};


GLfloat p[4][3] = {
    {150, 150, 0},
    {270, 180, 0},
    {310, 140, 0},
    {440, 190, 0}
};
GLfloat C[4][3];

void cubicPolynomialCurve(int);

void matrixMul(float A[4][4], float B[4][3], int aRows, int aCols, int bCols, float R[4][3]) {
//    float** R = (float*) malloc(sizeof(float)*rows);


    for (int i = 0; i < aRows; i++) {
        for (int z = 0; z < bCols; z++) {
            R[i][z] = 0;
            for (int j = 0; j < aCols; j++) {
             //   printf("(%d,%d)\n", i, j);
                R[i][z] += A[i][j] * B[j][z];
            }
        }
    } 
}

void myinit(void)
{
    matrixMul(M1, p, 4, 4, 3, C);

    /*
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.1f ", C[i][j]);
        }
        printf("\n");
    } */

    glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.3, 0.8, 0.1);
    glPointSize(2.0f);
    cubicPolynomialCurve(300);


    glPointSize(4.0f);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex3fv(p[0]);
    glVertex3fv(p[1]);
    glVertex3fv(p[2]);
    glVertex3fv(p[3]);
    glEnd();


    glutSwapBuffers();
}


void cubicPolynomialCurve(int numOfPoints) {
    GLfloat step = 1.0f / numOfPoints;
    GLfloat u = 0;
    GLfloat sum, uu;
    GLfloat point[3];

    for (int i = 0; i < numOfPoints; i++) {
        for (int coord = 0; coord < 3; coord++) {
            uu = 1.0f;
            sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += C[k][coord] * uu;
                uu *= u;
            }
            point[coord] = sum;
        }
        u += step;

        // printf("(%.1f, %.1f, %.1f)\n", point[0], point[1], point[2]);

        glBegin(GL_POINTS);
        glVertex3fv(point);
        glEnd();
    }
}

void mouse_callback_func(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            leftMouseClicked = true;

            clickX = x;
            clickY = maxWindowY - y;

            printf("(%d, %d)\n", x, y);

            GLfloat minDist = 12552.0f;
            GLfloat dist;
            GLint minI;
            for (int i = 0; i < 4; i++) {
                dist = pow(clickX - p[i][0], 2) + pow(clickY - p[i][1], 2);
                if (dist < minDist) {
                    minDist = dist;
                    minI = i;
                }
            }
            if (sqrt(minDist) < MAX_DISTANCE) {
                movePoint = true;
                mvPntInd = minI;
            }
        }
        else if (state == GLUT_UP && leftMouseClicked) {
            leftMouseClicked = false;
            movePoint = false;
          //  xCurr += x - xStart;
          //  yCurr += y - yStart;
        }
    }
}

void mouse_motion_callback_func(int x, int y) {
    if (leftMouseClicked) {

      //  x = maxWindowX - x;
        y = maxWindowY - y;

        printf("(%d, %d)\n", x, y);


        /*
        GLfloat minDist = 12552.0f;
        GLfloat dist;
        GLint minI;
        for (int i = 0; i < 4; i++) {
            dist = pow(x - p[i][0], 2) + pow(y - p[i][1], 2);
            if (dist < minDist) {
                minDist = dist;
                minI = i;
            }
        } */
        if (movePoint) {
            p[mvPntInd][0] += x - clickX;
            p[mvPntInd][1] += y - clickY;

            clickX += x - clickX;
            clickY += y - clickY;


            matrixMul(M1, p, 4, 4, 3, C);

            glutPostRedisplay();
        }
   //     shiftWindow(x - xStart, y - yStart);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(maxWindowX, maxWindowY);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("project1");
    glutDisplayFunc(display);

    glutMouseFunc(mouse_callback_func);
    glutMotionFunc(mouse_motion_callback_func);

    myinit();
    glutMainLoop();
    

    return 0;
}