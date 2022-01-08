#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define MAX_DISTANCE 15.0f

GLint windowId;
typedef enum {
    CUBIC_CURVES,
    CUBIC_SURFACE,
    EXIT
} Mode;


bool leftMouseClicked;
bool movePoint;
GLint mvPntInd;
GLint clickX;
GLint clickY;

GLint maxWindowX = 640;
GLint maxWindowY = 480;

//GLfloat C[4][3];  //C array for cubic curve
GLfloat CS[4][4][3];  //C array for cubic surface

GLfloat** C;
GLfloat** U;
GLfloat** V;
GLfloat** P;
GLfloat** p;
GLfloat** M1;
GLfloat** point;

void cubicPolynomialCurve(int);

GLfloat** matrixNew(GLint rows, GLint cols) {
    GLfloat** R = (GLfloat**)calloc(rows, sizeof(GLfloat) * rows);
    for (int i = 0; i < rows; i++) {
        R[i] = (GLfloat*)calloc(cols, sizeof(GLfloat) * cols);
    }

    return R;
}

void matrixMul(GLfloat** A, GLfloat** B, int aRows, int aCols, int bCols, GLfloat** R) {
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

void vectorInit(GLfloat* V, GLfloat arr[], GLint size) {
    for (int i = 0; i < size; i++) {
        V[i] = arr[i];
    }
}

void matrixMul4x4(float A[4][4], float B[4][4], int aRows, int aCols, int bCols, float R[4][4]) {
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

void matrixMul4x3(float A[4][4], float B[4][3], int aRows, int aCols, int bCols, float R[4][3]) {
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

void matrixCopy(GLfloat* new, GLfloat old[]) {
    
}

void myinit(void)
{
    U = matrixNew(1, 4);  //1x4 matrix
    V = matrixNew(4, 1);  //4x1 matrix
    C = matrixNew(4, 3);
    p = matrixNew(4, 3);
    M1 = matrixNew(4, 4);
    point = matrixNew(1, 3);
    //matrixMul4x3(M1, p, 4, 4, 3, C);
    

    GLfloat tempM1[4][4] = {
        {     1,     0,     0,     0},
        {  -5.5,     9,  -4.5,     1},
        {     9, -22.5,    18,  -4.5},
        {  -4.5,  13.5, -13.5,   4.5}
    };

    GLfloat tempPoints[4][3] = {
        {150, 150, 0},
        {270, 180, 0},
        {310, 140, 0},
        {440, 190, 0}
    };

    for (int i = 0; i < 4; i++) {
        vectorInit(p[i], tempPoints[i], 3);
     /*   for (int j = 0; j < 3; j++) {
            p[i][j] = tempPoints[i][j];
        } */
    }
    for (int i = 0; i < 4; i++) {
        vectorInit(M1[i], tempM1[i], 4);
      /*  for (int j = 0; j < 4; j++) {
            M1[i][j] = tempM1[i][j];
        } */
    }
    
    
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

void cubicPolynomialSurface(GLint numOfuPoints, int numOfvPoints) {
    GLfloat uStep = 1.0f / numOfuPoints;
    GLfloat vStep = 1.0f / numOfvPoints;
    GLfloat u = 0, v = 0;
    GLfloat uu, vv, sum;
    GLfloat point[3];


    for (u; u < 1; u += uStep) {
        for (v; v < 1; v += vStep) {
            for (int coord = 0; coord < 3; coord++) {
                uu = 1.0f;
                vv = 1.0f;
                sum = 0;

            }
        }
    }

}

void cubicPolynomialCurve(int numOfPoints) {
    GLfloat step = 1.0f / numOfPoints;
    GLfloat u = 0;
    GLfloat sum, uu;
  //  GLfloat point[3];

    /*
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
    } */

    for (int i = 0; i < numOfPoints; i++) {
        uu = 1.0f;
        for (int pow = 0; pow < 4; pow++) {
            U[0][pow] = uu;
            uu *= u;
        }
        matrixMul(U, C, 1, 4, 3, point);
        u += step;

     //   printf("(%.1f, %.1f, %.1f)\n", point[0][0], point[0][1], point[0][2]);

        glBegin(GL_POINTS);
        glVertex3fv(point[0]);
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

void menu(int id) {
    switch (id) {
    case CUBIC_CURVES:
        break;

    case CUBIC_SURFACE:
        break;

    case EXIT:
        glutDestroyWindow(windowId);
        exit(0);

    default:
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(maxWindowX, maxWindowY);
    glutInitWindowPosition(10, 10);
    windowId = glutCreateWindow("project1");
    glutDisplayFunc(display);

    glutMouseFunc(mouse_callback_func);
    glutMotionFunc(mouse_motion_callback_func);

    glutCreateMenu(menu);
    glutAddMenuEntry("Cubic Curves", CUBIC_CURVES);
    glutAddMenuEntry("Cubic Surface", CUBIC_SURFACE);
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myinit();
    glutMainLoop();
    

    return 0;
}