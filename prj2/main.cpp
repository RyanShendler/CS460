#include <GL/glut.h>
#include <vector>
#include <tuple>
#include <stdlib.h>
using namespace std;

typedef tuple<int, int> point;
vector<point> window{make_tuple(300, 400), make_tuple(300, 250), make_tuple(150, 250), make_tuple(150, 400)};
vector<point> polygon; //polygon vertices
vector<point> clipped; //clipped polygon vertices
vector<point> viewport{make_tuple(580, 350), make_tuple(580, 250), make_tuple(500, 250), make_tuple(500, 350)};
int mode = 0; //0, is default, 1 is clipping, 2 is filling, 3 is viewport
int drawPoly = 0; //0 if still taking control points, 1 if polygon is to be drawn
point seedPixel = make_tuple(-1, -1);

//get x-intercept of line with points P1 & P2 and line with points P3 & P4 
int x_intersect(int x1, int y1, int x2, int y2, int y) {
	float m = (static_cast<float>(y2) - y1) / (x2 - x1);
	return ((y - y1) / m) + x1;
}

//get y-intercept of line with points P1 & P2 and line with points P3 & P4 
int y_intersect(int x1, int y1, int x2, int y2, int x) {
	float m = (static_cast<float>(y2)-y1) / (x2-x1);
	return m * (x - x1) + y1;
}

void clipPolygon() { //seems good
	vector<point> outList;
	vector<point> inList = polygon;

	//clip left
	for (int i = 0; i < inList.size(); i++) {
		point curPoint = inList[i];
		point nextPoint = inList[(i+1) % inList.size()];
		int y_int = y_intersect(get<0>(curPoint), get<1>(curPoint), get<0>(nextPoint), get<1>(nextPoint), get<0>(window[2]));
		point intersect = make_tuple(get<0>(window[3]), y_int);
		if (get<0>(nextPoint) >= get<0>(window[3])) { //if next point inside
			if (get<0>(curPoint) < get<0>(window[3])) { //current point not inside
				outList.push_back(intersect);
			}
			outList.push_back(nextPoint);
		}
		else if (get<0>(curPoint) >= get<0>(window[3])) { //if current point inside
			outList.push_back(intersect);
		}
	}
	//clip right
	inList = outList;
	outList.clear();
	for (int i = 0; i < inList.size(); i++) {
		point curPoint = inList[i];
		point nextPoint = inList[(i + 1) % inList.size()];
		int y_int = y_intersect(get<0>(curPoint), get<1>(curPoint), get<0>(nextPoint), get<1>(nextPoint), get<0>(window[1]));
		point intersect = make_tuple(get<0>(window[0]), y_int);
		if (get<0>(nextPoint) <= get<0>(window[0])) { //if next point inside
			if (get<0>(curPoint) > get<0>(window[0])) { //current point not inside
				outList.push_back(intersect);
			}
			outList.push_back(nextPoint);
		}
		else if (get<0>(curPoint) <= get<0>(window[0])) { //if current point inside
			outList.push_back(intersect);
		}
	}
	//clip bottom
	inList = outList;
	outList.clear();
	for (int i = 0; i < inList.size(); i++) {
		point curPoint = inList[i];
		point nextPoint = inList[(i + 1) % inList.size()];
		int x_int = x_intersect(get<0>(curPoint), get<1>(curPoint), get<0>(nextPoint), get<1>(nextPoint), get<1>(window[2]));
		point intersect = make_tuple(x_int, get<1>(window[1]));
		if (get<1>(nextPoint) >= get<1>(window[1])) { //if next point inside
			if (get<1>(curPoint) < get<1>(window[1])) { //current point not inside
				outList.push_back(intersect);
			}
			outList.push_back(nextPoint);
		}
		else if (get<1>(curPoint) >= get<1>(window[1])) { //if current point inside
			outList.push_back(intersect);
		}
	}
	//clip top
	inList = outList;
	outList.clear();
	for (int i = 0; i < inList.size(); i++) {
		point curPoint = inList[i];
		point nextPoint = inList[(i + 1) % inList.size()];
		int x_int = x_intersect(get<0>(curPoint), get<1>(curPoint), get<0>(nextPoint), get<1>(nextPoint), get<1>(window[3]));
		point intersect = make_tuple(x_int, get<1>(window[0]));
		if (get<1>(nextPoint) <= get<1>(window[0])) { //if next point inside
			if (get<1>(curPoint) > get<1>(window[0])) { //current point not inside
				outList.push_back(intersect);
			}
			outList.push_back(nextPoint);
		}
		else if (get<1>(curPoint) <= get<1>(window[0])) { //if current point inside
			outList.push_back(intersect);
		}
	}

	clipped = outList;
}

