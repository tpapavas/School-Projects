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
    BEZIER_ORDER_6,
    CUBIC_BEZIER,
    SHOW_CONVEX,
    CUBIC_CURVES_BEZ,
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
GLfloat tempMB[4][4] = {
        {  1,  0,  0,  0},
        { -3,  3,  0,  0},
        {  3, -6,  3,  0},
        { -1,  3, -3,  1}
};
GLfloat tempMBIn[4][4] = {
        {     1,     0,     0,     0},
        {     1, 1/3.0,     0,     0},
        {     1, 2/3.0, 1/3.0,     0},
        {     1,     1,     1,     1}
};

GLfloat tempPoints[7][3] = {
    {150, 150, 0},
    {240, 180, 0},
    {360, 140, 0}, //collinear
    {400, 170, 0}, //collinear
    {440, 200, 0}, //collinear
    {470, 140, 0},
    {500, 190, 0},
};

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
bool movePoint;
GLint mvPntInd;
GLint clickX;
GLint clickY;

GLint maxWindowX = 640;
GLint maxWindowY = 480;

//GLfloat C[4][3];  //C array for cubic curve
//GLfloat CS[4][4][3];  //C array for cubic surface

GLfloat** C1;
GLfloat** C2;

GLfloat** U;
GLfloat** V;
GLfloat** P;

GLfloat** p;
GLfloat** ep;

GLfloat** M1;
GLfloat** M1T;
GLfloat** point;

GLfloat*** PS;
GLfloat*** CS;

GLfloat** tempArr;
GLfloat** tempVert;
GLfloat** tempEl;
GLfloat** subP;

void cubicPolynomialCurve(GLint);
void cubicPolynomialSurface(GLint, GLint);
void bezier(GLint, GLint, GLint);

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

void submatrix(GLfloat** M, GLfloat** subM, GLint iStart, GLint jStart, GLint rows, GLint cols) {
    for (int i = iStart; i < rows+iStart; i++) {
        for (int j = jStart; j < cols+jStart; j++) {
            subM[i-iStart][j-jStart] = M[i][j];
        }
    }
}

void myinit(void)
{
    tempArr = matrixNew(4, 4);
    tempVert = matrixNew(1, 4);
    tempEl = matrixNew(1, 1);

    U = matrixNew(1, 4);  //1x4 matrix
    V = matrixNew(4, 1);  //4x1 matrix
    
    C1 = matrixNew(4, 3);
    C2 = matrixNew(4, 3);
    
    p = matrixNew(7, 3);
    ep = matrixNew(7, 3);
    subP = matrixNew(4, 3);
    
    M1 = matrixNew(4, 4);
    M1T = matrixNew(4, 4);
    point = matrixNew(1, 3);

    //initialization on cubic polynomial curve
    currState.mode = CUBIC_CURVES;
    currState.numOfPoints = 7;
    currState.setPointsLeft = 7;
    currState.pointsSet = false;
    currState.lim1 = 0;
    currState.lim2 = -1;

    for (int i = 0; i < 4; i++) {
        vectorInit(M1[i], tempM1[i], 4);
    }
    for (int i = 0; i < 4; i++) {
        vectorInit(M1T[i], tempMBIn[i], 4);
    }

    matrixMul(M1T, M1, 4, 4, 4, tempArr);  //Mb-1 x M
    submatrix(tempArr, M1, 0, 0, 4, 4);

    //matrixMul4x3(M1, p, 4, 4, 3, C);




  /*  currState.mode = CUBIC_CURVES;
    currState.numOfPoints = 4;
    currState.pointsSet = false;
    currState.setPointsLeft = 7;
    currState.showConvex = false;
    currState.lim1 = 0;
    currState.lim2 = -1;
    
    for (int i = 0; i < 4; i++) {
        vectorInit(M1[i], tempM1[i], 4);
        vectorInit(M1T[i], tempM1T[i], 4);
    } 

    submatrix(p, subP, 0, 0, 4, 3);
    matrixMul(M1, subP, 4, 4, 3, C1);
       
    submatrix(p, subP, 3, 0, 4, 3);
    matrixMul(M1, subP, 4, 4, 3, C2);
*/ //for poly curve

    /*
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.1f ", C[i][j]);
        }
        printf("\n");
    } */

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MAP1_VERTEX_3);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xViewer, yViewer, zViewer, xref, yref, zref, Vx, Vy, Vz);


    glClearColor(1.0, 1.0, 1.0, 0.0);

    //set up viewing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, maxWindowX, 0, maxWindowY, -500, 500);
}

