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
typedef enum {
    CUBIC_CURVES,
    CUBIC_SURFACE,
    EXIT
} Mode;

GLfloat tempM1[4][4] = {
        {     1,     0,     0,     0},
        {  -5.5,     9,  -4.5,     1},
        {     9, -22.5,    18,  -4.5},
        {  -4.5,  13.5, -13.5,   4.5}
};
GLfloat tempM1T[4][4] = {
        {     1,  -5.5,     9,  -4.5},
        {     0,     9, -22.5,  13.5},
        {     0,  -4.5,    18, -13.5},
        {     0,     1,  -4.5,   4.5}
};

GLfloat tempPoints[4][3] = {
    {150, 150, 0},
    {270, 180, 0},
    {310, 140, 0},
    {440, 190, 0}
};

struct State {
    Mode mode;
    GLint numOfPoints;
} state;

bool leftMouseClicked;
bool movePoint;
GLint mvPntInd;
GLint clickX;
GLint clickY;

GLint maxWindowX = 640;
GLint maxWindowY = 480;

//GLfloat C[4][3];  //C array for cubic curve
//GLfloat CS[4][4][3];  //C array for cubic surface

GLfloat** C;
GLfloat** U;
GLfloat** V;
GLfloat** P;
GLfloat** p;
GLfloat** M1;
GLfloat** M1T;
GLfloat** point;
GLfloat*** PS;
GLfloat*** CS;
GLfloat** tempArr;
GLfloat** tempVert;
GLfloat** tempEl;

void cubicPolynomialCurve(GLint);
void cubicPolynomialSurface(GLint, GLint);

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

void CSvectorInit(GLint row, GLint col, GLfloat arr[]) {
    for (int i = 0; i < 3; i++) {
        PS[i][row][col] = arr[i];
    }
}

/*
* converts a CSpoint(PS{0,1,2}[i][j]) to vector (1x3 array)
*/
void CSpoint2vector(GLfloat**p, GLint row, GLint col) {
    p[0][0] = PS[0][row][col];
    p[0][1] = PS[1][row][col];
    p[0][2] = PS[2][row][col];
}

void matrixDel(GLfloat** M, GLint rows, GLint cols) {
    for (int i = 0; i < rows; i++) {
        free(M[i]);
    }
    free(M);
}

void myinit(void)
{
    tempArr = matrixNew(4, 4);
    tempVert = matrixNew(1, 4);
    tempEl = matrixNew(1, 1);

    U = matrixNew(1, 4);  //1x4 matrix
    V = matrixNew(4, 1);  //4x1 matrix
    C = matrixNew(4, 3);
    p = matrixNew(4, 3);
    M1 = matrixNew(4, 4);
    M1T = matrixNew(4, 4);
    point = matrixNew(1, 3);
    //matrixMul4x3(M1, p, 4, 4, 3, C);

    state.mode = CUBIC_CURVES;
    state.numOfPoints = 4;
    
    for (int i = 0; i < 4; i++) {
        vectorInit(p[i], tempPoints[i], 3);
        vectorInit(M1[i], tempM1[i], 4);
        vectorInit(M1T[i], tempM1T[i], 4);
    }
    
    matrixMul(M1, p, 4, 4, 3, C);

    /*
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.1f ", C[i][j]);
        }
        printf("\n");
    } */

    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xViewer, yViewer, zViewer, xref, yref, zref, Vx, Vy, Vz);


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

   // printf("(%.1f,%.1f)\n",xr,zr);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xr, yViewer, zr, xref, yref, zref, Vx, Vy, Vz);

    //for points
  //  glMatrixMode(GL_MODELVIEW);
 //  glLoadIdentity();

    glColor3f(0.3, 0.8, 0.1);
    glPointSize(2.0f);

    switch (state.mode) {
        case CUBIC_CURVES:
            cubicPolynomialCurve(300);


            glPointSize(4.0f);
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_POINTS);
            glVertex3fv(p[0]);
            glVertex3fv(p[1]);
            glVertex3fv(p[2]);
            glVertex3fv(p[3]);
            glEnd();

        //    printf("%f\n", p[0][0]);

            break;

        case CUBIC_SURFACE:
            cubicPolynomialSurface(200, 200);

            glPointSize(4.0f);
            glColor3f(0.0, 0.0, 0.0);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    CSpoint2vector(point, i, j);
                    glBegin(GL_POINTS);
                    glVertex3fv(point[0]);
                    glEnd();
                }
            }
            break;
    }
    
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -500);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 500, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(500, 0, 0);
    glEnd();
    glutSwapBuffers();
}

