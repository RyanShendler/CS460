#include <GL/glut.h>
#include <cmath>
#include <vector>
using namespace std;

class Point {
public:
	float x, y, z;
	Point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
};
class Triangle {
public:
	int v1, v2, v3; //indices of the vertexes
	Point faceNormal; //surface normal
	Triangle() {
		v1 = 0;
		v2 = 0;
		v3 = 0;
	}
	Triangle(int a, int b, int c, Point p) {
		v1 = a;
		v2 = b;
		v3 = c;
		faceNormal = p;
	}
};
Point control[16]; //array of control points
float camMove = 0.0; //used to move camera
float lightMove = 0.0; //used to move light source
int mode = 0; //0 for wireframe, 1 for flat shading, 2 for smooth shading
float specExp = 50.0; //specular exponent to change material shininess
float diffuseStep = 0.0; //step value for changing diffuse reflection of material

//draws the XYZ axes
void drawAxes() {
	glBegin(GL_LINES);
	float matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);;
	//x-axis
	float matAmbDefR[] = { 0.7, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDefR);
	glVertex3i(0, 0, 0);
	glVertex3i(30, 0, 0);
	//y-axis
	float matAmbDefG[] = { 0.0, 0.7, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDefG);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 30, 0);
	//z-axis
	float matAmbDefB[] = { 0.0, 0.0, 0.7, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDefB);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 0, 30);
	glEnd();
}

//draws the control points
void drawControl() {
	glBegin(GL_POINTS);
	glColor3ub(255, 0, 255);
	for (int i = 0; i < 16; i++) {
		glVertex3f(control[i].x, control[i].y, control[i].z);
	}
	glEnd();
}

float blend(float u, int degree) {
	float ret = 0.0;
	switch (degree) {
	case 0:
		ret = pow((1.0-u), 3);
		break;
	case 1:
		ret = 3 * u * pow((1.0-u), 2);
		break;
	case 2:
		ret = 3 * pow(u, 2) * (1.0-u);
		break;
	case 3:
		ret = pow(u, 3);
		break;
	}
	return ret;
}

Point calcPoint(float u, float v) {
	Point p;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int offset = j + (4*i);
			p.x += control[offset].x * blend(u, i) * blend(v, j);
			p.y += control[offset].y * blend(u, i) * blend(v, j);
			p.z += control[offset].z * blend(u, i) * blend(v, j);
		}
	}
	return p;
}

Point calcNormal(Point p1, Point p2, Point p3) { //calculates the face normal of a triangular face
	Point N;
	//calculate cross product
	N.x = (p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z);
	N.y = (p2.z - p1.z) * (p3.x - p1.x) - (p3.z - p1.z) * (p2.x - p1.x);
	N.z = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
	//normalize
	float mag = sqrt((N.x * N.x) + (N.y * N.y) + (N.z * N.z));
	N.x = N.x / mag;
	N.y = N.y / mag;
	N.z = N.z / mag;
	return N;
}

Point vertNormal(int index, vector<Triangle> indices) { //calculate vertex normal
	Point N;
	for (int i = 0; i < indices.size(); i++) {
		if ((index == indices[i].v1) || (index == indices[i].v2) || (index == indices[i].v3)) { //for any triangles with vertex index
			N.x += indices[i].faceNormal.x;
			N.y += indices[i].faceNormal.y;
			N.z += indices[i].faceNormal.z;
		}
	}
	return N;
}

//return 1 if face should be culled, return 0 otherwise
int cull(Point v1, Point v2, Point v3) {
	Point V; //view vector
	V.x = -45.0;
	V.y = -45.0;
	V.z = -(45.0 + camMove);
	float tmp1 = V.x * (v2.y - v1.y) * (v3.z - v1.z) - V.x * (v2.z - v1.z) * (v3.y - v1.y);
	float tmp2 = V.y * (v2.z - v1.z) * (v3.x - v1.x) - V.y * (v2.x - v1.x) * (v3.z - v1.z);
	float tmp3 = V.z * (v2.x - v1.x) * (v3.y - v1.y) - V.z * (v2.y - v1.y) * (v3.x - v1.x);
	if ((tmp1 + tmp2 + tmp3) > 0.0) return 1;
	else return 0;
}

