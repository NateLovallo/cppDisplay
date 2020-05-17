#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <assert.h>
#include <functional>
#include <thread>

#include "IRenderable.h"

#pragma once


class ArcGauge : public IRenderable
{
   public:
   ArcGauge(uint32_t min, uint32_t max, const std::string& title, const std::string& units) 
   : mMin(min), mMax(max), mTitle(title), mUnits(units) 
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
      
      float start = 120.0f;
      float end = 60+360;
      
      cairo_arc (cr, X, Y, 100, start*(M_PI/180.0), end*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 0.5);
      cairo_set_line_width (cr, 10.0);
      cairo_stroke (cr);
      
      float total = start + (end - start)*p;
      
      cairo_arc (cr, X, Y, 100, start*(M_PI/180.0), total*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0, 1, 0, 0.5);
      cairo_set_line_width (cr, 20.0);
      cairo_stroke (cr);

      cairo_select_font_face (cr, "../Gidole-Regular.ttf", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, 30.0);
      
      
      char sText[32];

      sprintf(sText, "%6u %s", mValue, mUnits.c_str());
      
      // Draw the numerical printout
      cairo_text_extents_t xt;
      cairo_text_extents(cr, sText, &xt);
      
      cairo_move_to (cr, X-xt.width/2, Y);
      cairo_show_text (cr, sText);
      
      // end the stroke
      cairo_stroke (cr);
      
      
      // Draw the gauge title below
      cairo_set_font_size (cr, 20.0);
      
      cairo_text_extents(cr, mTitle.c_str(), &xt);
      cairo_move_to (cr, X-xt.width/2, Y+150.0);
      cairo_show_text (cr, mTitle.c_str());
      
      // end the stroke
      cairo_stroke (cr);
   }
   
   private:
   
   const uint32_t mMin, mMax;
   const std::string mTitle;
   const std::string mUnits;
   
   uint32_t mValue = 0;
};