void cubicPolynomialSurface(GLint numOfuPoints, GLint numOfvPoints) {
    GLfloat uStep = 1.0f / numOfuPoints;
    GLfloat vStep = 1.0f / numOfvPoints;
    GLfloat u = 0;
    GLfloat v = 0;
    GLfloat uu, vv, sum;
    GLfloat point[3];


    for (u = 0; u < 1; u += uStep) {
        for (v = 0; v < 1; v += vStep) {
            uu = 1.0f;
            vv = 1.0f;
            for (int pow = 0; pow < 4; pow++) {
                U[0][pow] = uu;
                V[pow][0] = vv;
                uu *= u;
                vv *= v;
            }

            for (int coord = 0; coord < 3; coord++) {
                matrixMul(U, CS[coord], 1, 4, 4, tempVert);
                matrixMul(tempVert, V, 1, 4, 1, tempEl);
               // printf("(%f)\n", V[0][0]);
                point[coord] = tempEl[0][0];
            }

            glColor3f(u, v, u);
            glBegin(GL_POINTS);
            glVertex3fv(point);
            glEnd();
        }
    }

}

void cubicPolynomialCurve(int numOfPoints) {
    GLfloat step = 1.0f / numOfPoints;
    GLfloat u = 0;
    GLfloat sum, uu;

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

void clearMatrices() {
    switch (state.mode) {
        case CUBIC_CURVES:
            matrixDel(p, 4, 3);
            break;

        case CUBIC_SURFACE:
            break;
            
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
    if (state.mode == id) {
        return;
    } else {
        clearMatrices();
    }

    switch (id) {
    case CUBIC_CURVES:
        p = matrixNew(4, 3);

        state.mode = CUBIC_CURVES;
        state.numOfPoints = 4;

        for (int i = 0; i < 4; i++) {
            vectorInit(p[i], tempPoints[i], 3);
        }

        matrixMul(M1, p, 4, 4, 3, C);

        break;

    case CUBIC_SURFACE:
        PS = (GLfloat***)malloc(sizeof(GLfloat) * 3);
        for (int i = 0; i < 3; i++) {
            PS[i] = matrixNew(4, 4);
        }
        CS = (GLfloat***)malloc(sizeof(GLfloat) * 3);
        for (int i = 0; i < 3; i++) {
            CS[i] = matrixNew(4, 4);
        }
        /*
        CS = matrixNew(3, 4);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                CS[i][j] = (GLfloat*) malloc(sizeof(GLfloat) * 4);
            }
        }*/
        CSvectorInit(0, 0, (GLfloat[]) { 100, 100, 0 }); //front-left 
        CSvectorInit(0, 3, (GLfloat[]) { 300, 100, 0 }); //front-right
        CSvectorInit(3, 0, (GLfloat[]) { 100, 300, 0 }); //back-left        
        CSvectorInit(3, 3, (GLfloat[]) { 300, 300, 0 }); //back-right
        
        CSvectorInit(0, 1, (GLfloat[]) { 140, 110, 0 });
        CSvectorInit(0, 2, (GLfloat[]) { 190, 110, 0 });

        CSvectorInit(1, 0, (GLfloat[]) { 100, 150, -100 });
        CSvectorInit(1, 1, (GLfloat[]) { 120, 150, -100 });
        CSvectorInit(1, 2, (GLfloat[]) { 170, 150, -100 });
        CSvectorInit(1, 3, (GLfloat[]) { 190, 150, -100 });

        CSvectorInit(2, 0, (GLfloat[]) { 100, 230, -50 });
        CSvectorInit(2, 1, (GLfloat[]) { 120, 230, -50 });
        CSvectorInit(2, 2, (GLfloat[]) { 200, 230, -50 });
        CSvectorInit(2, 3, (GLfloat[]) { 240, 230, -50 });
        
        CSvectorInit(3, 1, (GLfloat[]) { 150, 290, 0 });
        CSvectorInit(3, 2, (GLfloat[]) { 190, 290, 0 });

        for (int coord = 0; coord < 3; coord++) {
            matrixMul(M1, PS[coord], 4, 4, 4, tempArr);
            matrixMul(tempArr, M1T, 4, 4, 4, CS[coord]);
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%.2f ", CS[2][i][j]);
            }
            printf("\n");
        }

        state.mode = CUBIC_SURFACE;

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
    } else if (key == 'd') {
        theta -= 10;
    } else if (key == 'a') {
        theta += 10;
    } else if (key == 's') {
        yViewer -= 10;
    } else if (key == 'r') {
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

    glutCreateMenu(menu);
    glutAddMenuEntry("Cubic Curves", CUBIC_CURVES);
    glutAddMenuEntry("Cubic Surface", CUBIC_SURFACE);
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutKeyboardFunc(keyboard_callback_func);

    myinit();
    glutMainLoop();
    

    return 0;
}


/*
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
} */