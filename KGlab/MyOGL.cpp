﻿#include "MyOGL.h"
#include <stdio.h>
#include <Math.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <chrono>


#include "Render.h"


OpenGL gl;

HWND wnd;

//блокировщики
std::mutex					hwnd_mutex;
std::mutex					message_mutex;
std::condition_variable		message_cv;
bool						have_message=true;







std::thread gl_thread;

std::thread msg_thread;
std::deque<Message> msg_deque;

std::atomic_bool bRender;
std::atomic_bool bMsg;

void render_cycle ();
void message_cycle();

void setHwnd(HWND window)
{
	std::lock_guard<std::mutex> guard(hwnd_mutex);
	gl.setHWND(window);
}

void start_gl_thread()
{
	bRender = true;
	gl_thread = std::thread(render_cycle);
}

void start_msg_thread()
{
	std::unique_lock<std::mutex> lock(message_mutex);
	bMsg = true;
	msg_thread = std::thread(message_cycle);
	
}

void add_message(Message msg)
{
	std::unique_lock<std::mutex> lock(message_mutex);
	have_message = true;
	message_cv.notify_all();
	msg_deque.push_back(msg);
}

void stop_all_threads()
{
	bRender = false;
	bMsg = false;
	gl_thread.join();
	have_message = true;
	message_cv.notify_all();
	msg_thread.join();
}

void render_cycle ()
{	
		gl.init();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		initRender();

		auto end_render = std::chrono::steady_clock::now();
	
		while (bRender)
		{
			auto cur_time = std::chrono::steady_clock::now();
			auto deltatime = cur_time - end_render;
			double delta = 1.0*std::chrono::duration_cast<std::chrono::microseconds>(deltatime).count()/1000000;
			end_render = cur_time;
			gl.render(delta);
		}
}

void message_cycle()
{
	while (bMsg)
	{
		short mouseX = -1;
		short mouseY = -1;

		std::unique_lock<std::mutex> lock(message_mutex);
		message_cv.wait(lock, [&]() {return have_message; });

		while (!msg_deque.empty())
		{			
			auto m = msg_deque.front();
			msg_deque.pop_front();

			switch (m.message)
			{
				case WM_MOUSELEAVE:
					gl.mouseLeave(mouseX, mouseY);
					break;
				case WM_MOUSEWHEEL:		
					gl.wheelEvent(GET_WHEEL_DELTA_WPARAM(m.wParam));
					break;	
				case WM_MOUSEMOVE:
					mouseX = (short)LOWORD(m.lParam);
					mouseY = (short)HIWORD(m.lParam);
					gl.mouseMovie((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_SIZE:
					gl.try_to_resize((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_LBUTTONDOWN:
					gl.mouseLdown((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_LBUTTONUP:
					gl.mouseLup((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_RBUTTONDOWN:
					gl.mouseRdown((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_RBUTTONUP:
					gl.mouseRup((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_MBUTTONDOWN:
					gl.mouseMdown((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_MBUTTONUP:
					gl.mouseMup((short)LOWORD(m.lParam), (short)HIWORD(m.lParam));
					break;
				case WM_KEYUP:
					gl.keyUp(m.wParam);
					break;
				case WM_KEYDOWN:
					gl.keyDown(m.wParam);
					break;
				case WM_CLOSE:
					//b_render = false;
					bMsg = false;
					msg_deque.clear();
					break;									
			}	
		}
		if (!bMsg) break;
		have_message = false;
	}

}

OpenGL::OpenGL()
{		
		

}
OpenGL::~OpenGL()
{

}

void OpenGL::setHWND(HWND window)
{
	g_hWnd = window;
}



void OpenGL::wheelEvent(float delta)
{
	MouseWheelEventArg arg{ delta };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this,arg]() {WheelEvent.exec(this, arg); });	
}


void OpenGL::mouseMovie(short mX, short mY)
{	
	MouseEventArg arg{ mX, mY};
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseMovieEvent.exec(this, arg); });
}

void OpenGL::mouseLeave(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseLeaveEvent.exec(this, arg); });
}

void OpenGL::mouseLdown(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseLdownEvent.exec(this, arg); });
}

void OpenGL::mouseLup(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseLupEvent.exec(this, arg); });
}

void OpenGL::mouseRdown(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseRdownEvent.exec(this, arg); });
}

void OpenGL::mouseRup(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseRupEvent.exec(this, arg); });
}

void OpenGL::mouseMdown(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseMdownEvent.exec(this, arg); });
}

void OpenGL::mouseMup(short mX, short mY)
{
	MouseEventArg arg{ mX, mY };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {MouseMupEvent.exec(this, arg); });
}

void OpenGL::keyDown(int key)
{	
	KeyEventArg arg{ key };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {KeyDownEvent.exec(this, arg); });
}

void OpenGL::keyUp(int key)
{
	KeyEventArg arg{ key };
	std::lock_guard<std::mutex> lock(events_mutex);
	events_for_render.push_back([this, arg]() {KeyUpEvent.exec(this, arg); });
}


void OpenGL::DrawAxes()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 10);
	glEnd();
	
	

	glColor3f(0.0f, 0.0f, 0.0f);

}


void OpenGL::render(double delta)
{
	
	glMatrixMode(GL_MODELVIEW);

	if (resize_pending)
	{
		resize_pending = false;
		gl.resize(gl.tmp_width,gl.tmp_height);
	}		

	if (events_for_render.empty() == false)
	{
		std::lock_guard<std::mutex> lock(events_mutex);
		for (auto& x : events_for_render)
			x();
		events_for_render.clear();
		
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
			

	glDisable(GL_LIGHTING);
		
	
	Render(delta);


	SwapBuffers(g_hDC);
}

void OpenGL::try_to_resize(int w, int h)
{
	resize_pending = true;
	tmp_height = h;
	tmp_width = w;
}

void OpenGL::resize(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);	
		
	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity();	
			
	gluPerspective(45.0, (GLdouble)width / (GLdouble)height, 0.2, 200.0);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();									
}

void OpenGL::init(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	g_hDC = GetDC(g_hWnd);
	GLuint iPixelFormat = ChoosePixelFormat(g_hDC, &pfd);

	if (iPixelFormat != 0)
	{
		PIXELFORMATDESCRIPTOR bestMatch_pfd;
		DescribePixelFormat(g_hDC, iPixelFormat, sizeof(pfd), &bestMatch_pfd);

		if (bestMatch_pfd.cDepthBits < pfd.cDepthBits)
		{
			return;
		}

		if (SetPixelFormat(g_hDC, iPixelFormat, &pfd) == FALSE)
		{
			DWORD dwErrorCode = GetLastError();
			return;
		}
	}
	else
	{
		DWORD dwErrorCode = GetLastError();
		return;
	}

	g_hRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hRC);

}

