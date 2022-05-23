#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glut.h>
using namespace std;

double eye[3] = {0.0f, 0.0f, 40.0f};
double look[3] = {0.0f, 0.0f, 0.0f};
double u[3] = {1.0f, 0.0f, 0.0f};
double v[3] = {0.0f, 1.0f, 0.0f};
double n[3] = {0.0f, 0.0f, -1.0f};
float angle = 0.0f;

double degreeToRad(double degree) {
	return (degree * M_PI) / 180.0;
}

void processKeys(unsigned char key, int x, int y) {
	if (key == 'w') { //pitch up
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			v[i] = v[i] * cos(rad) - n[i] * sin(rad);
			n[i] = v[i] * sin(rad) + n[i] * cos(rad);
			look[i] = eye[i] + n[i];
		}
	}
	else if (key == 's') { //pitch down
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			v[i] = v[i] * cos(rad) + n[i] * sin(rad);
			n[i] = -v[i] * sin(rad) + n[i] * cos(rad);
			look[i] = eye[i] + n[i];
		}
	}
	else if (key == 'a') { //yaw left
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			u[i] = u[i] * cos(rad) + n[i] * sin(rad);
			n[i] = -u[i] * sin(rad) + n[i] * cos(rad);
			look[i] = eye[i] + n[i];
		}
	}
	else if (key == 'd') { //yaw right
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			u[i] = u[i] * cos(rad) - n[i] * sin(rad);
			n[i] = u[i] * sin(rad) + n[i] * cos(rad);
			look[i] = eye[i] + n[i];
		}
	}
	glutPostRedisplay();
}

void processArrowKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) { //slide forwards
		for (int i = 0; i < 3; i++) {
			eye[i] = eye[i] + n[i];
		}
	}
	else if (key == GLUT_KEY_DOWN) { //slide back
		for (int i = 0; i < 3; i++) {
			eye[i] = eye[i] - n[i];
		}
	}
	else if (key == GLUT_KEY_LEFT) { //roll ccw
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			u[i] = u[i] * cos(rad) - v[i] * sin(rad);
			v[i] = u[i] * sin(rad) + v[i] * cos(rad);
		}
	}
	else if (key == GLUT_KEY_RIGHT) { //roll cw
		double rad = degreeToRad(10.0);
		for (int i = 0; i < 3; i++) {
			u[i] = u[i] * cos(rad) + v[i] * sin(rad);
			v[i] = -u[i] * sin(rad) + v[i] * cos(rad);
		}
	}
	glutPostRedisplay();
}

void processMenu(int option) {
	if (option == 1) { //lever rotation
		angle += 10.0f;
		glutPostRedisplay();
	}
}

void drawGroundPlane(int y) {
	glColor3ub(0, 0, 0);
	for (int x = -100; x < 100; x=x+5) {
		glBegin(GL_LINES);
		glVertex3i(x, y, -100);
		glVertex3i(x, y, 100);
		glEnd();
	}
	for (int z = -100; z < 100; z=z+5) {
		glBegin(GL_LINES);
		glVertex3i(-100, y, z);
		glVertex3i(100, y, z);
		glEnd();
	}
}

void drawLever() {
	glColor3ub(169, 169, 169);
	//glLoadIdentity();
	gluSphere(gluNewQuadric(), 2.0, 25, 10); //b2

	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); //align with y-axis
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 20.0, 10, 10); //s1
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f); //rotate around y-axis by step angle
	glTranslatef(0.0f, 0.0f, -10.0f); //translate center to origin
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 20.0, 10, 10); //s2-s3
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f); //rotate around y-axis by step angle
	glTranslatef(0.0f, 0.0f, -10.0f); //translate from origin
	gluSphere(gluNewQuadric(), 2.0, 25, 10); //b3
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f); //rotate around y-axis
	glTranslatef(0.0f, 0.0f, -10.0f); //translate from origin
	glRotatef(-angle, 0.0f, 0.0f, 1.0f); //rotate around b3
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); //fix orientation
	gluCylinder(gluNewQuadric(), 0.0, 1.0, 10.0, 10, 10); //s5
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -10.0f);
	glRotatef(-angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -10.0f, 0.0f);
	gluSphere(gluNewQuadric(), 4.0, 25, 10); //b5
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f); //rotate around y-axis by step angle
	glTranslatef(0.0f, 0.0f, 10.0f); //translate from origin
	gluSphere(gluNewQuadric(), 2.0, 25, 10); //b1
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 10.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(gluNewQuadric(), 0.0, 1.0, 10.0, 10, 10); //s4
	glPopMatrix();

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 10.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -10.0f, 0.0f);
	gluSphere(gluNewQuadric(), 4.0, 25, 10); //b4
	glPopMatrix();
}

void display() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 400.0 / 300.0, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	
	//viewport 1
	glViewport(400, 0, 400, 300);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		look[0], look[1], look[2],
		v[0], v[1], v[2]);
	//glColor3ub(0, 0, 0);
	drawGroundPlane(-10);
	GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_LINE);
	gluSphere(quad, 10.0, 25, 10);


	//viewport 2
	glViewport(0, 0, 400, 300);
	glLoadIdentity();
	gluLookAt(0.0f, 40.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f);
	drawGroundPlane(-20);
	drawLever();
	

	//viewport 3
	glViewport(0, 300, 400, 300);
	glLoadIdentity();
	gluLookAt(40.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	drawGroundPlane(-20);
	drawLever();

	//viewport 4
	glViewport(400, 300, 400, 300);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 40.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	drawGroundPlane(-20);
	drawLever();
	
	// swap double buffer
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");

	glutDisplayFunc(display);
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processArrowKeys);
	int menu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Lever Rotation", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}