#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

struct vertex {
	double x;
	double y;
	double z;
};
struct face {
	unsigned int v1, v2, v3;
};
vector<vertex> vertices;
vector<face> faces;

double eye[3] = { 0.0f, 0.0f, 10.0f };
double look[3] = { 0.0f, 0.0f, 0.0f };
double u[3] = { 1.0f, 0.0f, 0.0f };
double v[3] = { 0.0f, 1.0f, 0.0f };
double n[3] = { 0.0f, 0.0f, -1.0f };

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

void loadObj() {
	string line;
	ifstream file("teapot.obj");
	while (getline(file, line)) {
		stringstream ss(line);
		string s;
		ss >> s;
		if (s == "v") {
			vertex v;
			ss >> v.x >> v.y >> v.z;
			vertices.push_back(v);
			//cout << v.x << " " << v.y << " " << v.z << endl;
		}
		else if (s == "f") {
			face f;
			ss >> f.v1 >> f.v2 >> f.v3;
			faces.push_back(f);
			//cout << f.v1 << " " << f.v2 << " " << f.v3 << endl;
		}
	}
	file.close();
}

void drawGroundPlane(int y) {
	glColor3ub(0, 0, 0);
	for (int x = -100; x < 100; x = x + 5) {
		glBegin(GL_LINES);
		glVertex3i(x, y, -100);
		glVertex3i(x, y, 100);
		glEnd();
	}
	for (int z = -100; z < 100; z = z + 5) {
		glBegin(GL_LINES);
		glVertex3i(-100, y, z);
		glVertex3i(100, y, z);
		glEnd();
	}
}

void drawObj() {
	glColor3ub(169, 169, 169);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < faces.size(); i++) {
		vertex v1 = vertices[faces[i].v1-1];
		vertex v2 = vertices[faces[i].v2-1];
		vertex v3 = vertices[faces[i].v3-1];

		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
}

void display() {

	// clear buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera position and direction
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		look[0], look[1], look[2],
		v[0], v[1], v[2]);
	drawGroundPlane(-1);
	drawObj();

	// swap double buffer
	glutSwapBuffers();
}

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 800, 600);
	gluPerspective(45, 800.0 / 600.0, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {

	// create a window
	glutInit(&argc, argv);

	// use double buffer for animation
	// check OpenGL redbook 7e page 22 "Animation"
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create a window
	glutInitWindowPosition(300, 300);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");
	init();
	loadObj();

	// register a display callback
	glutDisplayFunc(display);
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processArrowKeys);

	// event processing loop
	glutMainLoop();

	return 0;
}