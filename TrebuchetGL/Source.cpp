/*
 * GKOM Projekt 15Z - Trebusz
 * Kacper Stachyra
 * 
 *
 * Na podstawie pliku przyk³adowego z laboratorium
 */

#include <windows.h>
#include <GL/gl.h>
#include "GLUT.H"

#include <SOIL.h>

#include <iostream>

#define FRAME_RATE 60
#define REFRESH_TIME 1000/FRAME_RATE

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500

//d³ugoœæ belki podpieraj¹cej
#define BASE_LEN 2.5
//szerokoœæ belki podpieraj¹cej
#define BASE_THICK 0.1
//rozstaw belek podpieraj¹cych
#define TREB_WID 2
//gêstoœæ belek
#define BASE_MASS 1
//grawitacja
#define GRAVITY 0.5
//tarcie
#define FRICTION 1

#define FOG_DENSE 0.25
#define FOG_START 15
#define FOG_END 50

#define M_PI 3.1415926535897932384626433832795
#define SQRT_2 1.41421356237
#define SQRT_3 1.73205080757
#define TO_RADIAN 0.0174532925

GLfloat step = 0.1;
GLfloat angleStep = 0.1;

GLfloat verticalAngle = 0;
GLfloat horizontalAngle = M_PI*3/2;


GLfloat lookX = 0, lookY = 0, lookZ = -1;
GLfloat cameraX = 0, cameraY = 1, cameraZ = 5;
GLfloat upX = 0, upY = 1, upZ = 0;
GLfloat payloadX = 0, payloadY = 0, payloadZ = 0;

//wielkoœæ i masa pocisku
GLfloat payloadSize = 1, payloadMass = 10;
//wielkoœæ i masa przeciwwagi
GLfloat counterSize = 1, counterMass = 100;
//d³ugoœci ramienia miotaj¹cego i ograniczenia
GLfloat r1 = 3, r2 = 2.2;
GLfloat minr1, minr2;
//kat obrotu ramienia miotaj¹cego w stopniach(!) i ograniczenia
GLfloat beamAngle = 0;
GLfloat minBeamAngle;
GLfloat maxBeamAngle;

bool thrown = 0;
bool launched = 0;
bool fogOn = 0;

GLfloat mv[16];
GLuint grassTex, stoneTex, woodTex;

GLfloat lightIntens = 0.3;
GLfloat lightPos = 0;

void init();
void reshape(GLsizei w, GLsizei h);
void display();
	void displayObjects();
		void drawTrebuchet();
		void drawPayload();
	void displayLight(GLfloat intens);
	void displayFog();

void KeyPressedFunc(unsigned char key, int kx, int ky);
void idleFunc();
void nextFrameWait(int* frameTime);

void getPayloadMatrix();
void calcPhysics();
void drawBox(GLfloat size, GLenum type);

int main(int argc, char** argv)
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitWindowPosition( 300, 300 );
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow( "TrebuchetGL - Kacper Stachyra 2016" );

	//funkcja s³u¿¹ca do odrysowania okna, uruchamiana tylko w sytuacjach, gdy okno siê zmienia (rozmiar, widocznoœæ itd.)
	glutDisplayFunc( display );

	//funkcja wywo³ywana przy zmianie rozmiaru okna
	glutReshapeFunc( reshape );

	//funkcja wywo³ywana przy naciœniêciu klawisza zwyk³ego
	glutKeyboardFunc( KeyPressedFunc );

	//funkcja wywo³ywana w stanie spoczynku
	glutIdleFunc( idleFunc );

	init();

	glutMainLoop();

	return 0;
}

