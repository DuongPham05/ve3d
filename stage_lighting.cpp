#include <windows.h> // Bắt buộc thêm trên VS Code/MinGW Windows để tránh lỗi APIENTRY
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define PI 3.1415926535f

// --- Biến điều khiển ---
float moveY = 0.0f;
float angleRotate = 0.0f;
float timeVal = 0.0f;
bool isRotating = true;
bool lightStatus[6] = { true, true, true, true, true, true };

// --- Hàm khởi tạo ánh sáng ---
void initLight(int lightID, float r, float g, float b) {
    glEnable(lightID);
    GLfloat diffuse[] = { r, g, b, 1.0f };
    // Specular mạnh hơn để làm nổi bật bề mặt kim loại
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(lightID, GL_DIFFUSE, diffuse);
    glLightfv(lightID, GL_SPECULAR, specular);
    // Spotlight gom sáng hơn
    glLightf(lightID, GL_SPOT_CUTOFF, 35.0f);
    glLightf(lightID, GL_SPOT_EXPONENT, 15.0f);
}

void init() {
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f); // Màu nền tối sẫm
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    // Bật chế độ tô màu Smooth quan trọng cho luồng sáng
    glShadeModel(GL_SMOOTH);

    // Ambient toàn cục rất thấp
    GLfloat global_ambient[] = { 0.05f, 0.05f, 0.1f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    initLight(GL_LIGHT0, 1.0f, 0.1f, 0.1f); // Đỏ rực
    initLight(GL_LIGHT1, 0.1f, 1.0f, 0.1f); // Xanh lá rực
    initLight(GL_LIGHT2, 0.1f, 0.1f, 1.0f); // Xanh dương rực
    initLight(GL_LIGHT3, 1.0f, 1.0f, 0.1f); // Vàng
    initLight(GL_LIGHT4, 0.1f, 1.0f, 1.0f); // Cyan
    initLight(GL_LIGHT5, 1.0f, 0.1f, 1.0f); // Tím
}

// =========================================================
// HÀM VẼ LUỒNG SÁNG & HÀO QUANG (ĐÃ LÀM NỔI BẬT ĐÈN)
// =========================================================
void drawLightBeamWithGlow(float r, float g, float b) {
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending

    // --- 1. Vẽ Hào quang tại đỉnh đèn (Làm nổi bật đèn) ---
    int glowSlices = 16;
    float glowRadius = 1.5f; // Bán kính hào quang rực rỡ
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r, g, b, 0.8f); // Tâm rất sáng
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor4f(r, g, b, 0.0f); // Rìa tan biến
    for (int i = 0; i <= glowSlices; i++) {
        float rad = (float)i * 360.0f / glowSlices * PI / 180.0f;
        glVertex3f(cos(rad) * glowRadius, 0.0f, sin(rad) * glowRadius);
    }
    glEnd();

    // --- 2. Vẽ Luồng sáng nón (Làm mịn như ảnh gốc) ---
    int slices = 36;
    float baseRadius = 7.0f; // Luồng sáng rộng hơn
    float coneHeight = 35.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r, g, b, 0.4f); // Đỉnh luồng sáng
    glVertex3f(0.0f, -0.5f, 0.0f); // Lùi xuống 1 chút
    glColor4f(r, g, b, 0.0f); // Đáy tan biến
    for (int i = 0; i <= slices; i++) {
    float rad = (float)i * 360.0f / slices * PI / 180.0f;
        glVertex3f(cos(rad) * baseRadius, -coneHeight, sin(rad) * baseRadius);
    }
    glEnd();

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

// --- Hàm vẽ bệ đèn pha (Đã làm lớn hơn để nổi bật) ---
void drawLampBody() {
    glColor3f(0.4f, 0.4f, 0.4f); // Màu xám kim loại
    // Thêm chất liệu bóng cho thân đèn
    GLfloat matSpecular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat matShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glPushMatrix();
    glScalef(1.2f, 1.8f, 1.2f); // Làm lớn bệ đèn
    glutSolidCube(1.0);
    glPopMatrix();

    // Tắt Material specular sau khi vẽ xong đèn
    GLfloat noSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, noSpecular);
}

// =========================================================
// HÀM VẼ GIÀN ĐÈN TRUSS (ĐÃ LÀM NỔI BẬT CỘT & XÀ)
// =========================================================
void drawTrussBeam(float length) {
    glColor3f(0.6f, 0.6f, 0.6f); // Màu nhôm sáng
    // Chất liệu kim loại Specular mạnh
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess[] = { 80.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    // Vẽ 4 thanh chính (tạo khối hộp truss)
    float beamThickness = 0.2f;
    float trussWidth = 0.6f;

    // 4 Thanh dọc
    glPushMatrix(); glTranslatef(trussWidth, 0, trussWidth); glScalef(beamThickness, length, beamThickness); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-trussWidth, 0, trussWidth); glScalef(beamThickness, length, beamThickness); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(trussWidth, 0, -trussWidth); glScalef(beamThickness, length, beamThickness); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(-trussWidth, 0, -trussWidth); glScalef(beamThickness, length, beamThickness); glutSolidCube(1.0); glPopMatrix();

    // Vẽ các thanh giằng chéo (Z-bracing) để làm nổi bật truss
    glColor3f(0.4f, 0.4f, 0.4f); // Giằng tối hơn
    glLineWidth(2.0f);
    glDisable(GL_LIGHTING); // Tắt lighting để vẽ đường nét
    glBegin(GL_LINES);
    float numSegments = length / 2.0f;
    for (int i = 0; i < (int)numSegments; i++) {
        float y_start = -length / 2.0f + i * 2.0f;
        float y_end = y_start + 2.0f;
        // Mặt trước
        glVertex3f(-trussWidth, y_start, trussWidth); glVertex3f(trussWidth, y_end, trussWidth);
        glVertex3f(trussWidth, y_start, trussWidth); glVertex3f(-trussWidth, y_end, trussWidth);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Tắt Material specular
    GLfloat noSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, noSpecular);
}

