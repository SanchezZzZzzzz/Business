#include "glew.h"
#include "glut.h"
#include "ModelLoader.cpp"
#include <cmath>
using namespace std;
typedef GLfloat GLTVector3[3];
typedef GLfloat GLTVector2[2];//Двукомпонентный век-тор с плавающей запятой
typedef GLfloat GLTVector3[3];//Трехомпонентный век-тор с плавающей запятой
typedef GLfloat GLTVector4[4];//Четырехкомпонентный вектор с плавающей //запятой
typedef GLfloat GLTMatrix[16];// Основноц столбец матрицы 4х4 с плавающей //запятой

GLTMatrix shadowMat;


GLfloat l1position[] = { 0, 80, 100, 1 };
GLfloat l2position[] = { 0, 100, 100, 1 };
GLfloat l3position[] = { 150, 90, 0, 1 };
GLfloat l4position[] = { -150, 90, 0, 1 };
ml::Model car;
static GLfloat MovX = 0, MovZ = 0, RsX = 0, RsY = 0;

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
void ProcessMenu(int value);

int main(int argc, char* argv[])
{
	std::setlocale(LC_NUMERIC, "POSIX");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("Машинка");
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Switch light source 0", 1);
	glutAddMenuEntry("Switch light source 1", 2);
	glutAddMenuEntry("Switch light source 2", 3);
	glutAddMenuEntry("Switch light source 3", 4);
	glutAddMenuEntry("Light model ambient", 5);
	glutAddMenuEntry("Ambient material", 8);
	glutAddMenuEntry("Emission material", 9);
	glutAddMenuEntry("Diffuse material", 10);
	glutAddMenuEntry("Specular material", 11);
	glutAddMenuEntry("Linear fog", 12);
	glutAddMenuEntry("Exp fog", 13);
	glutAddMenuEntry("Exp2 fog", 14);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	car.Import_objects("car.obj");
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
	glRotatef(RsX, 1, 0, 0);
	glRotatef(RsY, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, l1position);
	glLightfv(GL_LIGHT1, GL_POSITION, l2position);
	glLightfv(GL_LIGHT2, GL_POSITION, l3position);
	glLightfv(GL_LIGHT3, GL_POSITION, l4position);
	glBegin(GL_QUADS);
	glColor3ub(0, 32, 0);
	glVertex3f(1000.0f, -150.0f, -1000.0f);
	glVertex3f(-1000.0f, -150.0f, -1000.0f);
	glColor3ub(0, 255, 0);
	glVertex3f(-1000.0f, -150.0f, 1000.0f);
	glVertex3f(-1000.0f, -150.0f, 1000.0f);
	glEnd();
	car.Translate(MovX, -100.0f, MovZ);
	car.Draw_objects(1, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glMultMatrixf((GLfloat *)shadowMat);
	car.Draw_objects(0, 1);
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
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, ambientLight);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, ambientLight);
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT3, GL_SPECULAR, ambientLight);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 1000);
	gltMakeShadowMatrix(points, l3position, shadowMat);
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
	gluPerspective(60.0f, fAspect, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keys(int key, int x, int y) {
	int Skey = glutGetModifiers();
	if (Skey != GLUT_ACTIVE_SHIFT) {
		if (key == GLUT_KEY_UP)
			MovZ -= 5;
		if (key == GLUT_KEY_DOWN)
			MovZ += 5;
		if (key == GLUT_KEY_LEFT)
			MovX -= 5;
		if (key == GLUT_KEY_RIGHT)
			MovX += 5;
	}
	else {
		if (key == GLUT_KEY_UP) {
			RsX += 5;
			if (RsX > 360)
				RsY = 0;
		}
		if (key == GLUT_KEY_DOWN) {
			RsX -= 5;
			if (RsX > 360)
				RsY = 0;
		}
		if (key == GLUT_KEY_LEFT) {
			RsY += 5;
			if (RsY > 360)
				RsY = 0;
		}
		if (key == GLUT_KEY_RIGHT) {
			RsY -= 5;
			if (RsY > 360)
				RsY = 0;
		}
	}
	glutPostRedisplay();
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
int SwLight0 = 1, SwLight1 = 1, SwLight2 = 1, SwLight3 = 1;
void ProcessMenu(int value) {
	const GLfloat lmod2 = 0;
	GLfloat lmod1[] = { 0.5, 0.5, 0.5, 1 };
	GLfloat fLowLight[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	switch (value) {
	case 1:
		SwLight0 = (SwLight0 + 1) % 2;
		if (SwLight0 == 0)
			glEnable(GL_LIGHT0);
		else
			glDisable(GL_LIGHT0);
		break;
	case 2:
		SwLight1 = (SwLight1 + 1) % 2;
		if (SwLight1 == 0)
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);
		break;
	case 3:
		SwLight2 = (SwLight2 + 1) % 2;
		if (SwLight1 == 0)
			glEnable(GL_LIGHT2);
		else
			glDisable(GL_LIGHT2);
		break;
	case 4:
		SwLight3 = (SwLight3 + 1) % 2;
		if (SwLight3 == 0)
			glEnable(GL_LIGHT3);
		else
			glDisable(GL_LIGHT3);
		break;
	case 5:	
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmod1);
		break;
	case 6:
		glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, &lmod2);
		break;
	case 7:
		glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, &lmod2);
		break;
	case 8:
		glColorMaterial(GL_FRONT, GL_AMBIENT);
		break;
	case 9:
		glColorMaterial(GL_FRONT, GL_EMISSION);
		break;
	case 10:
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		break;
	case 11:
		glColorMaterial(GL_FRONT, GL_SPECULAR);
		break;
	case 12:
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, fLowLight);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		break;
	case 13:
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, fLowLight);
		glFogf(GL_FOG_DENSITY, 0.004);
		glFogi(GL_FOG_MODE, GL_EXP);
		break;
	case 14:
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, fLowLight);
		glFogf(GL_FOG_DENSITY, 0.002);
		glFogi(GL_FOG_MODE, GL_EXP2);
		break;
	}
}