void init()
{
	GLfloat mat_ambient[]    = { 1.0, 1.0,  1.0, 1.0 };
	GLfloat mat_specular[]   = { 1.0, 1.0,  1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	GLfloat lm_ambient[]     = { 0.2, 0.2,  0.2, 1.0 };

	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialf( GL_FRONT, GL_SHININESS, 50.0 );

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lm_ambient );
    
	glShadeModel( GL_SMOOTH );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_TEXTURE_2D);

	//³adowanie tekstur trawy, drewna i kamienia
	grassTex = SOIL_load_OGL_texture("textures/grass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	stoneTex = SOIL_load_OGL_texture("textures/stone.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	woodTex = SOIL_load_OGL_texture("textures/wood.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

	/*//³adowanie tekstur skyboxa
	skybox[0] = SOIL_load_OGL_texture("textures/forward.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	skybox[1] = SOIL_load_OGL_texture("textures/backward.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	skybox[2] = SOIL_load_OGL_texture("textures/left.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	skybox[3] = SOIL_load_OGL_texture("textures/right.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
	skybox[4] = SOIL_load_OGL_texture("textures/up.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);*/

	//niebo
	glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
}

void reshape(GLsizei w, GLsizei h)
{
	//"blokowanie" zmiany rozmiaru okna
	/*glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);*/

	//ustawianie vieport i perspektywy widoku wzglêdem rozmiaru okna
	GLfloat ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

/*wyœwietlanie sceny, zerowanie buferów, prze³¹czanie ich itd.*/
void display()
{
	//czyszczenie bufora koloru i bufora g³êbokoœci, by namalowaæ nowe
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//zerowanie macierzy MODELVIEW, na wszelki wypadek
	glLoadIdentity();
	
	//ustawienie kamery
	gluLookAt(cameraX, cameraY, cameraZ,
			cameraX+lookX, cameraY+lookY, cameraZ+lookZ,
			upX, upY,  upZ);

	/*œwiat³¹ i mg³a*/
	displayLight(lightIntens);
	displayFog();

	//w³asna funckja wyœwietlaj¹ca scenê
	displayObjects();

	//opró¿nienie buforowanych zadañ openGL - zrób wsystko co masz do zrobienia
	glFlush();

	//prze³¹czanie buforów
	glutSwapBuffers();
}

/*rysowanie obiektów*/
void displayObjects()
{
	drawTrebuchet();
	drawPayload();

	//pod³o¿e
	glBindTexture (GL_TEXTURE_2D, grassTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*wersja z jednym du¿ym kwadratem - problem z mg³¹, œwiat³o dzia³a lepiej (tajemnicze Ÿród³o œwiat³a...)*/
	/*glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex3f(-10, 0, -10);
		glTexCoord2i(0,64);
		glVertex3f(-10, 0, 10);
		glTexCoord2i(64,64);
		glVertex3f(10, 0, 10);
		glTexCoord2i(64,0);
		glVertex3f(10, 0, -10);
	glEnd();*/

	/*wersja z wieloma kwadratami - mg³a dzia³a doskonale, dziwne zachowanie œwiat³a (tajemnicze Ÿród³o œwiat³a*/
	for (int i=-10; i<=10; ++i)
	{
		for (int j=-10; j<=10; ++j)
		{
			glBegin(GL_QUADS);
				glTexCoord2i(0,0);
				glVertex3f(i*10, 0, j*10);
				glTexCoord2i(0,1);
				glVertex3f(i*10+10, 0, j*10);
				glTexCoord2i(1,1);
				glVertex3f(i*10+10, 0, j*10+10);
				glTexCoord2i(1,0);
				glVertex3f(i*10, 0, j*10+10);
			glEnd();
		}
	}
}
	void drawTrebuchet()
	{
		//lewa podstawa z ³¹czeniami
		GLUquadric *quadric = gluNewQuadric(); 
		gluQuadricTexture(quadric, GL_TRUE);      // Create Texture Coords  
		gluQuadricNormals(quadric, GLU_SMOOTH);   // Create Smooth Normals 

		glBindTexture (GL_TEXTURE_2D, woodTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glPushMatrix();
		glTranslatef(-TREB_WID/2, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
		glRotatef(60, 1, 0, 0);

		gluCylinder(quadric, BASE_THICK, BASE_THICK, BASE_LEN, 16, 5);
		glTranslatef(0, 0, BASE_LEN);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glTranslatef(0, 0, -BASE_LEN);

		glRotatef(60, 1, 0, 0);
		gluSphere(quadric, BASE_THICK, 16, 5);
		gluCylinder(quadric, BASE_THICK, BASE_THICK, BASE_LEN, 16, 5);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glTranslatef(0, 0, BASE_LEN);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glPopMatrix();

		//prawa podstawa z ³¹czeniami
		glPushMatrix();
		glTranslatef(TREB_WID/2, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
		glRotatef(60, 1, 0, 0);

		gluCylinder(quadric, BASE_THICK, BASE_THICK, BASE_LEN, 16, 5);
		glTranslatef(0, 0, BASE_LEN);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glTranslatef(0, 0, -BASE_LEN);

		glRotatef(60, 1, 0, 0);
		gluSphere(quadric, BASE_THICK, 16, 5);
		gluCylinder(quadric, BASE_THICK, BASE_THICK, BASE_LEN, 16, 5);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glTranslatef(0, 0, BASE_LEN);
		gluSphere(quadric, BASE_THICK, 16, 5);
		glPopMatrix();

		//belka poprzeczna
		glPushMatrix();
		glTranslatef(0, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, -TREB_WID/2);
		gluCylinder(quadric, BASE_THICK, BASE_THICK, TREB_WID, 16, 5);
		glPopMatrix();

		//ramiê wyrzucaj¹ce
		glPushMatrix();
		glTranslatef(0, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
		glRotatef(beamAngle, 1, 0, 0);
		glTranslatef(0, 0, -r2);
			//³ycha wyrzucaj¹ca
			glTranslatef(0, 0, +r1+r2);
			const GLfloat throwSize = 5*BASE_THICK;
			gluSphere(quadric, BASE_THICK, 16, 5);	//zakoñczenie ramienia
			glTranslatef(0, 0, throwSize);		//przesuniêcie na sam koniec ramienia

			glBindTexture (GL_TEXTURE_2D, woodTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBegin(GL_QUADS);					//element wyrzucaj¹cy
				glTexCoord2i(1,0);
				glVertex3f(-throwSize, 0, -throwSize);
				glTexCoord2i(1,1);
				glVertex3f(-throwSize, 0, throwSize);
				glTexCoord2i(0,1);
				glVertex3f(throwSize, 0, throwSize);
				glTexCoord2i(0,0);
				glVertex3f(throwSize, 0, -throwSize);
			glEnd();
			glTranslatef(0, 0, -r1-r2-throwSize);	//"powrót" macierzy
			//przeciwwaga
			glTranslatef(0, counterSize/2, 0);
			drawBox(counterSize, GL_QUADS);
			glTranslatef(0, -counterSize/2, 0);
			//ramiê
			gluCylinder(quadric, BASE_THICK, BASE_THICK, r1 + r2, 16, 5);
		glPopMatrix();
	}
	void drawPayload()
	{
		if(!thrown)
		{
			GLUquadric *quadric = gluNewQuadric(); 
			gluQuadricTexture(quadric, GL_TRUE);      // Create Texture Coords  
			gluQuadricNormals(quadric, GLU_SMOOTH);   // Create Smooth Normals 

			glBindTexture (GL_TEXTURE_2D, stoneTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glPushMatrix();
	
			glTranslatef(0, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
			glRotatef(beamAngle, 1, 0, 0);
			glTranslatef(0, 0, -r2);
			glTranslatef(0, payloadSize,  5*BASE_THICK+r1+r2);
	
			gluSphere(quadric, payloadSize, 16, 5);
			glPopMatrix();
		}
		else
		{
			GLUquadric *quadric = gluNewQuadric(); 
			gluQuadricTexture(quadric, GL_TRUE); 
			gluQuadricNormals(quadric, GLU_SMOOTH);

			glBindTexture (GL_TEXTURE_2D, stoneTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glPushMatrix();
			glTranslatef(0, payloadY, payloadZ);
			gluSphere(quadric, payloadSize, 16, 5);
			glPopMatrix();
		}
	}

/*uruchamianie œwiat³a ze zmienn¹ intensywnoœci¹ i po³o¿eniem*/
void displayLight(GLfloat intens)
{
	glDisable(GL_LIGHT0);
	GLfloat light_position[] = { sin(lightPos)*10, 10 ,cos(lightPos)*10 , 1.0 };
	GLfloat lm_ambient[]     = { intens, intens,  0, 1.0 };
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lm_ambient );
	glEnable( GL_LIGHT0 );
}

/*uruchamianie mg³y*/
void displayFog()
{
	if (fogOn)
	{
		glPushMatrix();
		GLfloat fogColor[4]= {0.5f, 0.5f, 0.5f, 1.0f};
		glClearColor(0.5f,0.5f,0.5f,1.0f);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, FOG_DENSE);
		glHint(GL_FOG_HINT, GL_DONT_CARE); //czy wierzcho³ki czy œcianki - obojêtnie
		glFogf(GL_FOG_START,FOG_START);
		glFogf(GL_FOG_END, FOG_END);
		glEnable(GL_FOG);
		glPopMatrix();
	}
	else
	{
		//wy³¹cz mg³ê
		glDisable(GL_FOG);
		//ustaw niebo
		glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
	}
}

/*
funkcja wywo³ywana po naciœniêciu przycisku klawiatury
s³u¿y do obracania kamery i manipulowania parametrami symulacji
*/
void KeyPressedFunc(unsigned char key, int kx, int ky)
{
	switch (key)
	{
		//k¹ty widzenia
		case 'q' :
			horizontalAngle -= angleStep;
			break;
		case 'e' :
			horizontalAngle += angleStep;
			break;
		case 'r' :
			verticalAngle += angleStep;
			break;
		case 'f' :
			verticalAngle -= angleStep;
			break;
		//pozycja kamery
		case 'w' :
			cameraX += lookX * step;
			cameraY += lookY * step;
			cameraZ += lookZ * step;
			break;
		case 's' :
			cameraX -= lookX * step;
			cameraY -= lookY * step;
			cameraZ -= lookZ * step;
			break;
		case 'a' :
			cameraX -= (lookY*upZ - lookZ*upY) * step;
			cameraY -= (lookZ*upX - lookX*upZ) * step;
			cameraZ -= (lookX*upY - lookY*upX) * step;
			break;
		case 'd' :
			cameraX += (lookY*upZ - lookZ*upY) * step;
			cameraY += (lookZ*upX - lookX*upZ) * step;
			cameraZ += (lookX*upY - lookY*upX) * step;
			break;
		case 't' :
			cameraX += upX * step;
			cameraY += upY * step;
			cameraZ += upZ * step;
			break;
		case 'g' :
			cameraX -= upX * step;
			cameraY -= upY * step;
			cameraZ -= upZ * step;
			break;
		case 'b' :
			lightIntens+=0.1;
			break;
		case 'n' :
			lightIntens-=0.1;
			break;
		case 'm' :
			if(fogOn == 1) fogOn = 0;
			else fogOn = 1;
			break;
		case 'o':
			lightPos+=0.1;
			break;
		case 'p':
			lightPos-=0.1;
			break;
	}

	//parametry mo¿na zmieniaæ tylko przed wystrza³em
	if(!launched)
	{
		switch(key)
		{
			case 'y' :
				r1 += 0.1;
				break;
			case 'h' :
				r1 -= 0.1;
				break;
			case 'u' :
				r2 += 0.1;
				break;
			case 'j' :
				r2 -= 0.1;
				break;
			case 'i' :
				beamAngle += 1;
				break;
			case 'k' :
				beamAngle -= 1;
				break;
			case 'l' :
				launched = 1;
				break;
			case 'z' :
				payloadMass += 2;
				break;
			case 'x' :
				payloadMass -= 2;
				break;
			case 'c' :
				counterMass += 5;
				break;
			case 'v' :
				counterMass -= 5;
				break;
		}
	}

	/*ograniczenie zmiany k¹tów widzenia*/
	if (verticalAngle < -M_PI/2) verticalAngle = -M_PI/2;
	else if (verticalAngle > M_PI/2) verticalAngle = M_PI/2;

	if (horizontalAngle >= 2*M_PI) horizontalAngle = 0;
	else if (horizontalAngle <= 0) horizontalAngle = 2*M_PI;

	/*ograniczenia zmiany d³ugoœci ramion*/
	if (r1<minr1) r1 = minr1;
	if (r2<minr2) r2 = minr2;

	/*ograniczenia beamAngle*/
	if (beamAngle < minBeamAngle) beamAngle = minBeamAngle;
	if (beamAngle > maxBeamAngle) beamAngle = maxBeamAngle;

	/*obliczenia wektora look*/
	lookZ = cos(verticalAngle) * sin(horizontalAngle);
	lookX = cos(verticalAngle) * cos(horizontalAngle);
	lookY = sin(verticalAngle);

	/*obliczanie wektora up*/
	verticalAngle += M_PI/2;
	upZ = cos(verticalAngle) * sin(horizontalAngle);
	upX = cos(verticalAngle) * cos(horizontalAngle);
	upY = sin(verticalAngle);
	verticalAngle -= M_PI/2;

	std::cout<<"payM: "<<payloadMass<<"\tcounterM: "<<counterMass<<"\n";
}

/*
funkcja wywo³ywana w pêtli, gdy nie ma nic innego do zrobienia
tutaj znajduje siê obliczanie stanu ca³ej sceny i jej rysowanie
*/
void idleFunc()
{
	static int frameTime = glutGet(GLUT_ELAPSED_TIME);
	calcPhysics();
	nextFrameWait(&frameTime);
	glutPostRedisplay();
}

/*
wywo³anie funkcji nexFrameWait powoduje zawieszenie programu do momentu, a¿ od ostatniego wywo³ania tej funkcji minie REFRESH_TIME
do wywo³ywania przed funkcj¹ glutPostRedisplay();
*/
void nextFrameWait(int* frameTime)
{
	/*wersja z aktywnym oczekiwaniem
	while (glutGet(GLUT_ELAPSED_TIME) - *frameTime < REFRESH_TIME);
	*frameTime = glutGet(GLUT_ELAPSED_TIME);
	*/

	/*wersja ze Sleep pomiêdzy klatkami*/
	int toWait = REFRESH_TIME - (glutGet(GLUT_ELAPSED_TIME) - *frameTime);
	if (toWait>0)
	{
		Sleep(toWait);
	}

	*frameTime = glutGet(GLUT_ELAPSED_TIME);
}

/*funkcja s³u¿¹ca do "odzyskiwania" macierzy MODEL dla przekszta³ceñ pocisku, powtórzone przekszta³cenia macierzy z funkcji
drawPayload, z wy³¹czeniem macierzy VIEW (gluLookAt), wywo³ywana raz, gdy potrzebna - wa¿ne, pilnowaæ takiego sameo kodu jaj w drawPayload()*/
void getPayloadMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
	glRotatef(beamAngle, 1, 0, 0);
	glTranslatef(0, 0, -r2);
	glTranslatef(0, payloadSize,  5*BASE_THICK+r1+r2);
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	glPopMatrix();
}

/*obliczanie fizyki animacji, wywo³ywana z idleFunc*/
void calcPhysics()
{
	static GLfloat inertia = 0;
	static GLfloat payloadSpeedY = 0;
	static GLfloat payloadSpeedZ = 0;
	if (!launched)
	{
		//rozmiary przeciwwagi i pocisku
		payloadSize = 0.2 + payloadMass*0.01;
		counterSize = counterMass*0.01;

		//ograniczenia d³ugoœci ramienia wyrzucaj¹cego
		minr1 = BASE_LEN*SQRT_3/2 - 5*BASE_THICK;
		minr2 = BASE_LEN*SQRT_3/2;

		//ograniczenia k¹ta ramienia wyrzucaj¹cego
		minBeamAngle = -asin((BASE_LEN*SQRT_3/2)/(r2+counterSize/2)) * (180/M_PI);
		maxBeamAngle = asin((BASE_LEN*SQRT_3/2)/(r1 + 10*BASE_THICK)) * (180/M_PI);

		inertia = (1/3)*(r1+r2)*(r1+r2)*BASE_MASS + counterMass + counterMass*r2*r2 + payloadMass + payloadMass*r1*r1;
	}

	else if (!thrown)
	{
		//dynamika bry³y sztywnej
		static GLfloat angleSpeed = 0;
		GLfloat sinus = sin(90-abs(beamAngle*TO_RADIAN));
		GLfloat force = (counterMass*r2*sinus + sinus*BASE_MASS*r2/2 - payloadMass*r1*sinus -sinus*BASE_MASS*r2/2)*GRAVITY;
		GLfloat angleAcc = force/inertia;
		angleSpeed += angleAcc/(FRAME_RATE);
		beamAngle-=(angleSpeed*(180/M_PI));

		std::cout<<"f: "<<force<<"\ti: "<<inertia<<"\ta: "<<angleAcc<<"\tv: "<<angleSpeed<<"\tbA: "<<beamAngle<<"\n";
		if (beamAngle<=minBeamAngle || beamAngle>=maxBeamAngle)
		{
			if (beamAngle>=maxBeamAngle)
			{
				beamAngle=maxBeamAngle;
				angleSpeed = 0;
				launched = 0;
			}
			else if(beamAngle<=minBeamAngle)
			{
				//pobiera pozycjê kuli
				getPayloadMatrix();
				payloadX = mv[12]/mv[15];
				payloadY = mv[13]/mv[15];
				payloadZ = mv[14]/mv[15];

				//wektor prêdkoœci kuli
				GLfloat payloadAngle = (90 + beamAngle)*TO_RADIAN;
				payloadSpeedY = sin(payloadAngle)*r1*angleSpeed*(180/M_PI);
				payloadSpeedZ = cos(payloadAngle)*r1*angleSpeed*(180/M_PI);

				//zatrzymaj ramiê
				beamAngle=minBeamAngle;
				angleSpeed = 0;

				//zmieñ status
				thrown = 1;
			}
		}
	}

	else //thrown
	{
		if (payloadY > payloadSize/2)
		{
			payloadY += payloadSpeedY/FRAME_RATE;
			payloadZ -= payloadSpeedZ/FRAME_RATE;
			payloadSpeedY -= GRAVITY/5;
		}
		else if (payloadSpeedZ > 0)
		{
			payloadZ -= payloadSpeedZ/FRAME_RATE;
			payloadSpeedZ -= FRICTION;
		}
		else
		{
			thrown = 0;
			launched = 0;
		}

	}
}

/*zmodyfikowana do w³asnych potrzeb funkcja rysuj¹ca szeœcian z biblioteki glut - rysuje przeciwwagê*/
void drawBox(GLfloat size, GLenum type)
{
	static GLfloat n[6][3] =
	{
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] =
	{
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 4;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 4;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--)
	{
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glTexCoord2i(0,0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2i(1,0);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2i(1,1);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2i(0,1);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}