void _bezier(GLfloat** points, GLint order) {
    GLfloat tmpP[100];
    for (int i = 0; i < order; i++) {
        for (int j = 0; j < 3; j++) {
            tmpP[i*3 + j] = points[i][j];
        }
    }
   // printf("\n");


    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, order, tmpP);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; i++) {
        glEvalCoord1f(i / 100.f);
    }
    glEnd();
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



    if (currState.mode != CUBIC_SURFACE) {
        glColor3f(0.3, 0.8, 0.1);
        glPointSize(2.0f);


        if (currState.pointsSet) {
            if (currState.mode == CUBIC_CURVES) {
                submatrix(p, subP, 0, 0, 4, 3);
                _bezier(subP, 4);
                submatrix(p, subP, 3, 0, 4, 3);
                _bezier(subP, 4);
    //            cubicPolynomialCurve(300, C1);
    //            cubicPolynomialCurve(300, C2);
            } else if (currState.mode == CUBIC_BEZIER) {
                //bezier(7, 6, 200);
                submatrix(p, subP, 0, 0, 4, 3);
                _bezier(subP, 4);
                submatrix(p, subP, 3, 0, 4, 3);
                _bezier(subP, 4);

            } else {
                _bezier(p, 7);
            }
        }

        glPointSize(4.0f);
        glColor3f(0.0, 0.0, 0.0);
        GLint numOfPoints = 7 - currState.setPointsLeft;
        glBegin(GL_POINTS);
        for (int i = 0; i < numOfPoints; i++) {
         //   printf("(%f, %f)\n", ep[i][0], ep[i][1]);
            glVertex3fv(ep[i]);
        }
        glEnd();


        //print convex hull
        if (currState.showConvex) {
            glColor3f(0.85, 0.85, 0.85);
            for (int i = 0; i < currState.lim1; i++) {
                for (int j = i; j < currState.lim1; j++) {
                    glBegin(GL_LINES);
                    glVertex3fv(ep[i]);
                    glVertex3fv(ep[j]);
                    glEnd();
                }
            }
            for (int i = currState.lim1 - 1; i < currState.lim2; i++) {
                for (int j = i; j < currState.lim2; j++) {
                    glBegin(GL_LINES);
                    glVertex3fv(ep[i]);
                    glVertex3fv(ep[j]);
                    glEnd();
                }
            }
        }
    } else {
        glColor3f(0.3, 0.8, 0.1);
        glPointSize(2.0f);

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
                point[coord] = tempEl[0][0];
            }
       //     printf("%.1f, %.1f, %.1f\n", point[0], point[1], point[2]);
            glColor3f(u, v, u);
            glBegin(GL_POINTS);
            glVertex3fv(point);
            glEnd();
        }
    }

}

void cubicPolynomialCurve(int numOfPoints, GLfloat** C) {
    GLfloat step = 1.0f / numOfPoints;
    GLfloat u = 0;
    GLfloat sum, uu;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= numOfPoints; i++) {
        uu = 1.0f;
        for (int pow = 0; pow < 4; pow++) {
            U[0][pow] = uu;
            uu *= u;
        }
        matrixMul(U, C, 1, 4, 3, point);
        u += step;

     //   printf("(%.1f, %.1f, %.1f)\n", point[0][0], point[0][1], point[0][2]);

        //glBegin(GL_POINTS);
        glVertex3fv(point[0]);
        //glEnd();
    }
    glEnd();

}

