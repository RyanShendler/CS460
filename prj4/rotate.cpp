#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>

//flower.bmp has width of 384 pixels and height of 256 pixels
unsigned char texture[384*256*3];

// the angle variable for rotation
float angle = 0.0f;

void loadImage() {
	FILE* file;
	file = fopen("flower.bmp", "rb");
	if (!file) {
		printf("file couldn't be opened\n");
		exit(0);
	}
	unsigned char header[54];
	fread(header, 1, 54, file);
	fread(texture, 1, 384*256*3, file);
	for (int i = 0; i < 384 * 256; i++) {
		int index = 3 * i;
		unsigned char B, R;
		B = texture[index];
		R = texture[index+2];
		texture[index] = R;
		texture[index + 2] = B;
	}
	fclose(file);
}

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 800, 600);
	glOrtho(-208.0, 592.0, -172.0, 428.0, 1.0, -1.0);
	//glViewport(0, 0, 900);
	//glOrtho();

	// set the matrix mode back to modelview mode
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluPerspective(45, 800.0 / 600.0, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void display() {

	// clear buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset transformations
	glLoadIdentity();

	// set the camera position and direction
	//gluLookAt(0.0f, 0.0f, 10.0f,
		//400.0f, 300.0f, 0.0f,
		//0.0f, 1.0f, 0.0f);

	// set the rotation matrix
	glRotatef(angle, 0.0f, 0.0f, 1.0f);

	//glRasterPos2i(-194, -128);
	glRasterPos2i(-150, -80);
	glDrawPixels(384, 256, GL_RGB, GL_UNSIGNED_BYTE, texture);
	// update the angle
	//angle += 0.05f;

	// swap double buffer
	glutSwapBuffers();
}

void idle() {
	angle += 0.05f;
	if ((int)angle % 30) {
		glutPostRedisplay();
	}
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
	loadImage();

	// register a display callback
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	// event processing loop
	glutMainLoop();

	return 0;
}