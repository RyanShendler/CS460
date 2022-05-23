#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

int center[2] = { 191, 127 };
//flower.bmp has width of 384 pixels and height of 256 pixels
unsigned char texture[384 * 256 * 3];

void loadImage() {
	FILE* file;
	file = fopen("flower.bmp", "rb");
	if (!file) {
		printf("file couldn't be opened\n");
		exit(0);
	}
	unsigned char header[54];
	fread(header, 1, 54, file);
	fread(texture, 1, 384 * 256 * 3, file);
	for (int i = 0; i < 384 * 256; i++) {
		int index = 3 * i;
		unsigned char B, R;
		B = texture[index];
		R = texture[index + 2];
		texture[index] = R;
		texture[index + 2] = B;
	}
	fclose(file);
}

void processMotion(int x, int y) {
	y = 260 - y;
	if (abs(x-center[0]) <= 10 && abs(y-center[1]) <= 10) {
		center[0] = x;
		center[1] = y;
	}
}

//(i, j) are the coordinates in the target rectangle
void renderPoint(int i, int j) {
	int point[2];
	float u, v;
	float tempX, tempY;
	if ((i <= center[0]) && (j <= center[1])) { //case 1
		u = (float)i / (float)(center[0]+1);
		v = (float)j / (float)(center[1]+1);
		tempX = 191.0 * u;
		tempY = 127.0 * v;
		point[0] = (int)tempX;
		point[1] = (int)tempY;
	}
	else if ((i > center[0]) && (j <= center[1])) { //case 2
		u = (float)(i-191) / (float)(383 - center[0]); //maybe should be 384
		v = (float)j / (float)(center[1]+1);
		tempX = 191.0 * u + 191.0; //maybe should be 192
		tempY = 127.0 * v;
		point[0] = (int)tempX;
		point[1] = (int)tempY;
	}
	else if ((i <= center[0]) && (j > center[1])) { //case 3
		u = (float)i / (float)(center[0]+1);
		v = (float)(j-127) / (float)(255 - center[1]);
		tempX = 192.0 * u;
		tempY = 127.0 * v + 127.0;
		point[0] = (int)tempX;
		point[1] = (int)tempY;
	}
	else if ((i > center[0]) && (j > center[1])) { //case 4
		u = (float)(i-191) / (float)(383 - center[0]);
		v = (float)(j-127) / (float)(255 - center[1]);
		tempX = 191.0 * u + 191.0;
		tempY = 127.0 * v + 127.0;
		point[0] = (int)tempX;
		point[1] = (int)tempY;
	}
	int offset = point[0] + 384 * point[1];
	int r = texture[3*offset];
	int g = texture[3*offset+1];
	int b = texture[3*offset+2];
	glColor3ub(r, g, b);
	glBegin(GL_POINTS);
	glVertex2i(i, j);
	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 384; x++) {
			renderPoint(x, y);
		}

	}

	// swap double buffer
	glutSwapBuffers();
}

void init() {
	// clear color buffer
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// set the viewport as the full screen
	glViewport(0, 0, 400, 260);

	// set the projection as orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 400.0, 0.0, 260.0, 1.0, -1.0);

	// set the matrix mode back to modelview mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {

	// create a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// create a window
	glutInitWindowPosition(300, 300);
	glutInitWindowSize(400, 260);
	glutCreateWindow("Computer Graphics");
	init();
	loadImage();

	// register a display callback
	glutDisplayFunc(display);
	glutMotionFunc(processMotion);
	glutIdleFunc(display);

	// event processing loop
	glutMainLoop();

	return 0;
}