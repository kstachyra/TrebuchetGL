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

void displayObjects(int frame_no)
{
	GLfloat torus_diffuse[]  = { 0.7, 0.7, 0.0, 1.0 };
	GLfloat cube_diffuse[]   = { 0.0, 0.7, 0.7, 1.0 };
	GLfloat sphere_diffuse[] = { 0.7, 0.0, 0.7, 1.0 };
	GLfloat octa_diffuse[]   = { 0.7, 0.4, 0.4, 1.0 };

	glPushMatrix();

	glRotatef( 30.0, 1.0, 0.0, 0.0 );

          glPushMatrix();
             glTranslatef( -0.80, 0.35, 0.0 ); 
             glRotatef( 100.0, 1.0, 0.0, 0.0 );
             glMaterialfv( GL_FRONT, GL_DIFFUSE, torus_diffuse );
             glutSolidTorus( 0.275, 0.85, 10, 10 );
          glPopMatrix();

         glPushMatrix();
            glTranslatef( -0.75, -0.50, 0.0 ); 
            glRotatef( 45.0, 0.0, 0.0, 1.0 );
            glRotatef( frame_no, 1.0, 0.0, 0.0 );
            glMaterialfv( GL_FRONT, GL_DIFFUSE, cube_diffuse );
            glutSolidCube( 1.5 );
         glPopMatrix();

         glPushMatrix();
             glTranslatef( 0.75, 0.60, 0.0 ); 
             glRotatef( 30.0, 1.0, 0.0, 0.0 );
	     glMaterialfv( GL_FRONT, GL_DIFFUSE, sphere_diffuse );
             glutSolidSphere( 1.0, 10, 10 );
         glPopMatrix();

	    GLUquadric *quadric = gluNewQuadric();

         glPushMatrix();
		  glRotatef( 30.0, 1.0, 0.0, 0.0 );
            glTranslatef( 0.70, -0.90, 0.25 ); 
            //glMaterialfv( GL_FRONT, GL_DIFFUSE, octa_diffuse );
	        gluCylinder(quadric, 0.5, 0.5, 1, 5, 5);
         glPopMatrix();

   glPopMatrix();	 
}

void display()
{
	static int frame_no = 0;
	//czyszczenie bufora koloru i bufora g³êbokoœci, by namalowaæ nowe
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



	if (frame_no==360) frame_no=0;
	frame_no++;
	//w³asna funckja wyœwietlaj¹ca scenê
	displayObjects(frame_no);

	//opró¿nienie buforowanych zadañ openGL - zrób wsystko co masz do zrobienia
	glFlush();

	//prze³¹czanie buforów
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	/*if( h > 0 && w > 0 )
	{
		glViewport( 0, 0, w, h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		if( w <= h )
		{
			glOrtho( -2.25, 2.25, -2.25*h/w, 2.25*h/w, -10.0, 10.0 );
		}
		else
		{
			glOrtho( -2.25*w/h, 2.25*w/h, -2.25, 2.25, -10.0, 10.0 );
		}
     glMatrixMode( GL_MODELVIEW );
     }*/
}

/*
wywo³anie funkcji powoduje zawieszenie programu do momentu, a¿ od ostatniego wywo³ania tej funkcji minie REFRESH_TIME
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
	std::cout<<"keypressed";

		glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();




			glOrtho( -2.25*WINDOW_WIDTH/WINDOW_HEIGHT, 2.25*WINDOW_WIDTH/WINDOW_HEIGHT, -2.25, 2.25, -10.0, 10.0 );

     glMatrixMode( GL_MODELVIEW );
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