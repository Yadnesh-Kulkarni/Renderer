#include <gl/GL.h>
#include <gl/GLU.h>
#include "renderer/Renderer.h"

class OpenGLRenderer : public Renderer {
	OpenGLRenderer(WindowCreator* window);
	~OpenGLRenderer();

	virtual void BeginFrame();
	virtual void EndFrame() ;
	virtual void DrawFrame();

	virtual void SwapBuffers();
	virtual void Initialize();

	virtual void WaitIdle();

	virtual void SetRequiredExtensions();
};