#include <GL/glut.h>
#include <stdlib.h>
#include <vector>
#include <tuple>
using namespace std;

//int startX = -1;
//int startY = -1;
typedef tuple<int, int> point;
vector<point> vertices; //list of control points for current list of line segments

void setPixel(int x, int y) {
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void drawLowSlope(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yStep = 1;
	if (dy < 0) {
		yStep = -1;
		dy = -dy;
	}

	int D = (2 * dy) - dx;
	int deltaE = 2 * dy;
	int deltaNE = 2 * (dy - dx);

	int x = x1;
	int y = y1;
	setPixel(x1, y1);
	while (x < x2) {
		if (D <= 0) {
			D += deltaE;
			x++;
		}
		else {
			D += deltaNE;
			x++;
			y += yStep;
		}
		setPixel(x, y);
	}
}

void drawHighSlope(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int xStep = 1;
	if (dx < 0) {
		xStep = -1;
		dx = -dx;
	}

	int D = (2 * dx) - dy;
	int deltaE = 2 * dx;
	int deltaNE = 2 * (dx - dy);

	int x = x1;
	int y = y1;
	setPixel(x1, y1);
	while (y < y2) {
		if (D <= 0) {
			D += deltaE;
			y++;
		}
		else {
			D += deltaNE;
			y++;
			x += xStep;
		}
		setPixel(x, y);
	}
}

void drawLine(int x1, int y1, int x2, int y2) {
	if (abs(y2 - y1) < abs(x2 - x1)) { //if slope between -1 and 1
		if (x1 > x2) {
			drawLowSlope(x2, y2, x1, y1);
		}
		else {
			drawLowSlope(x1, y1, x2, y2);
		}
	}
	else {
		if (y1 > y2) {
			drawHighSlope(x2, y2, x1, y1);
		}
		else {
			drawHighSlope(x1, y1, x2, y2);
		}
	}
	//glutSwapBuffers();
}

void processMouse(int button, int state, int x, int y) {
	// check if the left button is down 
	y = 600 - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (vertices.empty()) { //first control point
			glClear(GL_COLOR_BUFFER_BIT);
			vertices.push_back(make_tuple(x, y)); //add first control point to vertex list
			//glutSwapBuffers();
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			vertices.push_back(make_tuple(x, y));	//add new control point
			point startPoint = vertices[0];
			for (int i = 1; i < vertices.size(); i++) {	//redraw all lines, including line to new control point
				drawLine(get<0>(startPoint), get<1>(startPoint), get<0>(vertices[i]), get<1>(vertices[i]));
				startPoint = vertices[i];
			}
			//glutSwapBuffers();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		glClear(GL_COLOR_BUFFER_BIT);
		vertices.push_back(make_tuple(x, y));	//add last control point
		point startPoint = vertices[0];
		for (int i = 1; i < vertices.size(); i++) {	//redraw all lines, including line to last control point
			drawLine(get<0>(startPoint), get<1>(startPoint), get<0>(vertices[i]), get<1>(vertices[i]));
			startPoint = vertices[i];
		}
		vertices.clear(); //reset list of control points
		//glutSwapBuffers();
	}
}

void processMotion(int x, int y) {
	y = 600 - y;
	if (!vertices.empty()) {
		glClear(GL_COLOR_BUFFER_BIT);
		point startPoint = vertices[0];
		for (int i = 1; i < vertices.size(); i++) {	//redraw all lines
			drawLine(get<0>(startPoint), get<1>(startPoint), get<0>(vertices[i]), get<1>(vertices[i]));
			startPoint = vertices[i];
		}
		drawLine(get<0>(startPoint), get<1>(startPoint), x, y);
		glutSwapBuffers();
	}
}

void init()
{
	// clear color buffer
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// set the viewport as the full screen
	glViewport(0, 0, 800, 600);

	// set the projection as orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 800.0, 0.0, 600.0, 1.0, -1.0);

	// set the matrix mode back to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display() {
	//glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// create a window
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");

	// init settings
	init();
	glutDisplayFunc(display);

	//add a mouse motion callback
	glutPassiveMotionFunc(processMotion);

	// register a call back for mouse events
	glutMouseFunc(processMouse);

	//glClearColor(1.0, 1.0, 1.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glFlush();
	// enter main loop
	glutMainLoop();
}