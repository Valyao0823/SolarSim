/*
	3GC3 Final Project - Solar System Simulator
	|Rob Gorrie (#:1222547)
	|MD Rashad	(#:1212911)
	|Yiyu Yao 	(#:1074092)

	December 20th, 2014
*/
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glui.h>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/freeglut.h>
#endif

#include <vector>
#include <time.h>
#include <math.h>
#include "imageloader.h" 

using namespace std;

//constants go here -> #define whatever
//3D Display Window
#define WIN_HEIGHT		800
#define WIN_WIDTH			600
//2D UI Window
#define WIN2_HEIGHT		150
#define WIN2_WIDTH		100
//Other constants
#define MaxNumPlanet 	20
#define MinNumPlanet 	1
#define MaxRad 				20.0
#define MinRad 				5.0
#define NEW_PLANET 		100
#define EDIT_PLANET		101
#define DELETE_PLANET	102
#define UPDATE_PLANET	103

float RandomFloat(float a, float b);
float camPos[] = {500, 500, 500};
GLUI *glui;
GLUI_Button *newplan, *editplan, *delplan, *update;
GLUI_EditText *radtext, *dtext, *stext;
GLUI_Spinner *texturespinner, *selectspinner;
int selected_texture = 0;
int selected_planet = 0;
string selected_rad = "";
string selected_d = "";
string selected_fps = "";
string::size_type sz;
int counter = 0;
int timer = 0;
int window;
bool queuePlanet = false;
bool queueEdit = false;
bool queueDelete = false;
int choose;
float cols[10][4] = { {1,1,1,1}, {0.6,0.6,0.6,1}, {0.7,0.4,0.4,1}, {0.1,1,0.1,1}, {0,0,1,1}, {1,0,1,1}, {1,0.5,0.5,1}, {0.5,1,0.5,1}, {0.5,0.5,1,1}, {0.4,0.7,0.4,1} };
float lightPos[] = {0,0,0,0}; //position for light0
float diff[] = {0.7,0.7,0.5,1}; //light0 diffuse
float amb[] = {0.5,0.5,0.4,1}; //light0 ambient
float spec[] = {1,1,1,1}; //ling0 specular
float shine = 27;
string ss;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class pclass{
	public:
	float d; //distance from center to planet
	float radius; //planet radius
	int choose; //planet texture index
	int frameps; //planet speed
	int ang;
	int cnt;
	pclass(); //constructor
	void newD(float nd);
	void newRad(float nrad);
	void newFps(float nfps);
	void newChoose(int tex);

	private:
};
vector<pclass> planets(5);
//planet class constructor
pclass::pclass(){
	d = RandomFloat(20,300);
	radius = RandomFloat(MinRad, MaxRad);
	choose = rand() % 10;
	frameps = rand() % 10 + 5;
}

void pclass::newD(float nd){
	d = nd;
}
void pclass::newRad(float nrad){
	radius = nrad;
}

void pclass::newFps(float nfps){
	frameps = nfps;
}

void pclass::newChoose(int tex){
	choose = tex;
}
//______________________________MAIN PROGRAM___________________________________

void key(unsigned char key, int x, int y){
	switch (key)
	{
		case 'q':
		case 27: //27 is the esc key
			exit(0);
		break;
	}
}

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}
GLuint texturearray[9]; //The id of the textur
GLUquadric *quad;
void initRendering() {
	glEnable(GL_COLOR_MATERIAL);
	quad = gluNewQuadric();

	Image* image0 = loadBMP("earthmap1k.bmp");
	texturearray[0] = loadTexture(image0);	//earthmap
	Image* image1 = loadBMP("jupitermap.bmp");
	texturearray[1] = loadTexture(image1);	//jupiter
	Image* image2 = loadBMP("marsmap1k.bmp");
	texturearray[2] = loadTexture(image2);	//mars
	Image* image3 = loadBMP("mercurymap.bmp");
	texturearray[3] = loadTexture(image3);	//mercury
	Image* image4 = loadBMP("moonmap1k.bmp");
	texturearray[4] = loadTexture(image4);	//moon
	Image* image5 = loadBMP("neptunemap.bmp");
	texturearray[5] = loadTexture(image5);	//neptune
	Image* image6 = loadBMP("plutomap1k.bmp");
	texturearray[6] = loadTexture(image6);	//pluto
	Image* image7 = loadBMP("saturnmap.bmp");
	texturearray[7] = loadTexture(image7);	//saturn
	Image* image8 = loadBMP("uranusmap.bmp");
	texturearray[8] = loadTexture(image8);	//uranus
	Image* image9 = loadBMP("venusmap.bmp");
	texturearray[9] = loadTexture(image9);	//venus
	delete image0;
	delete image1;
	delete image2;
	delete image3;
	delete image4;
	delete image5;
	delete image6;
	delete image7;
	delete image8;
	delete image9;
}

void drawPlanet(pclass p){
// load pclass property
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
		gluSphere(quad,p.radius,32,32);
	glEnd();
}

