#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
#include <random>

bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}


double STEPUGOL = M_PI / 50; //chng
double STEPHEIGHT = 1; //chng

GLuint texId[10];
const int maxnum = 3;
int curnum = 0;
void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'U')
	{
		STEPUGOL -= M_PI / 500;
	}

	if (key == 'I')
	{
		STEPUGOL += M_PI / 500;
	}

	if (key == 'H')
	{
		STEPHEIGHT -= 0.1;
	}

	if (key == 'J')
	{
		STEPHEIGHT += 0.1;
	}

	if (key == 'M')
	{
		if (curnum < maxnum - 1)
		{
			++curnum;
		}
	}

	if (key == 'N')
	{
		if (curnum > 0)
		{
			--curnum;
		}
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}





//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	
	for (int i = 0; i < maxnum; ++i)
	{
		//массив трехбайтных элементов  (R G B)
		RGBTRIPLE* texarray;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray;
		int texW, texH;
		std::string s = "texture" + std::to_string(i) + ".bmp";
		OpenGL::LoadBMP(s.data(), &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//генерируем ИД для текстуры

		glGenTextures(1, &texId[i]);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, texId[i]);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//отчистка памяти
		free(texCharArray);
		free(texarray);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  


	//Начало рисования квадратика станкина
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/

	const double STEPS = 60;
	const double TOPCOORDZ = 20;
	const int GRN = 2; //Number of point before green side
	int PRP = 5; //Number of point before purple side
	const double NRPRP[] = { 2, -6, 1 }; //Point near the purple side
	const double CNTRPNT[] = { 0, 0, TOPCOORDZ };
	std::vector<std::vector<double>> IPNTS;
	IPNTS.push_back({ -8, 2, TOPCOORDZ }); //0 (A)
	IPNTS.push_back({ 0, 1, TOPCOORDZ }); //1 (B)
	IPNTS.push_back({ 2, 6, TOPCOORDZ }); //2 (C)
	IPNTS.push_back({ 5, 3, TOPCOORDZ }); //3 (D)
	IPNTS.push_back({ 1, 0, TOPCOORDZ }); //4 (E)
	IPNTS.push_back({ 5, -6, TOPCOORDZ }); //5 (F)
	IPNTS.push_back({ -1, -9, TOPCOORDZ }); //6 (G)
	IPNTS.push_back({ -1, -1, TOPCOORDZ }); //7 (H)
	//copy A point
	IPNTS.push_back({ -8, 2, TOPCOORDZ }); //8 (A)




	//Calculate points for green side
	double diameter = std::sqrt(std::pow(std::abs(IPNTS[GRN][0] - IPNTS[GRN + 1][0]), 2) + std::pow(std::abs(IPNTS[GRN][1] - IPNTS[GRN + 1][1]), 2));
	double CN[] = { (IPNTS[GRN][0] + IPNTS[GRN + 1][0]) / 2, (IPNTS[GRN][1] + IPNTS[GRN + 1][1]) / 2, TOPCOORDZ };
	double OC[] = { IPNTS[GRN][0] - CN[0], IPNTS[GRN][1] - CN[1], IPNTS[GRN][2] };
	double OLD0 = IPNTS[GRN][0], OLD1 = IPNTS[GRN][1];

	int INI = IPNTS.size() - GRN;
	int counter = 0;
	for (double i = M_PI / 16; i < M_PI; i += M_PI / 16)
	{
		IPNTS.insert(IPNTS.end() - INI + 1, { OC[0] * cos(i) + OC[1] * sin(i) + CN[0], -OC[0] * sin(i) + OC[1] * cos(i) + CN[1],TOPCOORDZ });
		++counter;
	}
	if (PRP > GRN)
	{
		PRP += counter;
	}

	double A = NRPRP[0] - IPNTS[PRP][0];
	double B = NRPRP[1] - IPNTS[PRP][1];
	double C = IPNTS[PRP + 1][0] - IPNTS[PRP][0];
	double D = IPNTS[PRP + 1][1] - IPNTS[PRP][1];
	double E = A * (IPNTS[PRP][0] + NRPRP[0]) + B * (IPNTS[PRP][1] + NRPRP[1]);
	double F = C * (IPNTS[PRP + 1][0] + IPNTS[PRP][0]) + D * (IPNTS[PRP + 1][1] + IPNTS[PRP][1]);
	double G = 2 * (A * (IPNTS[PRP + 1][1] - NRPRP[1]) - B * (IPNTS[PRP + 1][0] - NRPRP[0]));
	if (G == 0)
	{
		return;
	}

	double Cx = (D * E - B * F) / G;
	double Cy = (A * F - C * E) / G;

	double CNTRPRLP[] = { Cx, Cy, TOPCOORDZ };


	double radius = std::sqrt(std::pow(std::abs(IPNTS[PRP][0] - CNTRPRLP[0]), 2) + std::pow(std::abs(IPNTS[PRP][1] - CNTRPRLP[1]), 2));
	double hordlen = std::sqrt(std::pow(std::abs(IPNTS[PRP][0] - IPNTS[PRP + 1][0]), 2) + std::pow(std::abs(IPNTS[PRP][1] - IPNTS[PRP + 1][1]), 2));
	double ugol = 2 * std::asin(hordlen / (2 * radius));
	double POC[] = { IPNTS[PRP][0] - CNTRPRLP[0], IPNTS[PRP][1] - CNTRPRLP[1], TOPCOORDZ };

	int PINI = IPNTS.size() - PRP;
	for (double i = ugol / 16; i < ugol; i += (ugol / 16))
	{
		IPNTS.insert(IPNTS.end() - PINI + 1, { POC[0] * cos(i) - POC[1] * sin(i) + CNTRPRLP[0], POC[0] * sin(i) + POC[1] * cos(i) + CNTRPRLP[1],TOPCOORDZ });
	}




	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);




	std::vector<std::vector<double>> OLDPNTS(IPNTS);
	std::vector<std::vector<double>> NEWPNTS(IPNTS);

	glBindTexture(GL_TEXTURE_2D, texId[curnum]);
	glColor3d(0.5, 0.5, 0.5);

	double gu = 0;
	for (int j = 0; j < STEPS; ++j)
	{
		gu = STEPUGOL;
		for (int i = 0; i < NEWPNTS.size(); ++i)
		{
			NEWPNTS[i].data()[0] = OLDPNTS[i].data()[0] * cos(gu) + OLDPNTS[i].data()[1] * sin(gu);
			NEWPNTS[i].data()[1] = -OLDPNTS[i].data()[0] * sin(gu) + OLDPNTS[i].data()[1] * cos(gu);
			NEWPNTS[i].data()[2] -= STEPHEIGHT;
		}
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < OLDPNTS.size() - 1; ++i)
		{
			//glColor4d(distribution(generator), distribution(generator), distribution(generator), 0.5);
			//glColor3d(distribution(generator), distribution(generator), distribution(generator));
			{
				std::vector<std::vector<double>> veofnm;
				veofnm.push_back(OLDPNTS[i + 1]);
				veofnm.push_back(NEWPNTS[i]);
				veofnm.push_back(OLDPNTS[i]);

				double ax, ay, az,
					bx, by, bz;
				ax = veofnm[1][0] - veofnm[0][0];
				ay = veofnm[1][1] - veofnm[0][1];
				az = veofnm[1][2] - veofnm[0][2];

				bx = veofnm[2][0] - veofnm[0][0];
				by = veofnm[2][1] - veofnm[0][1];
				bz = veofnm[2][2] - veofnm[0][2];

				double nm[] = { ay * bz - by * az, az * bx - bz * ax, ax * by - bx * ay };
				double znam = std::sqrt(nm[0] * nm[0] + nm[1] * nm[1] + nm[2] * nm[2]);
				for (int i = 0; i < 3; ++i)
				{
					nm[i] /= znam;
				}

				//glBegin(GL_LINES);
				//glVertex3dv(OLDPNTS[i].data());
				//glVertex3d(nm[0] + OLDPNTS[i][0], nm[1] + OLDPNTS[i][1], nm[2] + OLDPNTS[i][2]);

				//glVertex3dv(OLDPNTS[i + 1].data());
				//glVertex3d(nm[0] + OLDPNTS[i + 1][0], nm[1] + OLDPNTS[i + 1][1], nm[2] + OLDPNTS[i + 1][2]);

				//glVertex3dv(NEWPNTS[i].data());
				//glVertex3d(nm[0] + NEWPNTS[i][0], nm[1] + NEWPNTS[i][1], nm[2] + NEWPNTS[i][2]);
				//glEnd();
			
				
				glNormal3dv(nm);
			}

			glTexCoord2d(1, 1);
			glVertex3dv(OLDPNTS[i].data());
			glTexCoord2d(0, 1);
			glVertex3dv(OLDPNTS[i + 1].data());
			glTexCoord2d(1, 0);
			glVertex3dv(NEWPNTS[i].data());
			{
				std::vector<std::vector<double>> veofnm;
				veofnm.push_back(NEWPNTS[i + 1]);
				veofnm.push_back(NEWPNTS[i]);
				veofnm.push_back(OLDPNTS[i + 1]);

				double ax, ay, az,
					bx, by, bz;
				ax = veofnm[1][0] - veofnm[0][0];
				ay = veofnm[1][1] - veofnm[0][1];
				az = veofnm[1][2] - veofnm[0][2];

				bx = veofnm[2][0] - veofnm[0][0];
				by = veofnm[2][1] - veofnm[0][1];
				bz = veofnm[2][2] - veofnm[0][2];

				double nm[] = { ay * bz - by * az, az * bx - bz * ax, ax * by - bx * ay };

				double znam = std::sqrt(nm[0] * nm[0] + nm[1] * nm[1] + nm[2] * nm[2]);
				for (int i = 0; i < 3; ++i)
				{
					nm[i] /= znam;
				}

				//glBegin(GL_LINES);
				//glVertex3dv(NEWPNTS[i].data());
				//glVertex3d(nm[0] + NEWPNTS[i][0], nm[1] + NEWPNTS[i][1], nm[2] + NEWPNTS[i][2]);

				//glVertex3dv(NEWPNTS[i + 1].data());
				//glVertex3d(nm[0] + NEWPNTS[i + 1][0], nm[1] + NEWPNTS[i + 1][1], nm[2] + NEWPNTS[i + 1][2]);

				//glVertex3dv(OLDPNTS[i + 1].data());
				//glVertex3d(nm[0] + OLDPNTS[i + 1][0], nm[1] + OLDPNTS[i + 1][1], nm[2] + OLDPNTS[i + 1][2]);
				//glEnd();

				glNormal3dv(nm);
			}
			glTexCoord2d(1, 0);
			glVertex3dv(NEWPNTS[i].data());
			glTexCoord2d(0, 0);
			glVertex3dv(NEWPNTS[i + 1].data());
			glTexCoord2d(0, 1);
			glVertex3dv(OLDPNTS[i + 1].data());



		}
		glEnd();
		OLDPNTS = NEWPNTS;
	}

	glBegin(GL_TRIANGLES);

	double NEWCNTRPNT[] = { CNTRPNT[0], CNTRPNT[1], OLDPNTS[0][2] };
	for (int i = 0; i < OLDPNTS.size() - 1; ++i)
	{
		glNormal3d(0, 0, -1);
		//glColor4d(distribution(generator), distribution(generator), distribution(generator), 0.3);

		glVertex3dv(NEWCNTRPNT);
		glVertex3dv(OLDPNTS[i].data());
		glVertex3dv(OLDPNTS[i + 1].data());
	}

	glEnd();

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < IPNTS.size() - 1; ++i)
	{

		glNormal3d(0, 0, 1);
		//glColor4d(distribution(generator), distribution(generator), distribution(generator), 0.3);

		glVertex3dv(CNTRPNT);
		glVertex3dv(IPNTS[i].data());
		glVertex3dv(IPNTS[i + 1].data());
	}
	glEnd();
	//конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}