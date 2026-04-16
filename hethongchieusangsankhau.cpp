#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265358979323846f

// =====================================================
// BIEN TOAN CUC
// =====================================================
float trussY = 7.4f;
float trussDir = 1.0f;

float rotTop[3] = {-18.0f, 0.0f, 18.0f};
float rotFront[3] = {-18.0f, 0.0f, 18.0f};

bool runTop[3] = {true, true, true};
bool runFront[3] = {true, true, true};

bool redOn = true;
bool blueOn = true;
bool yellowOn = true;
bool animateOn = true;

// =====================================================
// HO TRO CO BAN
// =====================================================
void setMaterial(float r, float g, float b, float shininess = 64.0f)
{
    GLfloat ambient[] = {r * 0.38f, g * 0.38f, b * 0.38f, 1.0f};
    GLfloat diffuse[] = {r, g, b, 1.0f};
    GLfloat specular[] = {0.95f, 0.95f, 0.95f, 1.0f};
    GLfloat shine[] = {shininess};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}

void drawCube(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawCylinder(float radius, float height, int slices = 24)
{
    GLUquadric *q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);
    gluCylinder(q, radius, radius, height, slices, 8);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void drawCone(float baseRadius, float height, int slices = 28)
{
    glutSolidCone(baseRadius, height, slices, 10);
}

// =====================================================
// SAN KHAU
// =====================================================
void drawGridFloor(float size = 20.0f, float step = 1.0f)
{
    glDisable(GL_LIGHTING);
    glColor3f(0.30f, 0.30f, 0.38f);

    glBegin(GL_LINES);
    for (float i = -size; i <= size; i += step)
    {
        glVertex3f(i, 0.001f, -size);
        glVertex3f(i, 0.001f, size);

        glVertex3f(-size, 0.001f, i);
        glVertex3f(size, 0.001f, i);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void drawStage()
{
    // de san
    setMaterial(0.14f, 0.14f, 0.16f, 25.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 0.0f);
    drawCube(16.0f, 0.7f, 10.0f);
    glPopMatrix();

    // mat san sang hon va bat sang tot hon
    setMaterial(0.78f, 0.78f, 0.80f, 140.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    drawCube(15.0f, 0.04f, 9.0f);
    glPopMatrix();

    // vach sau
    setMaterial(0.10f, 0.10f, 0.14f, 12.0f);
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, -4.7f);
    drawCube(15.0f, 8.0f, 0.2f);
    glPopMatrix();
}

// =====================================================
// GIAN DEN
// =====================================================
void drawTrussColumn(float x, float z, float h)
{
    setMaterial(0.78f, 0.78f, 0.82f, 90.0f);

    float d = 0.22f;
    float r = 0.04f;

    for (int ix = -1; ix <= 1; ix += 2)
    {
        for (int iz = -1; iz <= 1; iz += 2)
        {
            glPushMatrix();
            glTranslatef(x + ix * d, h / 2.0f, z + iz * d);
            drawCylinder(r, h);
            glPopMatrix();
        }
    }

    for (float y = 0.5f; y < h; y += 0.9f)
    {
        glPushMatrix();
        glTranslatef(x, y, z);
        drawCube(0.55f, 0.05f, 0.55f);
        glPopMatrix();
    }
}

void drawMovingTrussBeam()
{
    setMaterial(0.86f, 0.86f, 0.90f, 100.0f);

    glPushMatrix();
    glTranslatef(0.0f, trussY, 0.0f);
    drawCube(10.5f, 0.18f, 0.35f);
    glPopMatrix();

    for (float x = -4.6f; x <= 4.6f; x += 1.2f)
    {
        glPushMatrix();
        glTranslatef(x, trussY, 0.0f);
        glRotatef(45.0f, 0, 0, 1);
        drawCube(0.75f, 0.04f, 0.08f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(x, trussY, 0.0f);
        glRotatef(-45.0f, 0, 0, 1);
        drawCube(0.75f, 0.04f, 0.08f);
        glPopMatrix();
    }
}

void drawTrussSystem()
{
    drawTrussColumn(-5.3f, 0.0f, 8.2f);
    drawTrussColumn(5.3f, 0.0f, 8.2f);
    drawMovingTrussBeam();
}

// =====================================================
// DEN PHA
// =====================================================
void drawLampBody()
{
    setMaterial(0.24f, 0.24f, 0.24f, 30.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawCube(0.42f, 0.06f, 0.14f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.14f, 0.02f, 0.0f);
    drawCube(0.04f, 0.32f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.14f, 0.02f, 0.0f);
    drawCube(0.04f, 0.32f, 0.08f);
    glPopMatrix();

    setMaterial(0.12f, 0.12f, 0.14f, 65.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.22f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);

    GLUquadric *q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, 0.17f, 0.12f, 0.42f, 24, 8);
    glTranslatef(0.0f, 0.0f, 0.42f);
    gluDisk(q, 0.0f, 0.12f, 24, 2);
    gluDeleteQuadric(q);

    glPopMatrix();
}

void drawLampAt(float x, float y, float z, float yaw, bool frontLamp)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw, 0, 1, 0);

    if (frontLamp)
        glRotatef(-68.0f, 1, 0, 0);
    else
        glRotatef(58.0f, 1, 0, 0);

    drawLampBody();
    glPopMatrix();
}

// =====================================================
// TIA SANG PHU TRO TU NHIEN HON
// =====================================================
void drawBeamCone(float r, float g, float b, float alpha,
                  float x, float y, float z,
                  float yaw, bool frontLamp,
                  float radius, float length)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw, 0, 1, 0);

    if (frontLamp)
        glRotatef(-68.0f, 1, 0, 0);
    else
        glRotatef(58.0f, 1, 0, 0);

    glTranslatef(0.0f, -0.62f, 0.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // lop ngoai
    glColor4f(r, g, b, alpha * 0.35f);
    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);
    drawCone(radius * 1.35f, length, 28);
    glPopMatrix();

    // lop giua
    glColor4f(r, g, b, alpha * 0.55f);
    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);
    drawCone(radius, length * 0.98f, 28);
    glPopMatrix();

    // lop trong
    glColor4f(r, g, b, alpha * 0.75f);
    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);
    drawCone(radius * 0.68f, length * 0.95f, 28);
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// =====================================================
// SPOTLIGHT THAT
// =====================================================
void setupSpot(GLenum lightId,
               float px, float py, float pz,
               float dx, float dy, float dz,
               float r, float g, float b,
               bool enableLight)
{
    if (enableLight)
        glEnable(lightId);
    else
        glDisable(lightId);

    GLfloat pos[] = {px, py, pz, 1.0f};
    GLfloat dif[] = {r, g, b, 1.0f};
    GLfloat spe[] = {r, g, b, 1.0f};
    GLfloat amb[] = {0.05f * r, 0.05f * g, 0.05f * b, 1.0f};
    GLfloat dir[] = {dx, dy, dz};

    glLightfv(lightId, GL_POSITION, pos);
    glLightfv(lightId, GL_DIFFUSE, dif);
    glLightfv(lightId, GL_SPECULAR, spe);
    glLightfv(lightId, GL_AMBIENT, amb);
    glLightfv(lightId, GL_SPOT_DIRECTION, dir);

    // toa rong hon de thay ro mau
    glLightf(lightId, GL_SPOT_CUTOFF, 30.0f);
    glLightf(lightId, GL_SPOT_EXPONENT, 14.0f);

    // giam suy hao manh de den sang ro
    glLightf(lightId, GL_CONSTANT_ATTENUATION, 0.55f);
    glLightf(lightId, GL_LINEAR_ATTENUATION, 0.010f);
    glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.0008f);
}