void drawBezier() {
	vector<Point> surface;
	for (float u = 0.0; u <= 1.05; u += 0.05) {
		for (float v = 0.0; v <= 1.05; v += 0.05) {
			surface.push_back(calcPoint(u, v));
		}
	}
	float matAmbient[] = {0.0, 0.7, 0.0, 1.0};
	float matDiffuse[] = {0.0, 0.7+diffuseStep, 0.0, 1.0};
	float matSpecular[] = {1.0, 1.0, 1.0, 1.0};
	glColor3ub(128, 128, 128);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, specExp);
	if (mode == 0) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //surface shading
	vector<Triangle> indices;
	for (int i = 0; i < 419; i++) { //create list of traingle vertex indices
		if ((i % 21) != 20) { //so we dont use last vertex in each row
			indices.push_back(Triangle(i, i+21, i+22, calcNormal(surface[i], surface[i + 21], surface[i + 22])));
			indices.push_back(Triangle(i+22, i+1, i, calcNormal(surface[i + 22], surface[i + 1], surface[i])));
		}
	}
	if (mode == 1) glShadeModel(GL_FLAT);
	else if (mode == 2) glShadeModel(GL_SMOOTH);
	for (int i = 0; i < indices.size(); i++) { //render triangles
		Point v1 = surface[indices[i].v1];
		Point v2 = surface[indices[i].v2];
		Point v3 = surface[indices[i].v3];
		if (!cull(v1, v2, v3)) { //cull back faces
			glBegin(GL_TRIANGLES);
			//if (mode == 1) glNormal3f(indices[i].faceNormal.x, indices[i].faceNormal.y, indices[i].faceNormal.z);
			Point n1 = vertNormal(indices[i].v1, indices);
			glNormal3f(n1.x, n1.y, n1.z);
			glVertex3f(v1.x, v1.y, v1.z);
			Point n2 = vertNormal(indices[i].v2, indices);
			glNormal3f(n2.x, n2.y, n2.z);
			glVertex3f(v2.x, v2.y, v2.z);
			Point n3 = vertNormal(indices[i].v3, indices);
			glNormal3f(n3.x, n3.y, n3.z);
			glVertex3f(v3.x, v3.y, v3.z);
			glEnd();
		}
	}
}

