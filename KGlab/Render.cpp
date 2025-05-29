// clang-format off
#include "Render.h"
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "GUItextRectangle.h"
#include "figure/MyFigure.h"
// clang-format on


#ifdef _DEBUG
#include <Debugapi.h>
struct debug_print
{
    template <class C>
    debug_print& operator<<(const C& a)
    {
        OutputDebugStringA((std::stringstream() << a).str().c_str());
        return *this;
    }
} debout;
#else
struct debug_print
{
    template <class C>
    debug_print& operator<<(const C& a)
    {
        return *this;
    }
} debout;
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;


bool texturing = true;
bool lightning = true;
bool alpha = false;

void switchModes(OpenGL* sender, KeyEventArg arg)
{
    auto key = LOWORD(MapVirtualKeyA(arg.key, MAPVK_VK_TO_CHAR));

    switch (key)
    {
    case 'L':
        lightning = !lightning;
        break;
    case 'T':
        texturing = !texturing;
        break;
    case 'A':
        alpha = !alpha;
        break;
    }
}

GuiTextRectangle text;
GLuint texId;
void initRender()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    int x, y, n;
    unsigned char* data = stbi_load("texture.png", &x, &y, &n, 4);
    unsigned char* _tmp = new unsigned char[x * 4];
    for (int i = 0; i < y / 2; ++i)
    {
        std::memcpy(_tmp, data + i * x * 4, x * 4);
        std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
        std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
    }
    delete[] _tmp;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    camera.caclulateCameraPos();
    gl.WheelEvent.reaction(&camera, &Camera::Zoom);
    gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
    gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
    gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
    gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);
    gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
    gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
    gl.KeyUpEvent.reaction(&light, &Light::StopDrug);
    gl.KeyDownEvent.reaction(switchModes);
    text.setSize(512, 180);
    camera.setPosition(2, 1.5, 1.5);
}


//==========================Setup Render==========================
void setupCameraAndLight()
{
    glEnable(GL_DEPTH_TEST);
    if (gl.isKeyPressed('F'))
    {
        light.SetPosition(camera.x(), camera.y(), camera.z());
    }
    camera.SetUpCamera();
    light.SetUpLight();
    gl.DrawAxes();
}
void setupRenderingModes()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    if (lightning) glEnable(GL_LIGHTING);
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (alpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
void setupMaterial()
{
    float amb[] = {0.2, 0.2, 0.1, 1.};
    float dif[] = {0.4, 0.65, 0.5, 1.};
    float spec[] = {0.9, 0.8, 0.3, 1.};
    float sh = 0.2f * 256;
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, sh);
    glShadeModel(GL_SMOOTH);
}
void setupTopRightMessage(double delta_time)
{
    light.DrawLightGizmo();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, gl.getWidth() - 1, 0, gl.getHeight() - 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "T - " << (texturing ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"текстур" << std::endl;
    ss << "L - " << (lightning ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"освещение" << std::endl;
    ss << "A - " << (alpha ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"альфа-наложение" << std::endl;
    ss << L"F - Свет из камеры" << std::endl;
    ss << L"G - двигать свет по горизонтали" << std::endl;
    ss << L"G+ЛКМ двигать свет по вертекали" << std::endl;
    ss << L"Коорд. света: (" << std::setw(7) << light.x() << "," << std::setw(7) << light.y() << "," << std::setw(7)
       << light.z() << ")" << std::endl;
    ss << L"Коорд. камеры: (" << std::setw(7) << camera.x() << "," << std::setw(7) << camera.y() << "," << std::setw(7)
       << camera.z() << ")" << std::endl;
    ss << L"Параметры камеры: R=" << std::setw(7) << camera.distance() << ",fi1=" << std::setw(7) << camera.fi1()
       << ",fi2=" << std::setw(7) << camera.fi2() << std::endl;
    ss << L"delta_time: " << std::setprecision(5) << delta_time << std::endl;
    text.setPosition(10, gl.getHeight() - 10 - 180);
    text.setText(ss.str().c_str());
    text.Draw();
}
void setupFinishRendering()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void drawScene()
{
    glBindTexture(GL_TEXTURE_2D, texId);
    drawFigure();
}
//================================================================

void Render(double delta_time)
{
    setupCameraAndLight();
    setupRenderingModes();
    setupMaterial();

    drawScene();
    
    setupTopRightMessage(delta_time);
    setupFinishRendering();
}
