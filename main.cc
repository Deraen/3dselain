#include <iostream>
#include <cmath>

// Jotain jänniä juttuja varten, mm. glGenBuffers
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "alusta.h"

#include "camera.hh"

const int DEF_WINDOW_WIDTH = 800;
const int DEF_WINDOW_HEIGHT = 600;

// Globaaleja:
Camera camera(0.0, 10.0, 0.0);

bool keys[256];

GLfloat groundMaterial[] = {0.2, 1.0, 0.2}; //set the material to red
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white
GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0}; //set the material to green
GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0}; //set the light specular to white
GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0}; //set the light ambient to black
GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0}; //set the diffuse light to white
GLfloat blankMaterial[] = {0.0, 0.0, 0.0}; //set the diffuse light to white
GLfloat mShininess[] = {128}; //set the shininess of the material

GLfloat red[] = {1.0, 0.0, 0.0};
GLfloat green[] = {0.0, 1.0, 0.0};
GLfloat blue[] = {0.0, 0.0, 1.0};

GLuint vbo;
GLuint vinx;

void light (float time) {
    float light0Pos[] = { sin(time)*10.0, 25.0, cos(time)*10.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01); // Valo heikkenee hieman etäisyyden funktiona
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
}

void InitMesh() {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof (struct vertex_struct) * VERTEX_COUNT, vertexs, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vinx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vinx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indexes[0]) * FACES_COUNT * 3, indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#define BUFFER_OFFSET(x)((char *)NULL+(x))

void DrawMesh(GLuint _vbo, GLuint _inx) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMaterial);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _inx);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(0));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(3 * sizeof (float)));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof (struct vertex_struct), BUFFER_OFFSET(6 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, FACES_COUNT * 3, INX_TYPE, BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void HandleKeys() {
    if (keys['q']) exit(0);

    if (keys['i']) camera.move(-0.1);
    if (keys['k']) camera.move(0.1);
    if (keys['j']) camera.strafe(-0.1);
    if (keys['l']) camera.strafe(0.1);
    if (keys['y']) camera.moveHeight(0.1);
    if (keys['h']) camera.moveHeight(-0.1);

    if (keys['w']) camera.pitch(-1);
    if (keys['s']) camera.pitch(1);
    if (keys['a']) camera.heading(1);
    if (keys['d']) camera.heading(-1);
}

//
// Piirtofunktio
//
void display(void) {
    static float time = 0.0;
    time += 0.01;

// Tyhjennetään ruutu ja Z-puskuri
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.set();

    light(time);

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe
    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    DrawMesh(vbo, vinx);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, red);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
    glPushMatrix();
    glTranslatef(20.0, 0.0, 0.0);
    glutSolidCube(5);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, green);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
    glPushMatrix();
    glTranslatef(0.0, 20.0, 0.0);
    glutSolidCube(5);
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blue);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 20.0);
    glutSolidCube(5);
    glPopMatrix();

    // Vaihdetaan piirtopuskuri ja näkyvä puskuri
    glutSwapBuffers();
}

void motion(int x, int y) {
}

void mouse(int button, int state, int x, int y) {
// Hiiren näppäintä painettu
// button: GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON ...
// state: GLUT_DOWN, GLUT_UP
}


//
// Jos ikkunan kokoa muutetaan:
//
void resize(int newWidth, int newHeight) {
    // Asetetaan piirtoalue uudelleen, ja päivitetään leveys/korkeus; display-funktiossa
    //  kutsutaan gluPerspective-funktiota näistä lasketulla aspektisuhteella
    glViewport(0, 0, newWidth, newHeight);
    camera.setDisplayDim(newWidth, newHeight);
}


//
// GLUT kutsuu tätä aina kun sillä ei ole muuta tekemistä;
// pyydetään sitä kutsumaan piirtofunktioa
//
void animate(void) {
    HandleKeys();
    glutPostRedisplay();
}


//
// OpenGL:ään liittyvä alustus
//
void init(void) {
    glClearColor(0.0, 0.0, 0.2, 0.0); // Ruudun tyhjennysväri
    glEnable(GL_DEPTH_TEST);  // Z-testi

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);  // Sävytys: GL_FLAT / GL_SMOOTH

    InitMesh();
}



void HandleKey(unsigned char key, int, int) {
    keys[key] = true;
}

void HandleKeyRelease(unsigned char key, int, int) {
    keys[key] = false;
}

int main(int argc, char *argv[]) {
    // Alustetan GLUT ja luodaan ikkuna
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT);
    glutCreateWindow("TrollGL");

    // GL-alustukset
    init();

    glutIgnoreKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutKeyboardFunc(HandleKey);
    glutKeyboardUpFunc(HandleKeyRelease);

    // Asetetaan callback-funktiot
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(resize);
    glutIdleFunc(animate);
    glutMainLoop();
    return 0;
}

