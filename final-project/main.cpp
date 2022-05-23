#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
constexpr double pi = 3.14159265358979323846;
using namespace std;

//global variables
double radius = 10.0;
double interval = 16.0;
int offset = 32;
int mode = 1; //0 for points, 1 for wireframe

class Point {
public:
	double x, y, z;
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
};

void drawAxes() {
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3f(-25.0f, 0.0f, 0.0f);
	glVertex3f(25.0f, 0.0f, 0.0f);
	glColor3ub(0, 255, 0);
	glVertex3f(0.0f, -25.0f, 0.0f);
	glVertex3f(0.0f, 25.0f, 0.0f);
	glColor3ub(0, 0, 255);
	glVertex3f(0.0f, 0.0f, -25.0f);
	glVertex3f(0.0f, 0.0f, 25.0f);
	glEnd();
}

void drawSphere() {
	//set draw parameters
	glColor3ub(0, 0, 0);
	if (mode == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (mode == 2) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	//generating vertices
	vector<Point> vertices;
	vector<double> theta;
	vector<double> phi;
	int size = static_cast<int>(interval);
	for (int i = 1; i < size; i++) {
		double t = ((double)i * pi) / interval;
		theta.push_back(t);
	}
	for (int i = 0; i < offset; i++) {
		double p = ((double)i * pi) / interval;
		phi.push_back(p);
	}

	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			Point p;
			p.x = radius * sin(phi[j]) * sin(theta[i]);
			p.y = radius * cos(theta[i]);
			p.z = radius * cos(phi[j]) * sin(theta[i]);
			vertices.push_back(p);
		}
	}
	//int numFaces = 0;
	//cout << "Interval: " << interval << endl;
	//cout << "Number Vertices: " << vertices.size() + 2 << endl;

	//draw north polar region
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0, radius, 0.0); //draw north pole
	for (int i = 0; i < offset; i++) {
		glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
		//numFaces++;
	}
	glVertex3d(vertices[0].x, vertices[0].y, vertices[0].z);
	//numFaces++;
	glEnd();

	//draw southern polar region
	if (mode != 2) {
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0.0, -radius, 0.0); //draw north pole
		for (int i = vertices.size() - offset; i < vertices.size(); i++) {
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
			//numFaces++;
		}
		glVertex3d(vertices[vertices.size() - offset].x, vertices[vertices.size() - offset].y, vertices[vertices.size() - offset].z);
		//numFaces++;
		glEnd();
	}

	//generate non-polar region
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < (vertices.size()-offset-1); i++) {
		if (((i+1) % offset) != 0) { //if not the last point at this latitude
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
			glVertex3d(vertices[i + offset].x, vertices[i + offset].y, vertices[i + offset].z);
			glVertex3d(vertices[i + offset+1].x, vertices[i + offset + 1].y, vertices[i + offset + 1].z);

			glVertex3d(vertices[i + offset + 1].x, vertices[i + offset + 1].y, vertices[i + offset + 1].z);
			glVertex3d(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
		}
		else { //makes mesh wrap-around at last point at this latitude
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
			glVertex3d(vertices[i + offset].x, vertices[i + offset].y, vertices[i + offset].z);
			glVertex3d(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);

			glVertex3d(vertices[i + 1].x, vertices[i + 1].y, vertices[i  +1].z);
			glVertex3d(vertices[i - (offset - 1)].x, vertices[i - (offset - 1)].y, vertices[i - (offset - 1)].z);
			glVertex3d(vertices[i].x, vertices[i].y, vertices[i].z);
		}
		//numFaces += 2;
	}
	glEnd();
	//cout << "Number of Faces: " << numFaces << endl;
	if (mode == 2) glDisable(GL_CULL_FACE);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawAxes();
	drawSphere();
	glutSwapBuffers();
}

void processMode(int option) {
	switch (option) {
	case 1:
		mode = 0;
		break;
	case 2:
		mode = 1;
		break;
	case 3:
		mode = 2;
		break;
	}
	glutPostRedisplay();
}

void processMenu(int option) {
	switch (option) {
	case 1: //radius +
		radius += 5.0;
		break;
	case 2: //radius -
		if(radius > 5.0) radius -= 5.0;
		break;
	case 3:
		interval = 2.0 * interval;
		offset = 2 * offset;
		break;
	case 4:
		if (interval > 4.0) {
			interval = interval / 2.0;
			offset = offset / 2;
		}
		break;
	}
	glutPostRedisplay();
}

void init() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//set up perspective matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0 / 600.0, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, 800, 600);
	gluLookAt(40.0, 40.0, 40.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");

	init();

	glutDisplayFunc(display);
	int modeMenu = glutCreateMenu(processMode);
	glutAddMenuEntry("Points", 1);
	glutAddMenuEntry("Wireframe", 2);
	glutAddMenuEntry("Culling", 3);
	int menu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Radius+", 1);
	glutAddMenuEntry("Radius-", 2);
	glutAddMenuEntry("Interval+", 3);
	glutAddMenuEntry("Interval-", 4);
	glutAddSubMenu("Mode", modeMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}