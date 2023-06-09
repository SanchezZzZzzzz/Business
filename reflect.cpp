#include "glew.h"
#include "glut.h"
#include "ModelLoader.cpp"
#include <cmath>
using namespace std;
ml::Model car;
static GLfloat RotX = 0, RotY = 0;
void RenderScene(void);
void SetupRC();
void ChangeSize(int w, int h);
void Keys(int key, int x, int y);
int main(int argc, char* argv[])
{
	std::setlocale(LC_NUMERIC, "POSIX");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("машинка");
	car.Import_objects("car.obj");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(Keys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}
void RenderScene(void) {
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glRotatef(RotX, 1, 0, 0);
	glRotatef(RotY, 0, 1, 0);
	car.Draw_objects(1, 0);
	glPopMatrix();
	glutSwapBuffers();
}
// Функция выполняет всю необходимую инициализацию в контексте
//визуализации
void SetupRC() {
	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1 };
	GLfloat specular[] = { 1, 1, 1, 1 };
	GLfloat specref[] = { 1, 1, 1, 1 };
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ambientLight);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}
void ChangeSize(int w, int h)
{
	GLfloat range = 20;
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-range, range, range * h / w, -range * h / w, -range * 2, range * 2);
	else
		glOrtho(-range * w / h, range * w / h, range, -range, -range * 2, range * 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void Keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		RotX -= 5 % 360;
	if (key == GLUT_KEY_DOWN)
		RotX += 5 % 360;
	if (key == GLUT_KEY_LEFT)
		RotY -= 5 % 360;
	if (key == GLUT_KEY_RIGHT)
		RotY += 5 % 360;
	glutPostRedisplay();
}
