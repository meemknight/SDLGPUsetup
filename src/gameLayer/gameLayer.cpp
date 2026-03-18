#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <platformTools.h>
#include <logs.h>
#include <SDL3/SDL.h>
#include <gl2d/gl2d.h>
#include <glui/glui.h>

#include <imguiTools.h>

gl2d::Renderer2D renderer;

// Rebuilds shader binaries in development and reloads GPU shader objects.
static void tryHotReloadShaders()
{
#if defined(_WIN32) && defined(DEVELOPLEMT_BUILD) && (DEVELOPLEMT_BUILD == 1)
	std::string scriptPath = std::string(RESOURCES_PATH) + "shaders/compile_all_shaders.bat";
	for (char &c : scriptPath)
	{
		if (c == '/') { c = '\\'; }
	}

	std::ifstream scriptFile(scriptPath);
	if (!scriptFile.is_open())
	{
		platform::log("Shader reload failed: missing resources\\shaders\\compile_all_shaders.bat",
			LogManager::logError);
		return;
	}

	const std::string command = std::string("cmd.exe /C call \"") + scriptPath + "\"";
	int result = std::system(command.c_str());
	if (result != 0)
	{
		platform::log("Shader reload failed: compile_all_shaders.bat returned an error",
			LogManager::logError);
		return;
	}

	renderer.reloadGpuShaders();

	platform::log("Hot reloaded shaders", LogManager::logNormal);
#else
	platform::log("Shader reload is only enabled on Windows development builds", LogManager::logWarning);
#endif
}

gl2d::Renderer2D &getRenderer()
{
	return renderer;
}


bool initGame(SDL_Renderer *sdlRenderer)
{

	gl2d::init();


	renderer.create(sdlRenderer);
	#if GL2D_USE_SDL_GPU
	if (!renderer.gpuDevice)
	{
		platform::log("SDL_gpu device unavailable on this platform; GPU post-process effects are disabled.",
			LogManager::logWarning);
	}
	#endif

	//assetsManager.loadAllAssets();


	return true;
}


bool gameLogic(float deltaTime, platform::Input &input, SDL_Renderer *sdlRenderer)
{

#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	renderer.updateWindowMetrics(w, h);

	if (input.isButtonPressed(platform::Button::F1))
	{
		platform::setFullScreen(!platform::isFullScreen());
	}

	if (input.isButtonPressed(platform::Button::F5))
	{
		tryHotReloadShaders();
	}

	renderer.clearScreen();


	renderer.renderRectangle({10,10, 60, 60}, Colors_Blue);



	#if GL2D_USE_SDL_GPU
	if (!renderer.gpuDevice)
	#endif
	{
		renderer.flush();
	}


	return true;

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
