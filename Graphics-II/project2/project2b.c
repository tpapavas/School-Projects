#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define _USE_MATH_DEFINES

#include <math.h>

#define MAX_DISTANCE 15.0f
#define MAX_PARTICLES 1024

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
    SHOW_BOX,
    WIND,
    UNBOUNDED,
    EXIT
} Mode;

bool showBox = false;

typedef struct particle {
    GLfloat p[3];
    GLfloat v[3];
    GLfloat m;
} particle;

GLfloat g[3] = { 0.0, -1.0, 0.0 };  //gravity
GLfloat w[3] = { 0.0, 0.0, 0.0 };  //wind

particle particles[1024];
GLint numOfParticles = 0;
GLint createParticle = 0;
GLfloat bound = 200.0;

void newParticle(GLint i) {
    particles[i].p[0] = 0.0;
    particles[i].p[1] = 140.0;
    particles[i].p[2] = 0.0;

    particles[i].v[0] = (rand() / (float)RAND_MAX - 0.5) * 12;
    particles[i].v[1] = (rand() / (float)RAND_MAX - 0.5) * 1;
    particles[i].v[2] = (rand() / (float)RAND_MAX - 0.5) * 12;

    particles[i].m = 0.5;

    numOfParticles = (numOfParticles+1)% MAX_PARTICLES;
}

bool outOfBounds(GLfloat p[3]) {
    for (int i = 0; i < 3; i++)
        if (p[i] > bound || p[i] < -bound)
            return true;
    return false;
}

void collisionDetection(GLfloat p[3], GLfloat prevPosition[3] ,GLfloat prevVelocity[3]) {
    GLfloat newVn[3];
    GLfloat Vt[3];
    
    if (p[1] < 0) {
        GLfloat t = p[1] / (p[1] - prevPosition[1]);
            
        //new velocity after collision
        newVn[0] = 0;
        newVn[1] = -prevVelocity[1];
        newVn[2] = 0;

        Vt[0] = prevVelocity[0];
        Vt[1] = 0;
        Vt[2] = prevVelocity[2];

        //new v
        prevVelocity[0] = newVn[0] + Vt[0];
        prevVelocity[1] = newVn[1] + Vt[1];
        prevVelocity[2] = newVn[2] + Vt[2];


        //point on collosion with surface
        prevPosition[0] = p[0] - t * (p[0] - prevPosition[0]);
        prevPosition[1] = 0;
        prevPosition[2] = p[2] - t * (p[2] - prevPosition[2]);
    }
}

GLfloat h = 0.006;

void moveParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        GLfloat newPosition[3];
        newPosition[0] = particles[i].p[0] + h * particles[i].v[0];
        newPosition[1] = particles[i].p[1] + h * particles[i].v[1];
        newPosition[2] = particles[i].p[2] + h * particles[i].v[2];
        collisionDetection(newPosition, particles[i].p, particles[i].v);

        particles[i].p[0] += h * particles[i].v[0];
        particles[i].p[1] += h * particles[i].v[1];
        particles[i].p[2] += h * particles[i].v[2];

        particles[i].v[0] += h * w[0] / particles[i].m; // g[0] / particles[i].m;
        particles[i].v[1] += h * g[1] / particles[i].m;
        particles[i].v[2] += 0; // g[2] / particles[i].m;

        if (!outOfBounds(particles[i].p)) {
            glBegin(GL_POINTS);
            glVertex3fv(particles[i].p);
            glEnd();
        }
    }
}

void box() {
    glColor4f(0.1, 0.3, 0.2, 0.5);

    //upper
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-200, 140.0, 200);
    glVertex3f(-200, 140.0, -200);
    glVertex3f(200, 140.0, 200);
    glVertex3f(200, 140.0, -200);
    glVertex3f(200, 0.0, -200);
    glVertex3f(200, 140.0, 200);
    glVertex3f(200, 0.0, 200);
    glVertex3f(-200, 140.0, 200);
    glVertex3f(-200, 0.0, 200);
    glEnd();
}

void myinit(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MAP1_VERTEX_3);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glMatrixMode(GL_MODELVIEW);

    glPointSize(2.5);

    //glClearColor(1.0, 1.0, 1.0, 0.0);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //set up viewing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, maxWindowX, 0, maxWindowY, -500, 500);

    //initialize particles to zero
    for (int i = 0; i < MAX_PARTICLES; i++) {
        for (int j = 0; j < 3; j++) {
            particles[i].p[j] = 0;
            particles[i].v[j] = 0;
        }
        particles[i].m = 0;
    }
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


    glTranslatef(280, 120, -250);
    //glScalef(2, 2, 2);

    glColor3f(0.3, 0.6, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(-200, 0.0, -200);
    glVertex3f(200, 0.0, -200);
    glVertex3f(200, 0.0, 200);
    glVertex3f(-200, 0.0, 200);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    if (!(createParticle % 7)) {
        newParticle(numOfParticles);
        createParticle = 0;
    }
    moveParticles();
    
    if (showBox)
        box();

    glutSwapBuffers();
    glutPostRedisplay();

    createParticle++;
}

void menu(int id) {

    switch (id) {
 
    case WIND:
        w[0] = fabs(w[0] - 0.2);

        break;

    case SHOW_BOX:
        showBox = !showBox;

        break;

    case UNBOUNDED:
        bound = fabs(bound - 1000.0);
        showBox = false;

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
    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(maxWindowX, maxWindowY);
    glutInitWindowPosition(10, 10);
    windowId = glutCreateWindow("project1");
    glutDisplayFunc(display);

    GLint bezierMenu = glutCreateMenu(menu);
    glutAddMenuEntry("• Wind On/Off", WIND);
    glutAddMenuEntry("• Show/Hide box", SHOW_BOX);
    glutAddMenuEntry("• Unbound On/Off", UNBOUNDED);
    glutAddMenuEntry("Exit", EXIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutKeyboardFunc(keyboard_callback_func);

    myinit();
    glutMainLoop();

    return 0;
}