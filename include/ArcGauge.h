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
   ArcGauge(
   const std::string& title, 
   const std::string& units
   ) 
   : mTitle(title), mUnits(units) 
   {
   }
   
   ArcGauge() = delete;

   
   void Render(cairo_t *cr) override final
   {
      Rect center = Center();
      {
         cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 1);
         cairo_rectangle(cr, Position.X, Position.Y-20, Size.X, 20);
         cairo_fill (cr);

         cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
         cairo_set_font_size (cr, 10.0);
         
         char sText[64];

         sprintf(sText, "%s: %2.2f%s", mTitle.c_str(), CurrentValue, mUnits.c_str());
         
         // Draw the numerical printout
         cairo_set_source_rgba (cr, 1, 1, 1, 1);
         cairo_move_to (cr, Position.X, Position.Y);
         cairo_show_text (cr, sText);
         
         // end the stroke
         cairo_stroke (cr);
      }


      cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 0.5);
      cairo_rectangle(cr, Position.X, Position.Y, Size.X, Size.Y);
      cairo_fill (cr);

      double span = Max - Min;
      float p = float(CurrentValue - Min) / float(span);
      
      float start = 120.0f;
      float end = 60+360;

      cairo_arc (cr, center.X, center.Y, Size.X/2.50, start*(M_PI/180.0), end*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 1);
      cairo_set_line_width (cr, Size.X/20.0);
      cairo_stroke (cr);
      
      float total = start + (end - start)*p;
      
      cairo_arc (cr, center.X, center.Y, Size.X/2.50, start*(M_PI/180.0), total*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0, 1, 0, 1);
      cairo_set_line_width (cr, Size.X/10.0);
      cairo_stroke (cr);

      cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, 30.0);
      
      char sText[32];

      sprintf(sText, "%2.0f%s", CurrentValue, mUnits.c_str());
      
      // Draw the numerical printout
      cairo_text_extents_t xt;
      cairo_text_extents(cr, sText, &xt);
      
      cairo_move_to (cr, center.X-xt.width/2, center.Y-xt.height/2);
      cairo_show_text (cr, sText);
      
      // end the stroke
      cairo_stroke (cr);
   }
   
   private:
   
   const std::string mTitle;
   const std::string mUnits;
};
