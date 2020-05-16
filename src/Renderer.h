#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <assert.h>
#include <functional>
#include <thread>

#pragma once


class IRenderable
{
   public:
   virtual void Render(cairo_t *cr) = 0;
   
   void SetPosition(double x, double y)
   {
      X = x;
      Y = y;
   }
   protected:
   double X = 0;
   double Y = 0;
};

class ArcGauge : public IRenderable
{
   public:
   ArcGauge(uint32_t min, uint32_t max) : mMin(min), mMax(max) 
   {
      assert(mMin < mMax);
   }
   
   ArcGauge() = delete;

   
   void Update(uint32_t val) 
   {
      mValue = val;

      if (mValue < mMin)
      {
         mValue = mMin;
      } 
      
      if (mValue > mMax)
      {
         mValue = mMax;
      }
   }
   
   void Render(cairo_t *cr) override final
   {
      uint32_t span = mMax - mMin;
      float p = float(mValue - mMin) / float(span);
      
      
      cairo_arc (cr, X, Y, 100, (0)*(M_PI/180.0), (p*360.0)*(M_PI/180.0));
      cairo_set_source_rgba (cr, 0, 1, 0, 0.5);
      cairo_set_line_width (cr, 10.0);
      cairo_stroke (cr);
      
      cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
      cairo_set_font_size (cr, 30.0);
      cairo_move_to (cr, X, Y);
      
      char sText[32];
      sprintf(sText, "%u", mValue);
      cairo_show_text (cr, sText);
   }
   
   private:
   
   const uint32_t mMin, mMax;
   
   uint32_t mValue = 0;
};


class Renderer
{
public:

using RenderPtr_t = std::shared_ptr<IRenderable>;

Renderer()
:
mThread([this](){RenderLoop();})
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
   mThread.join();
   SDL_DestroyTexture(mTexture);
   SDL_DestroyRenderer(mRenderer);
   SDL_DestroyWindow(mWindow);
   SDL_Quit();
}



void AddItem(IRenderable* r)
{
   mRenderables.push_back(r);
}

private:
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

         // black background
         cairo_set_source_rgb (cr, 0, 0, 0);
         cairo_paint (cr);
         
         for (auto& r : mRenderables)
         {
            r->Render(cr);
            cairo_set_source_rgba (cr, 0, 0, 0, 0);
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