void setupAllLights()
{
    float topX[3] = {-3.0f, 0.0f, 3.0f};

    for (int i = 0; i < 3; i++)
    {
        float a = rotTop[i] * PI / 180.0f;

        float dx = sin(a) * 0.34f;
        float dy = -1.70f;
        float dz = 0.24f;

        float r = 0.0f, g = 0.0f, b = 0.0f;
        bool on = true;

        if (i == 0)
        {
            r = 1.0f;
            g = 0.0f;
            b = 0.0f;
            on = redOn;
        }
        if (i == 1)
        {
            r = 0.0f;
            g = 0.0f;
            b = 1.0f;
            on = blueOn;
        }
        if (i == 2)
        {
            r = 1.0f;
            g = 1.0f;
            b = 0.0f;
            on = yellowOn;
        }

        setupSpot(GL_LIGHT0 + i,
                  topX[i], trussY - 0.55f, -0.35f,
                  dx, dy, dz,
                  r, g, b, on);
    }

    float frontX[3] = {-4.0f, 0.0f, 4.0f};
    float frontY = 0.45f;
    float frontZ = 4.2f;

    for (int i = 0; i < 3; i++)
    {
        float a = rotFront[i] * PI / 180.0f;

        float dx = sin(a) * 0.22f;
        float dy = 1.28f;
        float dz = -0.10f;

        float r = 0.0f, g = 0.0f, b = 0.0f;
        bool on = true;

        if (i == 0)
        {
            r = 1.0f;
            g = 0.4f;
            b = 0.4f;
            on = redOn;
        }
        if (i == 1)
        {
            r = 0.4f;
            g = 0.4f;
            b = 1.0f;
            on = blueOn;
        }
        if (i == 2)
        {
            r = 1.0f;
            g = 1.0f;
            b = 0.2f;
            on = yellowOn;
        }

        setupSpot(GL_LIGHT3 + i,
                  frontX[i], frontY, frontZ,
                  dx, dy, dz,
                  r, g, b, on);
    }
}