void display() {
	//clear buffer contents
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set up perspective matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0 / 600.0, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, 800, 600);
	
	//move camera
	gluLookAt(45.0, 45.0, 45.0 + camMove, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//move light
	float lightPos[] = {50.0, 50.0 + lightMove, 50.0, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//draw content
	drawAxes();
	//drawControl();
	drawBezier();
	glutSwapBuffers();
}

//handles camera movement
void processMain(int option) {
	switch (option) {
	case 1:
		camMove += 5.0;
		break;
	case 2:
		camMove -= 5.0;
		break;
	case 3:
		lightMove += 5.0;
		break;
	case 4:
		lightMove -= 5.0;
		break;
	}
	glutPostRedisplay();
}

void processMode(int option) {
	switch (option) {
	case 1: //wireframe
		mode = 0;
		break;
	case 2: //flat shading
		mode = 1;
		//glShadeModel(GL_FLAT);
		break;
	case 3: //smooth shading
		mode = 2;
		//glShadeModel(GL_SMOOTH);
		break;
	}
	glutPostRedisplay();
}

void processLighting(int option) {
	switch (option) {
	case 1: //shininess+
		specExp += 5.0;
		break;
	case 2: //shininess-
		specExp -= 5.0;
		break;
	case 3: //diffuse+
		if(diffuseStep < 0.3) diffuseStep += 0.1;
		break;
	case 4: //diffuse-
		diffuseStep -= 0.1;
		break;
	}
	glutPostRedisplay();
}

//moves control point 0
void processPoint0(int option) {
	switch (option) {
	case 1: //X+
		control[0].x += 2.0;
		break;
	case 2: //X-
		control[0].x -= 2.0;
		break;
	case 3: //Y+
		control[0].y += 2.0;
		break;
	case 4: //Y-
		control[0].y -= 2.0;
		break;
	case 5: //Z+
		control[0].z += 2.0;
		break;
	case 6: //Z-
		control[0].z -= 2.0;
		break;
	}
	glutPostRedisplay();
}


//moves control point 5
void processPoint5(int option) {
	switch (option) {
	case 1: //X+
		control[5].x += 2.0;
		break;
	case 2: //X-
		control[5].x -= 2.0;
		break;
	case 3: //Y+
		control[5].y += 2.0;
		break;
	case 4: //Y-
		control[5].y -= 2.0;
		break;
	case 5: //Z+
		control[5].z += 2.0;
		break;
	case 6: //Z-
		control[5].z -= 2.0;
		break;
	}
	glutPostRedisplay();
}

//moves control point 11
void processPoint11(int option) {
	switch (option) {
	case 1: //X+
		control[11].x += 2.0;
		break;
	case 2: //X-
		control[11].x -= 2.0;
		break;
	case 3: //Y+
		control[11].y += 2.0;
		break;
	case 4: //Y-
		control[11].y -= 2.0;
		break;
	case 5: //Z+
		control[11].z += 2.0;
		break;
	case 6: //Z-
		control[11].z -= 2.0;
		break;
	}
	glutPostRedisplay();
}

//moves control point 14
void processPoint14(int option) {
	switch (option) {
	case 1: //X+
		control[14].x += 2.0;
		break;
	case 2: //X-
		control[14].x -= 2.0;
		break;
	case 3: //Y+
		control[14].y += 2.0;
		break;
	case 4: //Y-
		control[14].y -= 2.0;
		break;
	case 5: //Z+
		control[14].z += 2.0;
		break;
	case 6: //Z-
		control[14].z -= 2.0;
		break;
	}
	glutPostRedisplay();
}

void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//initializes control points
	float x = 5.0, z = 5.0;
	for (int i = 0; i < 16; i++) {
		if (x == 25.0) {
			x = 5.0;
			z += 5.0;
		}
		control[i].x = x;
		control[i].y = 0.0;
		control[i].z = z;
		x += 5.0;
	}
	//enables lighting and depth buffer
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); //normalize normal vectors
	glEnable(GL_LIGHTING);
	//set light parameters
	float lightAmbient[] = { 0.1, 0.1, 0.1, 1.0 };
	float lightDiffuse[] = { 0.7, 0.7, 0.7, 1.0 };
	float lightSpecular[] = { 0.4, 0.4, 0.4, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

//creates the menu
void createMenu() {
	int point0 = glutCreateMenu(processPoint0);
	glutAddMenuEntry("X+", 1);
	glutAddMenuEntry("X-", 2);
	glutAddMenuEntry("Y+", 3);
	glutAddMenuEntry("Y-", 4);
	glutAddMenuEntry("Z+", 5);
	glutAddMenuEntry("Z-", 6);
	int point5 = glutCreateMenu(processPoint5);
	glutAddMenuEntry("X+", 1);
	glutAddMenuEntry("X-", 2);
	glutAddMenuEntry("Y+", 3);
	glutAddMenuEntry("Y-", 4);
	glutAddMenuEntry("Z+", 5);
	glutAddMenuEntry("Z-", 6);
	int point11 = glutCreateMenu(processPoint11);
	glutAddMenuEntry("X+", 1);
	glutAddMenuEntry("X-", 2);
	glutAddMenuEntry("Y+", 3);
	glutAddMenuEntry("Y-", 4);
	glutAddMenuEntry("Z+", 5);
	glutAddMenuEntry("Z-", 6);
	int point14 = glutCreateMenu(processPoint14);
	glutAddMenuEntry("X+", 1);
	glutAddMenuEntry("X-", 2);
	glutAddMenuEntry("Y+", 3);
	glutAddMenuEntry("Y-", 4);
	glutAddMenuEntry("Z+", 5);
	glutAddMenuEntry("Z-", 6);
	int displayMode = glutCreateMenu(processMode);
	glutAddMenuEntry("Wireframe", 1);
	glutAddMenuEntry("Flat Shading", 2);
	glutAddMenuEntry("Smooth Shading", 3);
	int lighting = glutCreateMenu(processLighting);
	glutAddMenuEntry("Shininess+", 1);
	glutAddMenuEntry("Shininess-", 2);
	glutAddMenuEntry("Diffuse+", 3);
	glutAddMenuEntry("Diffuse-", 4);
	int mainMenu = glutCreateMenu(processMain);
	glutAddMenuEntry("Camera+", 1);
	glutAddMenuEntry("Camera-", 2);
	glutAddMenuEntry("Light+", 3);
	glutAddMenuEntry("Light-", 4);
	glutAddSubMenu("Display Mode", displayMode);
	glutAddSubMenu("Lighting", lighting);
	glutAddSubMenu("Control Point 0", point0);
	glutAddSubMenu("Control Point 5", point5);
	glutAddSubMenu("Control Point 11", point11);
	glutAddSubMenu("Control Point 14", point14);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create a window
	glutInitWindowPosition(150, 150);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");
	init();

	// register a display callback
	glutDisplayFunc(display);
	//create menu
	createMenu();

	// event processing loop
	glutMainLoop();

	return 0;
}