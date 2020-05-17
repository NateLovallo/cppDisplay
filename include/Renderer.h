#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "IRenderable.h"

#pragma once

class Renderer
{
public:

   using EventCallback = std::function<void(SDL_Event e)>;

   ///////////////////////////////////////////////////////////////////////////////
   Renderer();

   ///////////////////////////////////////////////////////////////////////////////
   ~Renderer();

   ///////////////////////////////////////////////////////////////////////////////
   void AddItem(IRenderable* r);
   
   ///////////////////////////////////////////////////////////////////////////////
   void AddHandler(Uint32 eventType, EventCallback e);
   
   ///////////////////////////////////////////////////////////////////////////////
   void Draw();

private:
   
   // TODO configurable?
   static constexpr unsigned int mSizeX = 800;
   static constexpr unsigned int mSizeY = 480;

   SDL_Surface* mScreenSurface = NULL;
   SDL_Renderer* mRenderer = NULL;
   SDL_Window* mWindow = NULL;
   SDL_Texture* mTexture = NULL;
   bool mRun = true;
   
   std::vector<IRenderable*> mRenderables;
   std::unordered_map<Uint32,EventCallback> mEventCallbacks; 
};

