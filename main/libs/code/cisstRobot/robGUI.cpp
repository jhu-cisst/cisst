#include <cisstRobot/robGUI.h>
#include <FL/glu.h>
#include <FL/glut.h>

#include <iostream>

using namespace cisstRobot;
using namespace std;

robGUI* robGUI::gui = NULL;

void KeyboardFunc(unsigned char c, int x, int y){
  robGUI::gui->Keyboard( (int)c, x, y );
}

void SpecialFunc(int c, int x, int y){
  robGUI::gui->Keyboard( c, x, y );
}

void robGUI::Keyboard(int k, int x, int y){
  //ESC key (27) quits
  if(k == 27){ exit(0); }

  //left arrow
  if(k == 65361){
    azimuth -= 0.005;
    if(azimuth <0) {azimuth += 2*M_PI;}
  }

  //up arrow
  if(k == 65362) {if(elevation <= M_PI/2)  {elevation +=0.005;}}

  //right arrow
  if(k == 65363){
    azimuth += 0.005;
    if(azimuth > 2*M_PI) {azimuth -= 2*M_PI;}
  }

  //down arrow
  if(k == 65364) {if(elevation >= -M_PI/2) {elevation -=0.005;}}

  if(k == 122) { distance *= 0.995; }
  if(k == 120) { distance *= 1.005; }

  glutPostRedisplay();
}

R3 robGUI::CameraPosition() const {
  return R3( distance*cos(azimuth)*cos(elevation),
	     distance*sin(azimuth)*cos(elevation),
	     distance*sin(elevation) );
}

void robGUI::DrawGrid(Real width, int subdivision){
  glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_LINES);

  for(int i = 0; i<=subdivision; i++){
    float stepsize = width/((float) (subdivision));

    glVertex3f(-width/2 + i*stepsize,width/2,0);
    glVertex3f(-width/2 + i*stepsize,-width/2,0);

    glVertex3f(width/2,-width/2 + i*stepsize,0);
    glVertex3f(-width/2,-width/2 + i*stepsize,0);

  }

  glEnd();
}

void robGUI::DrawXYZ(){

  glLineWidth(2);
  glBegin(GL_LINES);
  glColor3f(1.0,0.0,0.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(1.0,0.0,0.0);
  glColor3f(0.0,1.0,0.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,1.0,0.0);
  glColor3f(0.0,0.0,1.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,0.0,1.0);
  glEnd();
  glLineWidth(1);

}

void robGUI::draw(){
  if (!valid()) {
    valid(1);

    //keyboard event callbacks
    keyboard = KeyboardFunc;
    special = SpecialFunc;

    // (REFERENCE: SGI 'light.c' EXAMPLE)
    GLfloat mat_ambient[]    = { 1.0, 1.0, 1.0, 1.0 };  // RGBA
    GLfloat mat_diffuse[]    = { 1.0, 1.0, 1.0, 1.0 };  // RGBA
    GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };  // RGBA
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialf(GL_FRONT,  GL_SHININESS, 20.0);


    GLfloat lightAmbient[]   = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat lightDiffuse[]   = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightSpecular[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightZero[]      = { 0.1, 0.1, 0.1, 0.5 };
    GLfloat lightPosition0[] = {  30.0,  30.0,  30.0, 1.0 };
    GLfloat lightPosition1[] = { -30.0, -30.0, -30.0, 1.0 };
    glClearColor(0.0, 0.0, 0.0, 0.0);                   // bg color
    glShadeModel(GL_SMOOTH);

    // initialise lighting state
    // set light properties
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lightZero);   // global ambient light
    glLightfv( GL_LIGHT0, GL_AMBIENT,  lightAmbient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  lightDiffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpecular );
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition0 );
    
    glLightfv( GL_LIGHT1, GL_AMBIENT,  lightAmbient);
    glLightfv( GL_LIGHT1, GL_DIFFUSE,  lightDiffuse);
    glLightfv( GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv( GL_LIGHT1, GL_POSITION, lightPosition1 );

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

  }

  //clear screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Set up Projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  GLdouble aspect = ( (GLdouble) w() / (GLdouble) h() );
  gluPerspective( 60, aspect, 0.001, 10);

  //Set up Modelview matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  //set camera location and bearing
  R3 xyz = CameraPosition();

  gluLookAt(xyz[0], xyz[1], xyz[2], 0, 0, 0, 0, 0, 1);

  DrawGrid(10,10);
  DrawXYZ();

  for(size_t i=0; i<bodies.size(); i++ )
    bodies[i]->Draw();

  //glPopMatrix();

}

void robGUI::Insert( const robBody* body ) { bodies.push_back( body ); }
void robGUI::Insert( const robManipulator* manipulator ) { 
  vector<robLink>::const_iterator it = manipulator->Links().begin();

  for(; it!=manipulator->Links().end(); it++ ){
    bodies.push_back( &(*it) ); 
  }
}

robGUI::robGUI( int X, int Y, int W, int H, const char*L) 
 : Fl_Glut_Window( X, Y, W, H, L ){

  robGUI::gui = this;

  this->x = X;
  this->x = Y;
  this->width = W;
  this->height = H;

  azimuth = 0;
  elevation = 3.14/4.0;
  distance = 3;
  perspective = 60;

}