void setPixel(int x, int y) {
	glColor3ub(255, 0, 0);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void boundaryFill(int x, int y) {
	struct { GLubyte red, green, blue; } pixel;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
	if (((int)pixel.red != 0 && (int)pixel.green != 0 && (int)pixel.blue !=0) && ((int)pixel.green != 0 && (int)pixel.blue != 0)) {
		setPixel(x, y);
		boundaryFill(x+1, y);
		boundaryFill(x, y+1);
		boundaryFill(x-1, y);
		boundaryFill(x, y-1);
	}
	
}

void drawWindow() {
	glColor3ub(0, 0, 0); //takes RGB values in range 0-255
	glLineStipple(5, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINE_LOOP);
	
	for (int i = 0; i < window.size(); i++) {
		glVertex2i(get<0>(window[i]), get<1>(window[i]));
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

void drawPolygon() {
	if (polygon.size() >= 2) {
		glColor3ub(0, 0, 0);
		if(drawPoly == 0){ //if control points still being placed
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < polygon.size(); i++) {
				glVertex2i(get<0>(polygon[i]), get<1>(polygon[i]));
			}
			glEnd();
		}
		else { //if final control point has been placed
			if (mode > 0) { //draw clipped polygon
				clipPolygon(); //vector of clipped vertices
				glBegin(GL_LINE_LOOP);

				for (int i = 0; i < clipped.size(); i++) {
					glVertex2i(get<0>(clipped[i]), get<1>(clipped[i]));
				}
				glEnd();
			}
			else { //draw original polygon
				glBegin(GL_LINE_LOOP);

				for (int i = 0; i < polygon.size(); i++) {
					glVertex2i(get<0>(polygon[i]), get<1>(polygon[i]));
				}
				glEnd();
			}
		}
	}
}

void drawViewport() {
	glColor3ub(0, 0, 0); //takes RGB values in range 0-255
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < viewport.size(); i++) {
		glVertex2i(get<0>(viewport[i]), get<1>(viewport[i]));
	}
	glEnd();
}

void mapToViewport() {
	if (!clipped.empty()) { //only draw in viewport if polygon has been clipped
		int xmin = get<0>(window[2]), xmax = get<0>(window[0]), ymin = get<1>(window[2]), ymax = get<1>(window[0]);
		int umin = get<0>(viewport[2]), umax = get<0>(viewport[0]), vmin = get<1>(viewport[2]), vmax = get<1>(viewport[0]);
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < clipped.size(); i++) {
			int x = get<0>(clipped[i]), y = get<1>(clipped[i]);
			float x_temp = (static_cast<float>(umax) - umin) / (xmax - xmin);
			float y_temp = (static_cast<float>(vmax) - vmin) / (ymax - ymin);
			x = (x - xmin) * x_temp + umin;
			y = (y - ymin) * y_temp + vmin;
			glVertex2i(x, y);
		}
		glEnd();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawWindow();
	drawPolygon();
	//if (drawPoly == 1) drawPolygon();
	if (mode == 2 && get<0>(seedPixel) != -1 && get<1>(seedPixel) != -1){
		boundaryFill(get<0>(seedPixel), get<1>(seedPixel));
		get<0>(seedPixel) = -1;
		get<1>(seedPixel) = -1;
	}
	if (mode == 3) {
		drawViewport();
		mapToViewport();
	}
	glutSwapBuffers();
}

void processMenu(int option)
{
	if (option == 1) { //clipping
		mode = 1;
	}
	else if (option == 2) { //filling
		mode = 2;
	}
	else { //viewport
		mode = 3;
	}
	glutPostRedisplay();
}

void processMouse(int button, int state, int x, int y) {
	y = 600 - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && drawPoly == 0) { //add new control point
		polygon.push_back(make_tuple(x, y));
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && drawPoly == 0) { //add last control point
		polygon.push_back(make_tuple(x, y));
		drawPoly = 1;
	}
}

void processMotion(int x, int y) {
	y = 600 - y;
	if (abs(x - get<0>(window[0])) <= 8 && abs(y - get<1>(window[0])) <= 8) {
		window[0] = make_tuple(x, y);
		get<0>(window[1]) = x;
		get<1>(window[3]) = y;
	}
	else if (abs(x - get<0>(viewport[0])) <= 8 && abs(y - get<1>(viewport[0])) <= 8 && mode == 3) {
		viewport[0] = make_tuple(x, y);
		get<0>(viewport[1]) = x;
		get<1>(viewport[3]) = y;
	}
}

void processArrowKeys(int key, int x, int y) { //pan window on arrow key input
	if (key == GLUT_KEY_UP) {
		for (int i = 0; i < window.size(); i++) {
			get<1>(window[i]) += 10;
		}
		glutPostRedisplay();
	}
	else if(key == GLUT_KEY_LEFT) {
		for (int i = 0; i < window.size(); i++) {
			get<0>(window[i]) -= 10;
		}
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_RIGHT) {
		for (int i = 0; i < window.size(); i++) {
			get<0>(window[i]) += 10;
		}
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_DOWN) {
		for (int i = 0; i < window.size(); i++) {
			get<1>(window[i]) -= 10;
		}
		glutPostRedisplay();
	}
}

void processKeys(unsigned char key, int x, int y) {
	y = 600 - y;
	if (key == 'f' && mode == 2) {
		//boundaryFill(x, y);
		get<0>(seedPixel) = x;
		get<1>(seedPixel) = y;
		glutPostRedisplay();
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

int main(int argc, char** argv) {

	// create a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Computer Graphics");

	//initialize settings
	init();

	glutDisplayFunc(display);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMotion);
	glutSpecialFunc(processArrowKeys);
	glutKeyboardFunc(processKeys);

	int menu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Polygon Clipping", 1);
	glutAddMenuEntry("Region Filling", 2);
	glutAddMenuEntry("Window-to-Viewport Mapping", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON); //attach to 'm' key

	// event processing loop
	glutMainLoop();

	return 0;
}
