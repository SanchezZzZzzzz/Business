#include "glew.h"
#include "glut.h"
#include "ModelLoader.h"
#include <cmath>
using namespace std;
typedef GLfloat GLTVector3[3];
typedef GLfloat GLTVector2[2];//Двукомпонентный век-тор с плавающей запятой
typedef GLfloat GLTVector3[3];//Трехомпонентный век-тор с плавающей запятой
typedef GLfloat GLTVector4[4];//Четырехкомпонентный вектор с плавающей //запятой
typedef GLfloat GLTMatrix[16];// Основноц столбец матрицы 4х4 с плавающей //запятой
GLTMatrix shadowMat;

GLfloat l1pos[] = { 150, 90, 0, 1 };
GLfloat l1dir[] = { -1, 0, 0 };
GLfloat l2pos[] = { -150, 90, 0, 1 };
GLfloat l2dir[] = { 1, 0, 0 };
ml::Model snowman;
static GLfloat MovX = 0, MovZ = 0;


void RenderScene(void);
void SetupRC();
void ChangeSize(int w, int h);
void Keys(int key, int x, int y);
void CreatePlain(GLfloat y, GLfloat size);
void gltScaleVector(GLTVector3 vVector, const GLfloat fScale);
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector);
void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult);
void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult);
void gltNormalizeVector(GLTVector3 vNormal);
void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal);
GLfloat gltGetVectorLength(const GLTVector3 vVector);
void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane);
void gltMakeShadowMatrix(GLTVector3 vPoints[3], GLTVector4 vLightPos, GLTMatrix destMat);

int main(int argc, char* argv[])
{
	std::setlocale(LC_NUMERIC, "POSIX");
	glutInit(&argc, argv);
	cout << "Starting..." << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Snowman");
	snowman.Model_import("car.obj");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(Keys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}
void RenderScene(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, l1pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l1dir);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_POSITION, l2pos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l2dir);
	glEnable(GL_LIGHT1);
	glBegin(GL_QUADS);
	glColor3ub(0, 32, 0);
	glVertex3f(400.0f, -150.0f, -200.0f);
	glVertex3f(-400.0f, -150.0f, -200.0f);
	glColor3ub(0, 255, 0);
	glVertex3f(-400.0f, -150.0f, 200.0f);
	glVertex3f(400.0f, -150.0f, 200.0f);
	glEnd();
	glPushMatrix();
	glTranslatef(0, -130, 0);
	glTranslatef(MovX, 0, 0);
	glTranslatef(0, 0, MovZ);
	snowman.Model_draw(1, 0);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glMultMatrixf((GLfloat *)shadowMat);
	glTranslatef(MovX, 0, 0);
	glTranslatef(0, 0, MovZ);
	snowman.Model_draw(0, 1);
	glPopMatrix();
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glutSwapBuffers();
}

void SetupRC() {
	GLTVector3 points[3] = { { -30.0f, -150.0f, -20.0f }, { -30.0f, -150.0f, 20.0f }, { 40.0f, -150.0f, 20.0f } };
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	GLfloat ambientLight[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1 };
	GLfloat specular[] = { 1, 1, 1, 1 };
	GLfloat specref[] = { 1, 1, 1, 1 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, ambientLight);
	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0f);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, ambientLight);
	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0f); 

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	gltMakeShadowMatrix(points, l1pos, shadowMat);
}

void ChangeSize(int w, int h)
{
	GLfloat fAspect;
	// Предотвращает деление на ноль
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, fAspect, 200, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -300);
	//glLightfv(GL_LIGHT0, GL_POSITION, l1pos);
}

void Keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		MovZ -= 5;
	if (key == GLUT_KEY_DOWN)
		MovZ += 5;
	if (key == GLUT_KEY_LEFT)
		MovX -= 5;
	if (key == GLUT_KEY_RIGHT)
		MovX += 5;
	glutPostRedisplay();
}

void CreatePlain(GLfloat y, GLfloat size) {
	//glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glColor3ub(0, 32, 0);
	glVertex3f(size / 2, y, size / 2);
	glVertex3f(size / 2, y, -size / 2);
	glColor3ub(0, 255, 0);
	glVertex3f(-size / 2, y, -size / 2);
	glVertex3f(-size/2, y, size/2);
	glEnd();
}

