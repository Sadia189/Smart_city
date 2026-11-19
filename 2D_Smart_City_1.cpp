#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>

// =====================================================
// 2D INTERACTIVE SMART CITY SIMULATION
// Modern Buildings + Balconies + Hanging Clothes
// C++ + OpenGL + FreeGLUT
// =====================================================

const int WIDTH = 1200;
const int HEIGHT = 700;

bool isNight = false;
bool paused = false;
bool showInfo = true;

bool birdsOn = true;
bool peopleOn = true;
bool rainOn = false;


float birdX = 0;
float personX = 300;
float cloudX = 0;
float rainOffset = 0;
float signalTimer = 0;

int trafficState = 0;

// ---------------- BASIC DRAWING ----------------

void color(float r, float g, float b) {
    glColor3f(r, g, b);
}

void rect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
    glEnd();
}

void line(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void circle(float x, float y, float r, int n=30) {
    glBegin(GL_POLYGON);

    for(int i=0; i<n; i++) {
        float a = 2.0f * 3.14159265f * i / n;
        glVertex2f(x+r*cos(a), y+r*sin(a));
    }

    glEnd();
}

void text(float x, float y, const char* str) {
    glRasterPos2f(x,y);

    for(const char* p=str; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*p);
}

// ---------------- SKY ----------------

void drawSky() {

    if(isNight)
        color(0.025f,0.04f,0.15f);
    else
        color(0.40f,0.78f,0.96f);

    rect(0,250,WIDTH,450);

    if(isNight) {

        color(1,1,0.75f);
        circle(1050,615,28);

        color(0.025f,0.04f,0.15f);
        circle(1063,625,25);

        color(1,1,1);

        for(int i=0;i<35;i++) {
            float x=(i*137+37)%WIDTH;
            float y=360+(i*71)%300;
            circle(x,y,1.5f,8);
        }
    }
    else {
        color(1,0.85f,0.12f);
        circle(1050,610,32);
    }
}

// ---------------- CLOUDS ----------------

void drawCloud(float x,float y) {

    color(0.94f,0.97f,1);

    circle(x,y,20);
    circle(x+24,y+10,25);
    circle(x+50,y,20);

    rect(x-1,y-15,53,20);
}

void drawClouds() {

    drawCloud(fmod(cloudX+100,1350)-80,590);
    drawCloud(fmod(cloudX+530,1400)-80,530);
    drawCloud(fmod(cloudX+900,1450)-80,630);
}

// =====================================================
// MODERN BUILDING COMPONENTS
// =====================================================

// ---------------- BALCONY PERSON ----------------

void drawBalconyPerson(float x,float y) {

    // Head
    color(0.95f,0.72f,0.52f);
    circle(x,y+13,4);

    // Body
    color(0.15f,0.25f,0.75f);
    rect(x-4,y+4,8,8);

    // Hands
    color(0.95f,0.72f,0.52f);

    line(x-3,y+9,x-7,y+13);
    line(x+3,y+9,x+7,y+13);
}

// ---------------- HANGING CLOTHES ----------------

void drawClothes(float x,float y) {

    // Clothes line
    color(0.12f,0.12f,0.12f);
    line(x,y,x+48,y);

    // Red Shirt
    color(0.95f,0.12f,0.15f);

    glBegin(GL_POLYGON);

    glVertex2f(x+3,y);
    glVertex2f(x+8,y-3);
    glVertex2f(x+13,y);
    glVertex2f(x+17,y-5);
    glVertex2f(x+14,y-19);
    glVertex2f(x+5,y-19);

    glEnd();

    // Blue Shirt
    color(0.10f,0.45f,0.95f);

    glBegin(GL_POLYGON);

    glVertex2f(x+21,y);
    glVertex2f(x+26,y-3);
    glVertex2f(x+31,y);
    glVertex2f(x+35,y-5);
    glVertex2f(x+32,y-19);
    glVertex2f(x+23,y-19);

    glEnd();

    // Yellow Towel
    color(1.0f,0.80f,0.12f);
    rect(x+38,y-19,8,19);

    // White details
    color(1,1,1);
    line(x+8,y-8,x+14,y-8);
}

// ---------------- MODERN BALCONY ----------------

void drawModernBalcony(float x,float y,float w) {

    // Balcony floor
    color(0.22f,0.24f,0.28f);
    rect(x-3,y,w+6,5);

    // Glass railing
    if(isNight)
        color(0.35f,0.50f,0.68f);
    else
        color(0.45f,0.80f,0.95f);

    rect(x,y+5,w,13);

    // Railing borders
    color(0.85f,0.90f,0.95f);

    rect(x,y+17,w,2);
    rect(x,y+5,2,14);
    rect(x+w-2,y+5,2,14);

    // Vertical railing
    for(float bx=x+12;bx<x+w;bx+=15)
        rect(bx,y+5,1.5f,12);
}

// ---------------- ROOFTOP ----------------

void drawRoof(float x,float y,float w) {

    color(0.20f,0.24f,0.30f);
    rect(x-5,y,w+10,9);

    // Rooftop boundary
    color(0.70f,0.75f,0.80f);
    rect(x,y+9,w,3);

    // Small rooftop garden
    color(0.08f,0.48f,0.16f);

    circle(x+15,y+17,7);
    circle(x+35,y+17,7);

    color(0.38f,0.23f,0.12f);
    rect(x+14,y+5,3,10);
    rect(x+34,y+5,3,10);
}

// ---------------- MODERN BUILDING ----------------

void drawModernBuilding(float x, float y, float w,
                        float h, int type) {

    // Building colors
    if(type == 0)
        color(0.83f, 0.85f, 0.89f);
    else if(type == 1)
        color(0.66f, 0.76f, 0.86f);
    else if(type == 2)
        color(0.91f, 0.83f, 0.70f);
    else
        color(0.72f, 0.69f, 0.82f);

    // Main building
    rect(x, y, w, h);

    // Side shadow
    color(0.40f, 0.45f, 0.53f);
    rect(x + w - 9, y, 9, h);

    // Floor divisions
    color(0.45f, 0.50f, 0.57f);

    for(float fy = y + 40; fy < y + h; fy += 42) {
        rect(x, fy, w - 9, 2);
    }

    // Modern glass windows
    for(float wx = x + 10; wx < x + w - 22; wx += 25) {

        for(float wy = y + 12; wy < y + h - 12; wy += 30) {

            // Window frame
            color(0.20f, 0.28f, 0.38f);
            rect(wx - 2, wy - 2, 19, 23);

            // Window glass
            if(isNight)
                color(1.0f, 0.80f, 0.32f);
            else
                color(0.30f, 0.72f, 0.92f);

            rect(wx, wy, 15, 19);

            // Glass reflection
            color(0.85f, 0.95f, 1.0f);
            line(wx + 3, wy + 3, wx + 3, wy + 15);
        }
    }

    // Main entrance
    color(0.20f, 0.24f, 0.30f);
    rect(x + w * 0.40f, y, w * 0.22f, 30);

    // Entrance glass
    color(0.40f, 0.78f, 0.90f);
    rect(x + w * 0.43f, y + 4, w * 0.16f, 22);

    // Entrance handle
    color(1.0f, 0.85f, 0.25f);
    circle(x + w * 0.56f, y + 15, 2, 10);

    // Modern rooftop
    color(0.20f, 0.24f, 0.30f);
    rect(x - 5, y + h, w + 10, 9);

    // Rooftop glass design
    color(0.35f, 0.55f, 0.65f);
    rect(x + 10, y + h + 9, w - 20, 8);

    // Rooftop garden
    color(0.08f, 0.48f, 0.16f);

    circle(x + 18, y + h + 20, 6);
    circle(x + w - 20, y + h + 20, 6);
}
// ---------------- CITY ----------------

void drawCity() {

    drawModernBuilding(35,250,105,210,1);
    drawModernBuilding(165,250,100,155,2);
    drawModernBuilding(290,250,120,260,0);
    drawModernBuilding(435,250,100,185,3);
    drawModernBuilding(560,250,115,225,1);
    drawModernBuilding(700,250,100,165,2);
    drawModernBuilding(825,250,125,270,0);
    drawModernBuilding(975,250,90,195,3);
    drawModernBuilding(1080,250,90,230,1);

    // Solar panels
    color(0.08f,0.15f,0.32f);

    rect(60,464,42,13);
    rect(850,524,45,13);

    color(0.4f,0.8f,1);

    line(80,464,80,477);
    line(60,470,102,470);
}

// ---------------- GROUND AND ROAD ----------------

void drawGround() {

    // Green Ground
    color(0.35f,0.72f,0.30f);
    rect(0,190,WIDTH,60);

    // Road
    color(0.22f,0.23f,0.26f);
    rect(0,75,WIDTH,100);

    // Road markings
    color(1,1,1);

    for(int x=0;x<WIDTH;x+=120)
        rect(x,122,65,5);

    // Sidewalk
    color(0.70f,0.70f,0.70f);
    rect(0,175,WIDTH,15);

    // River
    color(0.05f,0.45f,0.83f);
    rect(0,0,WIDTH,28);

    color(0.45f,0.85f,1);

    for(int x=10;x<WIDTH;x+=90)
        line(x,12,x+35,12);
}

// ---------------- TREES AND PARK ----------------

void drawTree(float x,float y) {

    color(0.38f,0.22f,0.10f);
    rect(x-5,y,10,30);

    color(0.05f,0.48f,0.16f);

    circle(x,y+39,22);
    circle(x-13,y+30,15);
    circle(x+13,y+30,15);

    color(0.95f,0.30f,0.48f);

    circle(x-7,y+42,3,10);
    circle(x+9,y+35,3,10);
}

void drawPark() {

    color(0.16f,0.62f,0.22f);
    rect(0,190,250,60);

    drawTree(40,198);
    drawTree(105,198);
    drawTree(180,198);

    color(0.96f,0.82f,0.16f);

    circle(70,215,4,10);
    circle(145,220,4,10);
}


// ---------------- PEOPLE AND BIRDS ----------------

void drawPerson(float x,float y) {

    color(0.95f,0.72f,0.52f);
    circle(x,y+25,7);

    color(0.12f,0.25f,0.85f);
    rect(x-6,y+7,12,14);

    color(0.10f,0.10f,0.10f);

    line(x-3,y+7,x-7,y);
    line(x+3,y+7,x+7,y);

    line(x-5,y+17,x-12,y+11);
    line(x+5,y+17,x+12,y+12);
}

void drawBird(float x,float y) {

    color(0.05f,0.05f,0.05f);

    line(x-10,y,x,y+7);
    line(x,y+7,x+10,y);
}

// ---------------- TRAFFIC LIGHT ----------------

void drawTrafficLight() {

    color(0.15f,0.15f,0.15f);

    rect(920,175,8,85);
    rect(908,255,32,70);

    color(trafficState==0?1:0.25f,0,0);
    circle(924,310,9);

    color(trafficState==1?1:0.25f,
          trafficState==1?0.8f:0.25f,0);

    circle(924,290,9);

    color(0,trafficState==2?1:0.25f,0);
    circle(924,270,9);
}

// ---------------- RAIN ----------------

void drawRain() {

    if(!rainOn) return;

    color(0.65f,0.83f,1);

    glBegin(GL_LINES);

    for(int i=0;i<160;i++) {

        float x=(i*83)%WIDTH;
        float y=fmod(i*47+rainOffset,HEIGHT);

        glVertex2f(x,y);
        glVertex2f(x-5,y-12);
    }

    glEnd();
}

// ---------------- INFORMATION PANEL ----------------



// ---------------- DISPLAY ----------------

void display() {

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();

    if(!isNight)
        drawClouds();

    drawCity();
    drawPark();
    drawGround();
    drawTrafficLight();

    // Walking Humans
    if(peopleOn) {
        drawPerson(personX,190);
        drawPerson(personX+60,190);
    }

    // Birds Animation
    if(birdsOn) {
        drawBird(birdX,555);
        drawBird(birdX+35,570);
        drawBird(birdX+70,550);
    }

    // Rain Animation
    drawRain();

    glutSwapBuffers();
}
// ---------------- ANIMATION UPDATE ----------------

void update(int value) {

    if(!paused) {

        // Bird Animation
        if(birdsOn) {

            birdX+=3;

            if(birdX>WIDTH+50)
                birdX=-100;
        }

        // Human Animation
        if(peopleOn) {

            personX+=1.5f;

            if(personX>WIDTH+50)
                personX=-100;
        }

        // Cloud Animation
        cloudX+=0.5f;

        // Rain Animation
        rainOffset+=15;

        // Traffic Signal Animation
        signalTimer+=0.03f;

        if(signalTimer>=3.0f) {

            trafficState=(trafficState+1)%3;
            signalTimer=0;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(30,update,0);
}
// ---------------- KEYBOARD CONTROLS ----------------
void keyboard(unsigned char key,int x,int y) {

    switch(key) {

        case '1':
            birdsOn=!birdsOn;
            break;

        case '2':
            peopleOn=!peopleOn;
            break;

        case 'n':
        case 'N':
            isNight=true;
            break;

        case 'd':
        case 'D':
            isNight=false;
            break;

        case 'p':
        case 'P':
            paused=!paused;
            break;

        case 'h':
        case 'H':
            showInfo=!showInfo;
            break;

        case 'w':
        case 'W':
            rainOn=!rainOn;
            break;

        case 'r':
        case 'R':

            birdX=0;
            personX=300;
            cloudX=0;
            rainOffset=0;
            signalTimer=0;
            trafficState=0;

            paused=false;

            break;

        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}
// ---------------- RESHAPE ----------------

void reshape(int w,int h) {

    if(h==0)
        h=1;

    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0,WIDTH,0,HEIGHT);

    glMatrixMode(GL_MODELVIEW);
}

// ---------------- MAIN ----------------

int main(int argc,char** argv) {

    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);

    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(50,30);

    glutCreateWindow("2D Smart City Simulation");

    glClearColor(0.4f,0.78f,0.96f,1);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutTimerFunc(30,update,0);

    glutMainLoop();

    return 0;
}
