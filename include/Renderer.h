#include <SDL2/SDL.h>
#include <vector>
#include <thread>

#include "IRenderable.h"

#pragma once

class Renderer
{
public:
   ///////////////////////////////////////////////////////////////////////////////
   Renderer();

   ///////////////////////////////////////////////////////////////////////////////
   ~Renderer();

   ///////////////////////////////////////////////////////////////////////////////
   void AddItem(IRenderable* r);

private:

   void RenderLoop();

   static constexpr unsigned int mSizeX = 800;
   static constexpr unsigned int mSizeY = 480;

   SDL_Surface* mScreenSurface = NULL;
   SDL_Renderer* mRenderer = NULL;
   SDL_Window* mWindow = NULL;
   SDL_Texture* mTexture = NULL;
   bool mRun = true;
   
   std::vector<IRenderable*> mRenderables;
   std::thread mThread;
};

