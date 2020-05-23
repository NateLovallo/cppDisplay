
#include "Renderer.h"

#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <assert.h>
#include <functional>
#include <thread>

#include "Renderer.h"

///////////////////////////////////////////////////////////////////////////////
Renderer::Renderer()
{
   if (SDL_Init(SDL_INIT_VIDEO) < 0) 
   {
      fprintf(stderr,"ERROR in SDL_Init(): %s\n",SDL_GetError());
   }

   SDL_ShowCursor(SDL_DISABLE);

   mWindow= SDL_CreateWindow( "SDL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSizeX, mSizeY, SDL_WINDOW_SHOWN );
   if (mWindow == NULL)
   {
      fprintf(stderr,"ERROR in SDL_CreateWindow(): %s\n",SDL_GetError());
   }

   mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED );
   if( mRenderer == NULL )
   {
      fprintf(stderr,"Coudln't make renderino %s\n",SDL_GetError());
   }

   mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, mSizeX, mSizeY);
   if( mTexture == NULL )
   {
      fprintf(stderr,"Coudln't make texture %s\n",SDL_GetError());
   }
}

///////////////////////////////////////////////////////////////////////////////
Renderer::~Renderer()
{
   mRun = false;
   SDL_DestroyTexture(mTexture);
   SDL_DestroyRenderer(mRenderer);
   SDL_DestroyWindow(mWindow);
   SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::AddItem(IRenderable* r)
{
   mRenderables.push_back(r);
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::AddHandler(Uint32 eventType, EventCallback e)
{
   // make sure there isn't a handler yet
   assert(mEventCallbacks.find(eventType) == mEventCallbacks.end());
   mEventCallbacks[eventType] = e;
}


///////////////////////////////////////////////////////////////////////////////
void Renderer::HandleEvents()
{
   SDL_Event e;

   while (SDL_PollEvent(&e) != 0)
   {
      auto cb = mEventCallbacks.find(e.type);
      if (cb != mEventCallbacks.end())
      {
         cb->second(e);
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void Renderer::Draw()
{
   HandleEvents();

   SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
   SDL_RenderClear(mRenderer);

   void* pixels;
   int pitch;

   if (0 == SDL_LockTexture(mTexture, NULL, &pixels, &pitch))
   {
      cairo_surface_t* cairo_surface =
         cairo_image_surface_create_for_data((unsigned char*)pixels, CAIRO_FORMAT_ARGB32, mSizeX, mSizeY, pitch);
      cairo_t* cr = cairo_create(cairo_surface);

      // black background
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_paint(cr);

      for (auto& r : mRenderables)
      {
         r->Render(cr);
         cairo_set_source_rgba(cr, 0, 0, 0, 0);
      }

      cairo_surface_destroy(cairo_surface);
      cairo_destroy(cr);

      SDL_UnlockTexture(mTexture);
   }

   // draw to screen
   SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);

   // flip buffer
   SDL_RenderPresent(mRenderer);
}