void gltMakeShadowMatrix(GLTVector3 vPoints[3], GLTVector4 vLightPos, GLTMatrix destMat)
{
	GLTVector4 vPlaneEquation;
	GLfloat dot;
	gltGetPlaneEquation(vPoints[0], vPoints[1], vPoints[2], vPlaneEquation);
		// Скалярное произведение положение самолета и света
		dot = vPlaneEquation[0] * vLightPos[0] +
		vPlaneEquation[1] * vLightPos[1] +
		vPlaneEquation[2] * vLightPos[2] +
		vPlaneEquation[3] * vLightPos[3];
	// Проецируем
	// Первый столбец
	destMat[0] = dot - vLightPos[0] * vPlaneEquation[0];
	destMat[4] = 0.0f - vLightPos[0] * vPlaneEquation[1];
	destMat[8] = 0.0f - vLightPos[0] * vPlaneEquation[2];
	destMat[12] = 0.0f - vLightPos[0] * vPlaneEquation[3];
	// Второй столбец
	destMat[1] = 0.0f - vLightPos[1] * vPlaneEquation[0];
	destMat[5] = dot - vLightPos[1] * vPlaneEquation[1];
	destMat[9] = 0.0f - vLightPos[1] * vPlaneEquation[2];
	destMat[13] = 0.0f - vLightPos[1] * vPlaneEquation[3];
	// Третий столбец
	destMat[2] = 0.0f - vLightPos[2] * vPlaneEquation[0];
	destMat[6] = 0.0f - vLightPos[2] * vPlaneEquation[1];
	destMat[10] = dot - vLightPos[2] * vPlaneEquation[2];
	destMat[14] = 0.0f - vLightPos[2] * vPlaneEquation[3];
	// Четвертый столбец
	destMat[3] = 0.0f - vLightPos[3] * vPlaneEquation[0];
	destMat[7] = 0.0f - vLightPos[3] * vPlaneEquation[1];
	destMat[11] = 0.0f - vLightPos[3] * vPlaneEquation[2];
	destMat[15] = dot - vLightPos[3] * vPlaneEquation[3];
}

void gltScaleVector(GLTVector3 vVector, const GLfloat fScale)
{
	vVector[0] *= fScale; vVector[1] *= fScale; vVector[2] *= fScale;
}
// Возвращает длину вектора в квадрате
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector)
{
	return (vVector[0] * vVector[0]) + (vVector[1] * vVector[1]) + (vVector[2] * vVector[2]);
}
// Возвращает длину вектора
GLfloat gltGetVectorLength(const GLTVector3 vVector)
{
	return (GLfloat)sqrt(gltGetVectorLengthSqrd(vVector));
}
// Вычитание одного вектора из другого
void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult)
{
	vResult[0] = vFirst[0] - vSecond[0];
	vResult[1] = vFirst[1] - vSecond[1];
	vResult[2] = vFirst[2] - vSecond[2];
}
// Вычислить векторное произведение двух векторов
void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult)
{
	vResult[0] = vU[1] * vV[2] - vV[1] * vU[2];
	vResult[1] = -vU[0] * vV[2] + vV[0] * vU[2];
	vResult[2] = vU[0] * vV[1] - vV[0] * vU[1];
}
// Масштабирование вектора по длине - создание еди-ничсного вектора
void gltNormalizeVector(GLTVector3 vNormal)
{
	GLfloat fLength = 1.0f / gltGetVectorLength(vNormal);
	gltScaleVector(vNormal, fLength);
}
// Три точки на плоскости расположены против часовой стрелки, вычисление
// нормали
void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal)
{
	GLTVector3 vV1, vV2;
	gltSubtractVectors(vP2, vP1, vV1);
	gltSubtractVectors(vP3, vP1, vV2);
	gltVectorCrossProduct(vV1, vV2, vNormal);
	gltNormalizeVector(vNormal);
}
// Полученные три коэффициента уравнения плоскости дают три точки на поверхности
void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane)
{
	// Получение нормали из трех точек. Нормаль – первые три коэффициента
	// уравнения плоскости
	gltGetNormalVector(vPoint1, vPoint2, vPoint3, vPlane);
	// Итоговый коэффициент находится обратной под-становкой
	vPlane[3] = -(vPlane[0] * vPoint3[0] + vPlane[1] * vPoint3[1] + vPlane[2] * vPoint3[2]);
}