void bezier(GLint order, GLint degree, GLint numOfPoints) {
    GLfloat step = 1.0f / numOfPoints;
    GLfloat sum, uu, u1;
    GLfloat u = 0.0f;

    GLfloat point[3];
    GLfloat U[7], U1[7], Sel[7];

    Sel[0] = 1;
    //Sel[1] = order * (order - 1) / 2;
    for (int i = 1; i < order; i++) {
        Sel[i] = Sel[i - 1] * (degree - i + 1) / i;
    }
    printf("%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f\n", Sel[0], Sel[1], Sel[2], Sel[3], Sel[4], Sel[5], Sel[6]);


    for (int pts = 0; pts < numOfPoints; pts++) {
        uu = 1.0f;
        u1 = 1.0f;
        for (int pow = 0; pow < order; pow++) {
            U[pow] = uu;
            U1[pow] = u1;
            uu *= u;
            u1 *= (1 - u);
        }
        for (int coord = 0; coord < 3; coord++) {
            sum = 0;
            for (int i = 0; i < order; i++) {
                sum += Sel[i] * U[i] * U1[degree - i] * p[i][coord];
            }
            point[coord] = sum;
        }

        u += step;

        printf("(%.1f, %.1f, %.1f)\n", point[0], point[1], point[2]);

        glBegin(GL_POINTS);
        glVertex3fv(point);
        glEnd();
    }
}

void clearMatrices() {
    switch (currState.mode) {
        case CUBIC_CURVES:
            matrixDel(p, 4, 3);
            break;

        case CUBIC_SURFACE:
            break;
            
    }
}

void mouse_callback_func(int button, int state, int x, int y) {
    clickX = x;
    clickY = maxWindowY - y;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            if (currState.pointsSet) {
                leftMouseClicked = true;

                printf("(%d, %d)\n", x, y);

                GLfloat minDist = 12552.0f;
                GLfloat dist;
                GLint minI;
                for (int i = 0; i < 7; i++) {
                    dist = pow(clickX - ep[i][0], 2) + pow(clickY - ep[i][1], 2);
                    if (dist < minDist) {
                        minDist = dist;
                        minI = i;
                    }
                }
                if (sqrt(minDist) < MAX_DISTANCE) {
                    movePoint = true;
                    mvPntInd = minI;
                }
            } else if (currState.setPointsLeft) {  //setting points
                GLint index = 7 - currState.setPointsLeft;
                
                vectorInit(ep[index], (GLfloat[]) { clickX, clickY, 0 }, 3);
                currState.setPointsLeft--;

                printf("(%f, %f)\n", ep[index][0], ep[index][1]);
                if (!currState.setPointsLeft) {
               //     currState.setPointsLeft = 7;
                    currState.pointsSet = true;

                    if (currState.mode == CUBIC_CURVES) {
                        submatrix(ep, C1, 0, 0, 4, 3);
                        matrixMul(M1, C1, 4, 4, 3, subP);

                        printf("\n");
                        for (int i = 0; i < 4; i++) {
                            for (int j = 0; j < 3; j++) {
                                p[i][j] = subP[i][j];
                            }
                            printf("(%f, %f)\n", p[i][0], p[i][1]);
                        }

                        submatrix(ep, C1, 3, 0, 4, 3);
                        matrixMul(M1, C1, 4, 4, 3, subP);

                        for (int i = 3; i < 7; i++) {
                            for (int j = 0; j < 3; j++) {
                                p[i][j] = subP[i - 3][j];
                            }
                        }
                        glutPostRedisplay();
                        return;
                    } /*if (currState.mode == CUBIC_CURVES) {
                        submatrix(ep, subP, 0, 0, 4, 3);
                        matrixMul(M1, subP, 4, 4, 3, C1);
                        submatrix(ep, subP, 3, 0, 4, 3);
                        matrixMul(M1, subP, 4, 4, 3, C2);
                    } */
                } else if (currState.mode == BEZIER_ORDER_6 && currState.setPointsLeft == 1) {
                    currState.setPointsLeft = 0;
                    currState.pointsSet = true;

                    vectorInit(ep[6], ep[0], 3);
                }
                submatrix(ep, p, 0, 0, 7, 3);
                glutPostRedisplay();
            }
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

        if (movePoint) {
            ep[mvPntInd][0] += x - clickX;
            ep[mvPntInd][1] += y - clickY;

            if (currState.mode == CUBIC_BEZIER) {
                switch (mvPntInd) {
                    case 3:
                        ep[2][0] += x - clickX;
                        ep[2][1] += y - clickY;
                        ep[4][0] += x - clickX;
                        ep[4][1] += y - clickY;
                        
                        break;

                    case 2:
                    case 4:
                        ep[6 - mvPntInd][0] -= x - clickX;
                        ep[6 - mvPntInd][1] -= y - clickY;
                        break;
                }
            } else if (currState.mode == CUBIC_CURVES) {
                submatrix(ep, C1, 0, 0, 4, 3);
                matrixMul(M1, C1, 4, 4, 3, subP);

                printf("\n");
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 3; j++) {
                        p[i][j] = subP[i][j];
                    }
                    printf("(%f, %f)\n", p[i][0], p[i][1]);
                }

                submatrix(ep, C1, 3, 0, 4, 3);
                matrixMul(M1, C1, 4, 4, 3, subP);

                for (int i = 3; i < 7; i++) {
                    for (int j = 0; j < 3; j++) {
                        p[i][j] = subP[i - 3][j];
                    }
                }clickX += x - clickX;
                clickY += y - clickY;
                glutPostRedisplay();
                return;
                
            }

            if (currState.mode == BEZIER_ORDER_6 && mvPntInd % 6 == 0) {
                ep[6 - mvPntInd][0] = ep[mvPntInd][0];
                ep[6 - mvPntInd][1] = ep[mvPntInd][1];
            }

            clickX += x - clickX;
            clickY += y - clickY;

            submatrix(ep, subP, 0, 0, 4, 3);
            matrixMul(M1, subP, 4, 4, 3, C1);
            submatrix(ep, subP, 3, 0, 4, 3);
            matrixMul(M1, subP, 4, 4, 3, C2);
           // matrixMul(M1, p, 4, 4, 3, C1);

            submatrix(ep, p, 0, 0, 7, 3);
            
            glutPostRedisplay();
        }
    }
}