// =========================================================
// HÀM VẼ BỤC SÂN KHẤU (THÊM MỚI SÂN KHẤU)
// =========================================================
void drawFloatingStage() {
    // 1. Mặt bục sân khấu chính
    glColor3f(0.08f, 0.08f, 0.1f); // Màu xám rất tối
    GLfloat matSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat matShininess[] = { 20.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glPushMatrix();
    glTranslatef(0, 1.5f, 0); // Bục cao 1.5 đơn vị
    glScalef(50.0f, 3.0f, 30.0f); // Bục rộng 50x30
    glutSolidCube(1.0);
    glPopMatrix();

    // Tắt Material specular
    GLfloat noSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, noSpecular);
}

// --- Hàm Display ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 35, 75, 0, 5, 0, 0, 1, 0); // Góc nhìn bao quát hơn

    // 1. Vẽ Mặt bục sân khấu (Thêm mới)
    drawFloatingStage();

    // 2. Vẽ mặt sàn nền tối phía dưới
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f); // Màu sàn nền rất tối
    glNormal3f(0, 1, 0);
    glVertex3f(-100, 0, -100);
    glVertex3f(100, 0, -100);
    glVertex3f(100, 0, 100);
    glVertex3f(-100, 0, 100);
    glEnd();

    // 3. Hệ thống giàn đèn Truss (Đã làm nổi bật cột & xà)
    // Cột đứng trái/phải chi tiết hơn
    glPushMatrix(); glTranslatef(-22, 12, -10); drawTrussBeam(24); glPopMatrix();
    glPushMatrix(); glTranslatef(22, 12, -10); drawTrussBeam(24); glPopMatrix();

    // 4. Thanh ngang di chuyển và 3 đèn treo
    glPushMatrix();
    glTranslatef(0, 22.0f + moveY, -10.0f);
    // Thanh ngang chi tiết hơn
    glPushMatrix(); glRotatef(90, 0, 0, 1.0f); drawTrussBeam(40); glPopMatrix();

    float topX[] = { -14, 0, 14 };
    float topColors[3][3] = { {1,0.1,0.1}, {0.1,1,0.1}, {0.1,0.1,1} };
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(topX[i], -2.0f, 0);
        glRotatef(angleRotate, 0, 1, 0);
        glRotatef(35, 1, 0, 0); // Nghiêng hơn 1 chút

        GLfloat pos[] = { 0, 0, 0, 1 };
        GLfloat dir[] = { 0, -1, 0 };
        glLightfv(GL_LIGHT0 + i, GL_POSITION, pos);
        glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, dir);

        drawLampBody(); // Bệ đèn nổi bật hơn
        if (lightStatus[i]) drawLightBeamWithGlow(topColors[i][0], topColors[i][1], topColors[i][2]);
        glPopMatrix();
    }
    glPopMatrix();

    // 5. Ba đèn pha đặt sát mép sân khấu (Chiếu ngược lên)
    float botX[] = { -12, 0, 12 };
    float botColors[3][3] = { {1,1,0.1}, {0.1,1,1}, {1,0.1,1} };
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(botX[i], 3.2f, 13.0f); // Đặt trên mặt bục sân khấu (y=3.2)
        glRotatef(angleRotate, 0, 1, 0);
        glRotatef(-155, 1, 0, 0); // Chiếu ngược lên không trung
        
        GLfloat pos[] = { 0, 0, 0, 1 };
        GLfloat dir[] = { 0, -1, 0 };
        glLightfv(GL_LIGHT3 + i, GL_POSITION, pos);
        glLightfv(GL_LIGHT3 + i, GL_SPOT_DIRECTION, dir);

        drawLampBody(); // Bệ đèn nổi bật hơn
        if (lightStatus[i + 3]) drawLightBeamWithGlow(botColors[i][0], botColors[i][1], botColors[i][2]);
        glPopMatrix();
    }

    glutSwapBuffers();
}

// --- Các hàm Keyboard, Idle, Reshape (giữ nguyên tốc độ chậm 1 nửa) ---
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r': case 'R': lightStatus[0] = !lightStatus[0]; break;
        case 'g': case 'G': lightStatus[1] = !lightStatus[1]; break;
        case 'b': case 'B': lightStatus[2] = !lightStatus[2]; break;
        case '1': case 'Y': lightStatus[3] = !lightStatus[3]; break;
        case '2': case 'C': lightStatus[4] = !lightStatus[4]; break;
        case '3': case 'P': lightStatus[5] = !lightStatus[5]; break;
        case '4': isRotating = !isRotating; break;
        case 27: exit(0); break;
    }
    for (int i = 0; i < 6; i++) {
        if (lightStatus[i]) glEnable(GL_LIGHT0 + i);
        else glDisable(GL_LIGHT0 + i);
    }
    glutPostRedisplay();
}

void idle() {
    timeVal += 0.005f; // Tốc độ thời gian chậm 1 nửa
    moveY = sin(timeVal * 1.0f) * 3.0f;
    if (isRotating) angleRotate += 0.25f; // Tốc độ xoay chậm 1 nửa
    glutPostRedisplay();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 1.0, 300.0); // Tầm nhìn xa hơn
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Kiem tra Giua ky - Do hoa may tinh - STAGE UPDATE");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}