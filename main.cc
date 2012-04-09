#include <iostream>

// Jotain jänniä juttuja varten, mm. glGenBuffers - täytyy includea tässä vaikkei
// tässä tiedosossa käytetä koska GL/gl.h sisältää ifndef macron
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>

#include "engine.hh"

const int DEF_WINDOW_WIDTH = 800;
const int DEF_WINDOW_HEIGHT = 600;

int main(int argc, char *argv[]) {
    // Alustetan GLUT ja luodaan ikkuna
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT);
    glutCreateWindow("TrollGL");

    glutIgnoreKeyRepeat(GLUT_KEY_REPEAT_ON);

    Engine::create();

    glutKeyboardFunc(Engine::HandleKey);
    glutKeyboardUpFunc(Engine::HandleKeyRelease);

    glutDisplayFunc(Engine::display);
    // glutMouseFunc(mouse);
    // glutMotionFunc(motion);
    glutReshapeFunc(Engine::resize);
    glutIdleFunc(Engine::animate);
    glutMainLoop();

    return 0;
}