void menu(int id) {
    if (currState.mode == id) {
        return;
    }

    switch (id) {
  /*  case CUBIC_CURVES:
        clearMatrices();

        p = matrixNew(7, 3);

        currState.mode = CUBIC_CURVES;
        currState.numOfPoints = 7;
        currState.setPointsLeft = 7;
        currState.pointsSet = false;
        currState.lim1 = 0;
        currState.lim2 = -1;

        for (int i = 0; i < 4; i++) {
            vectorInit(M1[i], tempM1[i], 4);
        }

        submatrix(p, subP, 0, 0, 4, 3);
        matrixMul(M1, subP, 4, 4, 3, C1);
        submatrix(p, subP, 3, 0, 4, 3);
        matrixMul(M1, subP, 4, 4, 3, C2);

        break; */

    case CUBIC_SURFACE:
        PS = (GLfloat***)malloc(sizeof(GLfloat) * 3);
        for (int i = 0; i < 3; i++) {
            PS[i] = matrixNew(4, 4);
        }
        CS = (GLfloat***)malloc(sizeof(GLfloat) * 3);
        for (int i = 0; i < 3; i++) {
            CS[i] = matrixNew(4, 4);
        }
        for (int i = 0; i < 4; i++) {
            vectorInit(M1[i], tempM1[i], 4);
            vectorInit(M1T[i],  tempM1T[i], 4);
        }

        CSvectorInit(0, 0, (GLfloat[]) { 0, 100, 100 }); //front-left 
        CSvectorInit(0, 3, (GLfloat[]) { 0, 100, 300 }); //front-right
        CSvectorInit(3, 0, (GLfloat[]) { 0, 300, 100 }); //back-left        
        CSvectorInit(3, 3, (GLfloat[]) { 0, 300, 300 }); //back-right

        CSvectorInit(0, 1, (GLfloat[]) { 0, 110, 140 });
        CSvectorInit(0, 2, (GLfloat[]) { 0, 110, 190 });

        CSvectorInit(1, 0, (GLfloat[]) { 100, 150, 100 });
        CSvectorInit(1, 1, (GLfloat[]) { 100, 150, 120 });
        CSvectorInit(1, 2, (GLfloat[]) { 100, 150, 170 });
        CSvectorInit(1, 3, (GLfloat[]) { 100, 150, 190 });

        CSvectorInit(2, 0, (GLfloat[]) { 50, 230, 100 });
        CSvectorInit(2, 1, (GLfloat[]) { 50, 230, 120 });
        CSvectorInit(2, 2, (GLfloat[]) { 50, 230, 200 });
        CSvectorInit(2, 3, (GLfloat[]) { 50, 230, 240 });

        CSvectorInit(3, 1, (GLfloat[]) { 0, 290, 150 });
        CSvectorInit(3, 2, (GLfloat[]) { 0, 290, 190 });

        for (int coord = 0; coord < 3; coord++) {
            matrixMul(M1, PS[coord], 4, 4, 4, tempArr);
            matrixMul(tempArr, M1T, 4, 4, 4, CS[coord]);
        }

        /*
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%.2f ", CS[2][i][j]);
            }
            printf("\n");
        } */

        currState.mode = CUBIC_SURFACE;

        break;

    case CUBIC_BEZIER:
        currState.mode = CUBIC_BEZIER;
        currState.numOfPoints = 7;
        currState.setPointsLeft = 0;
        currState.pointsSet = true;
        currState.lim1 = 4;
        currState.lim2 = 7;
        
        p = matrixNew(7, 3);
        for (int i = 0; i < 7; i++) {
            vectorInit(ep[i], tempPoints[i], 3);
            vectorInit(p[i], tempPoints[i], 3);
        }    

        for (int i = 0; i < 4; i++) {
            vectorInit(M1[i], tempMB[i], 4);
        }

        submatrix(ep, subP, 0, 0, 4, 3);
        matrixMul(M1, subP, 4, 4, 3, C1);
        submatrix(ep, subP, 3, 0, 4, 3);
        matrixMul(M1, subP, 4, 4, 3, C2);

        break;

    case BEZIER_ORDER_6:
        currState.mode = BEZIER_ORDER_6;
        currState.numOfPoints = 7;
        currState.setPointsLeft = 7;
        currState.pointsSet = false;
        currState.lim1 = 7;
        currState.lim2 = -1;

        ep = matrixNew(7, 3);
        for (int i = 0; i < 6; i++) {
            vectorInit(ep[i], tempPoints[i], 3);
        }
        vectorInit(ep[6], tempPoints[0], 3);
        break;

    case SHOW_CONVEX:
        if (currState.mode == CUBIC_BEZIER || currState.mode == BEZIER_ORDER_6) {
            currState.showConvex = !currState.showConvex;
        } else {
            return;
        }
        break;

    case CUBIC_CURVES:
        ep = matrixNew(7, 3);

        currState.mode = CUBIC_CURVES;
        currState.numOfPoints = 7;
        currState.setPointsLeft = 7;
        currState.pointsSet = false;
        currState.lim1 = 0;
        currState.lim2 = -1;

        for (int i = 0; i < 4; i++) {
            vectorInit(M1[i], tempM1[i], 4);
        }
        for (int i = 0; i < 4; i++) {
            vectorInit(M1T[i], tempMBIn[i], 4);
        }

        matrixMul(M1T, M1, 4, 4, 4, tempArr);  //Mb-1 x M
        submatrix(tempArr, M1, 0, 0, 4, 4);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                printf("%.1f ", M1[i][j]);
            }
            printf("\n");
        }
        

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

    GLint bezierMenu = glutCreateMenu(menu);
    glutAddMenuEntry("• Bezier Order 6", BEZIER_ORDER_6);
    glutAddMenuEntry("• Cubic Bezier", CUBIC_BEZIER);
    glutAddMenuEntry("Show/Hide Convex Hull", SHOW_CONVEX);
    glutCreateMenu(menu);
    glutAddMenuEntry("• Cubic Curves", CUBIC_CURVES);
    glutAddMenuEntry("• Cubic Curves (w Bezier)", CUBIC_CURVES_BEZ);
    glutAddSubMenu("- Bezier", bezierMenu);
    glutAddMenuEntry("• Cubic Surface", CUBIC_SURFACE);
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutKeyboardFunc(keyboard_callback_func);

    myinit();
    glutMainLoop();
    

    return 0;
}