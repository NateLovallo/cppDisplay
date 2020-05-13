#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <vector>
#include <memory>
#include <cstdint>

#pragma once


class IRenderable
{
   public:
   virtual void Render(cairo_t *cr) = 0;
};

class ArcGauge : public IRenderable
{
   public:

   
   void Update(uint32_t val) 
   {
      mValue = val;
   }
   
   void Render(cairo_t *cr) override final
   {
      cairo_set_source_rgba (cr, 0, 1, 0, 0.5);
      cairo_set_line_width (cr, 10.0);
      cairo_arc (cr, 200, 200, 100, (0)*(M_PI/180.0), (270.0)*(M_PI/180.0));
      cairo_stroke (cr);
   }
   
   private:
   
   uint32_t mValue = 0;
};


class Renderer
{
public:

using RenderPtr_t = std::unique_ptr<IRenderable>;

Renderer()
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

~Renderer()
{
   mRun = false;
   SDL_DestroyTexture(mTexture);
   SDL_DestroyRenderer(mRenderer);
   SDL_DestroyWindow(mWindow);
   SDL_Quit();
}

void RenderLoop()
{
   while (mRun)
   {
      uint32_t startTicks = SDL_GetTicks();
      SDL_Event e;

      while( SDL_PollEvent( &e ) != 0 )
      {
         if(e.type == SDL_KEYDOWN)
         {
            mRun = false;
         }
         else if(e.type == SDL_QUIT)
         {
            mRun = false;
         }
      }

      SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      SDL_RenderClear( mRenderer );

      void* pixels;
      int pitch;

      if (0 == SDL_LockTexture(mTexture, NULL, &pixels, &pitch))
      {
         cairo_surface_t *cairo_surface =
            cairo_image_surface_create_for_data ((unsigned char*)pixels, CAIRO_FORMAT_ARGB32, mSizeX, mSizeY, pitch);
         cairo_t *cr=cairo_create(cairo_surface);

         cairo_set_source_rgb (cr, 0, 0, 0);
         cairo_paint (cr);
         
         for (auto& r : mRenderables)
         {
            r->Render(cr);
         }

         cairo_surface_destroy (cairo_surface);
         cairo_destroy (cr);

         SDL_UnlockTexture(mTexture);
      }

       // draw to screen
      SDL_RenderCopy( mRenderer, mTexture, NULL, NULL);
      
      // flip buffer
      SDL_RenderPresent(mRenderer);

      SDL_Delay(20);
   }

}

void AddItem(RenderPtr_t r)
{
   mRenderables.push_back(std::move(r));
}

private:

   static constexpr unsigned int mSizeX = 800;
   static constexpr unsigned int mSizeY = 480;

   SDL_Surface* mScreenSurface = NULL;
   SDL_Renderer* mRenderer = NULL;
   SDL_Window* mWindow = NULL;
   SDL_Texture* mTexture = NULL;
   bool mRun = true;
   
   std::vector<RenderPtr_t> mRenderables;

};