float RandomFloat(float a, float b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void display(void)
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0); //position camera, look at origin
	for (int i = 0; i < planets.size(); i++){
		planets[i].cnt ++;
		if (planets[i].cnt % planets[i].frameps == 0)
		{
			planets[i].ang ++;
			planets[i].cnt = 0;
		}


		glBindTexture(GL_TEXTURE_2D, texturearray[planets[i].choose]);
		

		//Bottom
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cols[planets[i].choose]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cols[planets[i].choose]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

		glPushMatrix();
		glRotatef(planets[i].ang,0,1,0);
		glTranslatef(planets[i].d, 0, 0);
		gluQuadricTexture(quad,1);
		drawPlanet(planets[i]);
		glPopMatrix();
	}

	glutSwapBuffers();
}

////////GLUI CALLBACK FUNCTIONS///////////////
void idle2(void){
	//glui idle function
	if (glutGetWindow() != window) glutSetWindow(window);
	if(queuePlanet==true){
		selectspinner->disable();
		texturespinner->enable();
		radtext->enable();
		dtext->enable();
		stext->enable();
	}else if(queueEdit==true){
		selectspinner->enable();
		texturespinner->enable();
		radtext->enable();
		dtext->enable();
		stext->enable();
		ss = to_string(planets[selected_planet].radius);
		selected_rad = ss;
		ss = to_string(planets[selected_planet].d);
		selected_d = ss;
		ss = to_string(planets[selected_planet].frameps);
		selected_fps = ss;
	}else if(queueDelete==true){
		selectspinner->enable();
		texturespinner->disable();
		radtext->disable();
		dtext->disable();
		stext->disable();
	}

  glutPostRedisplay();

  counter++;
  glui->sync_live();
}

void pointer(GLUI_Control* control){
	//when the described button is pressed
	if(control->get_id() == NEW_PLANET){
		queueEdit = false;
		queueDelete = false;
		queuePlanet = true;
	}else if(control->get_id() == EDIT_PLANET){
		queuePlanet = false;
		queueDelete = false;
		queueEdit = true;
	}else if(control->get_id() == DELETE_PLANET){
		queuePlanet = false;
		queueEdit = false;
		queueDelete = true;
	}else if(control->get_id() == UPDATE_PLANET){
		if(queuePlanet){
			//push new planet onto vector
			pclass p;
			p.newD(stof(selected_d,&sz));
			p.newRad(stof(selected_rad,&sz));
			p.newFps(stof(selected_fps,&sz));
			p.newChoose(selected_texture);
			planets.push_back(p);
			selectspinner->set_int_limits(1, planets.size());
		}else if(queueEdit){
			//update the planet under question
			planets[selected_planet-1].newD(stof(selected_d,&sz));
			planets[selected_planet-1].newRad(stof(selected_rad,&sz));
			planets[selected_planet-1].newFps(stof(selected_fps,&sz));
			planets[selected_planet-1].newChoose(selected_texture);
		}else if(queueDelete){
			//erase planet from vector at index selected_planet-1
			planets.erase(planets.begin()+(selected_planet-1));
			selectspinner->set_int_limits(1, planets.size());
		}
	}
}
//////////////////////////////////////////////


void init(){
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE); // backface culling by default
	
	
	glClearColor(0, 0, 0,0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 2000);
}

int main(int argc, char** argv){
	//initializing functions
	srand((time(0)));
	glutInit(&argc, argv);

	//create the window
	glutInitWindowSize(WIN_HEIGHT, WIN_WIDTH);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	window = glutCreateWindow("SolarSim 3D");	//creates the window
	//-=-=-=-=-=-=-=-=-=-=-=-=-=-3D DISPLAY WINDOW
	glutSetWindow(window);
	init();
	initRendering();
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	//-=-=-=-=-=-=-=-=-=-=-=-=-=-UI WINDOW
	//___GLUI____
	glui = GLUI_Master.create_glui("SolarSim Menu");
	newplan = new GLUI_Button(glui, "New Planet", NEW_PLANET, pointer);
	editplan = new GLUI_Button(glui, "Edit Planet", EDIT_PLANET, pointer);
	delplan = new GLUI_Button(glui, "Delete Planet", DELETE_PLANET, pointer);
	selectspinner = new GLUI_Spinner(glui, "Planet #:", &selected_planet);
  selectspinner->set_int_limits(1, planets.size());
	selectspinner->disable();
	texturespinner = new GLUI_Spinner(glui, "Texture:", &selected_texture);
  texturespinner->set_int_limits(1, 10);
	texturespinner->disable();
	radtext = new GLUI_EditText(glui, "Radius:", selected_rad);
  radtext->set_w(150);
	radtext->disable();
	dtext = new GLUI_EditText(glui, "Distance:", selected_d);
  dtext->set_w(150);
	dtext->disable();
	stext = new GLUI_EditText(glui, "Speed(1/fps):", selected_fps);
  stext->set_w(150);
	stext->disable();
	update = new GLUI_Button(glui, "Update", UPDATE_PLANET, pointer);
	new GLUI_Button(glui, "Quit", 0,(GLUI_Update_CB)exit);
	glui->set_main_gfx_window(window);
	GLUI_Master.set_glutIdleFunc(idle2); 	


	glutMainLoop(); //run glut loop

	return EXIT_SUCCESS;
}
