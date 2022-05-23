#include <GL/glut.h>
#include <stdlib.h>

void setPixel(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void drawLowSlope(int x1, int y1, int x2, int y2, int width, int stipple) { //draws line with slope between -1 and 1
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yStep = 1;
	if (dy < 0) { //if slope is negative, decrease y instead of increasing it
		yStep = -1;
		dy = -dy;
	}
	int D = (2 * dy) - dx;
	int deltaD1 = 2 * dy;
	int deltaD2 = 2 * (dy - dx);
	int y = y1;

	int counter = 0;
	for (int x = x1; x <= x2; x++) {
		counter++;
		if (counter > (2 * stipple)) counter = 0;	//stippling
		if (counter <= stipple) {
			setPixel(x, y);
			for (int i = 0; i <= width; i++) {	//line width
				setPixel(x, y+i);
				setPixel(x, y-i);
			}
		}

		if (D < 0) {
			D += deltaD1;
		}
		else {
			D += deltaD2;
			y += yStep;
		}
	}
}

void drawHighSlope(int x1, int y1, int x2, int y2, int width, int stipple) { //draws line with slope less than -1 or greater than 1
	int dx = x2 - x1;
	int dy = y2 - y1;
	int xStep = 1;
	if (dx < 0) {
		xStep = -1;
		dx = -dx;
	}
	int D = (2 * dx) - dy;
	int deltaD1 = 2 * dx;
	int deltaD2 = 2 * (dx - dy);
	int x = x1;

	int counter = 0;
	for (int y = y1; y <= y2; y++) {
		counter++;
		if (counter > (2 * stipple)) counter = 0;
		if (counter <= stipple) {
			setPixel(x, y);
			for (int i = 0; i <= width; i++) {	//line width
				setPixel(x+i, y);
				setPixel(x-i, y);
			}
		}

		if (D < 0) {
			D += deltaD1;
		}
		else {
			D += deltaD2;
			x += xStep;
		}
	}
}

void drawLine(int x1, int y1, int x2, int y2, int width, int stipple) { //width of 0 for single-pixel line, stipple of 0 for solid line
	if (abs(y2-y1) < abs(x2-x1)) { //if slope between -1 and 1
		if (x1 > x2) {
			drawLowSlope(x2, y2, x1, y1, width, stipple);
		}
		else {
			drawLowSlope(x1, y1, x2, y2, width, stipple);
		}
	}
	else {
		if (y1 > y2) {
			drawHighSlope(x2, y2, x1, y1, width, stipple);
		}
		else {
			drawHighSlope(x1, y1, x2, y2, width, stipple);
		}
	}
}

void display() {

	// clear color buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 800.0, 0.0, 600.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3i(0,0,0); //set line color to black

	//my implementation
	//H
	drawLine(50, 450, 50, 350, 0, 0);
	drawLine(50, 400, 100, 400, 0, 0);
	drawLine(100, 350, 100, 450, 0, 0);
	//E
	drawLine(110, 450, 160, 450, 0, 0);
	drawLine(110, 450, 110, 350, 0, 0);
	drawLine(110, 400, 160, 400, 0, 0);
	drawLine(110, 350, 160, 350, 0, 0);
	//L
	drawLine(170, 350, 220, 350, 0, 0);
	drawLine(170, 350, 170, 450, 0, 0);
	//L
	drawLine(230, 350, 280, 350, 0, 0);
	drawLine(230, 350, 230, 450, 0, 0);
	//O
	drawLine(290, 350, 340, 350, 0, 0);
	drawLine(290, 450, 340, 450, 0, 0);
	drawLine(290, 350, 290, 450, 0, 0);
	drawLine(340, 350, 340, 450, 0, 0);
	//other lines
	drawLine(50, 325, 340, 325, 0, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawLine(50, 300, 340, 300, 1, 10);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawLine(50, 275, 340, 275, 3, 0);

	//OpenGL implementation
	glColor3f(0.0f, 0.0f, 0.0f);
	
	//H
	glBegin(GL_LINES);
	glVertex2i(450, 350);
	glVertex2i(450, 450);
	glVertex2i(450, 400);
	glVertex2i(500, 400);
	glVertex2i(500, 350);
	glVertex2i(500, 450);
	glEnd();
	//E
	glBegin(GL_LINES);
	glVertex2i(510, 350);
	glVertex2i(560, 350);
	glVertex2i(510, 400);
	glVertex2i(560, 400);
	glVertex2i(510, 450);
	glVertex2i(560, 450);
	glVertex2i(510, 350);
	glVertex2i(510, 450);
	glEnd();
	//L
	glBegin(GL_LINES);
	glVertex2i(570, 350);
	glVertex2i(620, 350);
	glVertex2i(570, 350);
	glVertex2i(570, 450);
	glEnd();
	//L
	glBegin(GL_LINES);
	glVertex2i(630, 350);
	glVertex2i(680, 350);
	glVertex2i(630, 350);
	glVertex2i(630, 450);
	glEnd();
	//O
	glBegin(GL_LINES);
	glVertex2i(690, 350);
	glVertex2i(740, 350);
	glVertex2i(690, 450);
	glVertex2i(740, 450);
	glVertex2i(690, 350);
	glVertex2i(690, 450);
	glVertex2i(740, 350);
	glVertex2i(740, 450);
	glEnd();
	//other lines
	glLineStipple(5, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glVertex2i(450, 325);
	glVertex2i(740, 325);
	glEnd();

	glLineStipple(10, 0x5555);
	glLineWidth(3.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2i(450, 300);
	glVertex2i(740, 300);
	glEnd();

	glDisable(GL_LINE_STIPPLE);
	glLineWidth(7.0);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2i(450, 275);
	glVertex2i(740, 275);
	glEnd();

	glFlush();
}

int main(int argc, char** argv) {

	// create a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(300, 150);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Bresenham Algorithm");

	// register a display callback
	glutDisplayFunc(display);

	// event processing loop
	glutMainLoop();

	return 0;
}
