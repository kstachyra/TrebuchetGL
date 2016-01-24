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

#include <iostream>

#define FRAME_RATE 60
#define REFRESH_TIME 1000/FRAME_RATE

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

//d³ugoœæ belki podpieraj¹cej
#define BASE_LEN 2.5
//szerokoœæ belki podpieraj¹cej
#define BASE_THICK 0.1
//rozstaw belek podpieraj¹cych
#define TREB_WID 2

#define M_PI 3.1415926535897932384626433832795
#define SQRT_2 1.41421356237
#define SQRT_3 1.73205080757
#define EPSILON 0.001

double verticalAngle = 0;
double horizontalAngle = M_PI*3/2;
double step = 0.1;
double angleStep = 0.1;

double lookX = 0, lookY = 0, lookZ = -1;
double cameraX = 0, cameraY = 0, cameraZ = 5;
double upX = 0, upY = 1, upZ = 0;

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
}

void drawPodst()
{
	//lewa podstawa
	glPushMatrix();
	glTranslatef(-TREB_WID/2, BASE_LEN*SQRT_3/2 + BASE_THICK, 0);
	glRotatef(60, 1, 0, 0);

	GLUquadric *quadric = gluNewQuadric();
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

	//prawa podstawa
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



}

void displayObjects(int frame_no)
{
	GLfloat cube_diffuse[]   = { 0.0, 0.7, 0.7, 1.0 };

	drawPodst();
	glPushMatrix();
            glRotatef( frame_no, 1.0, 0.0, 0.0 );
            glMaterialfv( GL_FRONT, GL_DIFFUSE, cube_diffuse );
		  GLUquadric *quadric = gluNewQuadric();
		  gluCylinder(quadric, 0.5, 0.5, 1, 5, 500);
            //glutSolidCube( 1.5 );
		  //glutSolidTeapot(1.5);
	glPopMatrix();

	glBegin(GL_QUADS);
		glVertex3f(-100, 0, -100);
		glVertex3f(-100, 0, 100);
		glVertex3f(100, 0, 100);
		glVertex3f(100, 0, -100);
	glEnd();
}

void display()
{
	static int frame_no = 0;

	if (frame_no==360) frame_no=0;
	frame_no++;


	//czyszczenie bufora koloru i bufora g³êbokoœci, by namalowaæ nowe
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset transformations
	glLoadIdentity();
	

	gluLookAt(cameraX, cameraY, cameraZ,
			cameraX+lookX, cameraY+lookY, cameraZ+lookZ,
			upX, upY,  upZ);




	

	//w³asna funckja wyœwietlaj¹ca scenê
	displayObjects(frame_no);

	//opró¿nienie buforowanych zadañ openGL - zrób wsystko co masz do zrobienia
	glFlush();

	//prze³¹czanie buforów
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	//"blokowanie" zmiany rozmiaru okna
	/*glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);*/

	//ustawianie vieport i perspektywy widoku wzglêdem rozmiaru okna
	float ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
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

/*
funkcja wywo³ywana w pêtli, gdy nie ma nic innego do zrobienia
tutaj znajduje siê obliczanie stanu ca³ej sceny i jej rysowanie
*/
void idleFunc()
{
	static int frameTime = glutGet(GLUT_ELAPSED_TIME);

	//todo wywo³anie funkcja do obliczeñ sceny i wszystkiego

	nextFrameWait(&frameTime);
	glutPostRedisplay();
}

/*
funkcja wywo³ywana po naciœniêciu przycisku klawiatury
s³u¿y do obracania kamery i manipulowania wszelkimi parametrami symulacji
*/
void KeyPressedFunc(unsigned char key, int kx, int ky)
{
		/*glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		glOrtho(-2.25,2.25,-2.25,2.25,-10.0, 10.0 );
     glMatrixMode( GL_MODELVIEW );*/

		/*x = r * sin(phi) * cos(theta)
		y = r * sin(phi) * sin(theta)
		z = r * cos(phi)*/

	switch (key)
	{
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
	}

	/*ograniczenie zmiany k¹tów widzenia*/
	if (verticalAngle < -M_PI/2) verticalAngle = -M_PI/2;
	else if (verticalAngle > M_PI/2) verticalAngle = M_PI/2;

	if (horizontalAngle >= 2*M_PI) horizontalAngle = 0;
	else if (horizontalAngle <= 0) horizontalAngle = 2*M_PI;

	/*obliczenia wektorwa look*/
	lookZ = cos(verticalAngle) * sin(horizontalAngle);
	lookX = cos(verticalAngle) * cos(horizontalAngle);
	lookY = sin(verticalAngle);

	/*obliczanie wektora up*/
	verticalAngle += M_PI/2;
	upZ = cos(verticalAngle) * sin(horizontalAngle);
	upX = cos(verticalAngle) * cos(horizontalAngle);
	upY = sin(verticalAngle);
	verticalAngle -= M_PI/2;

	/*wypisywanie logów do konsoli*/
	std::cout.precision(2);
	std::cout<<"camera: "<<cameraX<<", "<<cameraY<<", "<<cameraZ<<"\t look: "<<lookX<<", "<<lookY<<", "<<lookZ<<"\n";
	std::cout<<"up: "<<upX<<", "<<upY<<", "<<upZ<<"\n";
	std::cout<<"V: "<<verticalAngle<<"\tH:"<<horizontalAngle<<"\n";
	std::cout<<lookX*lookX+lookY*lookY+lookZ*lookZ<<"\n";
	
}

int main(int argc, char** argv)
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitWindowPosition( 100, 100 );
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

	std::cout<<FRAME_RATE<<" "<<REFRESH_TIME;

	glutMainLoop();

	return 0;
}