// =====================================================
// VE DEN + TIA
// =====================================================
void drawAllLamps()
{
    drawLampAt(-3.0f, trussY - 0.02f, 0.0f, rotTop[0], false);
    drawLampAt(0.0f, trussY - 0.02f, 0.0f, rotTop[1], false);
    drawLampAt(3.0f, trussY - 0.02f, 0.0f, rotTop[2], false);

    drawLampAt(-4.0f, 0.45f, 4.2f, rotFront[0], true);
    drawLampAt(0.0f, 0.45f, 4.2f, rotFront[1], true);
    drawLampAt(4.0f, 0.45f, 4.2f, rotFront[2], true);
}

void drawAllBeams()
{
    // tren gian: ro mau, toa rong hon
    if (redOn)
        drawBeamCone(1.0f, 0.0f, 0.0f, 0.075f, -3.0f, trussY - 0.02f, 0.0f, rotTop[0], false, 0.72f, 8.2f);
    if (blueOn)
        drawBeamCone(0.0f, 0.0f, 1.0f, 0.075f, 0.0f, trussY - 0.02f, 0.0f, rotTop[1], false, 0.72f, 8.2f);
    if (yellowOn)
        drawBeamCone(1.0f, 1.0f, 0.0f, 0.075f, 3.0f, trussY - 0.02f, 0.0f, rotTop[2], false, 0.72f, 8.2f);

    // mep truoc: beam nhe hon nhung van thay ro
    if (redOn)
        drawBeamCone(1.0f, 0.4f, 0.4f, 0.040f, -4.0f, 0.45f, 4.2f, rotFront[0], true, 0.75f, 9.0f);
    if (blueOn)
        drawBeamCone(0.4f, 0.4f, 1.0f, 0.040f, 0.0f, 0.45f, 4.2f, rotFront[1], true, 0.75f, 9.0f);
    if (yellowOn)
        drawBeamCone(1.0f, 1.0f, 0.2f, 0.040f, 4.0f, 0.45f, 4.2f, rotFront[2], true, 0.75f, 9.0f);
}

// =====================================================
// HIEN THI
// =====================================================
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0f, 9.0f, 16.5f,
              0.0f, 2.8f, 0.0f,
              0.0f, 1.0f, 0.0f);

    setupAllLights();

    drawGridFloor();
    drawStage();
    drawTrussSystem();
    drawAllLamps();
    drawAllBeams();

    glutSwapBuffers();
}

// =====================================================
// CAP NHAT
// =====================================================
void update(int value)
{
    if (animateOn)
    {
        trussY += 0.028f * trussDir;
        if (trussY > 8.7f)
            trussDir = -1.0f;
        if (trussY < 6.3f)
            trussDir = 1.0f;

        for (int i = 0; i < 3; i++)
        {
            if (runTop[i])
            {
                rotTop[i] += 0.45f;
                if (rotTop[i] > 22.0f)
                    rotTop[i] = -22.0f;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            if (runFront[i])
            {
                rotFront[i] += 0.60f;
                if (rotFront[i] > 28.0f)
                    rotFront[i] = -28.0f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// =====================================================
// RESHAPE
// =====================================================
void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, ratio, 0.1f, 100.0f);
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
}

// =====================================================
// PHIM
// =====================================================
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;

    case 'r':
    case 'R':
        redOn = !redOn;
        break;

    case 'g':
    case 'G':
        blueOn = !blueOn;
        break;

    case 'b':
    case 'B':
        yellowOn = !yellowOn;
        break;

    case ' ':
        animateOn = !animateOn;
        break;

    case '1':
        runTop[0] = !runTop[0];
        break;
    case '2':
        runTop[1] = !runTop[1];
        break;
    case '3':
        runTop[2] = !runTop[2];
        break;
    case '4':
        runFront[0] = !runFront[0];
        break;
    case '5':
        runFront[1] = !runFront[1];
        break;
    case '6':
        runFront[2] = !runFront[2];
        break;
    }

    glutPostRedisplay();
}

// =====================================================
// KHOI TAO
// =====================================================
void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // ambient toan canh cao hon 1 chut de nhin ro san khau
    GLfloat globalAmbient[] = {0.12f, 0.12f, 0.12f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glClearColor(0.06f, 0.06f, 0.11f, 1.0f);
}

// =====================================================
// MAIN
// =====================================================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("He thong chieu sang san khau - sang ro mau");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}