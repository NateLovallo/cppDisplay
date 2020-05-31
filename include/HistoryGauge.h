#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <vector>
#include <memory>
#include <cstdint>
#include <assert.h>
#include <functional>
#include <thread>
#include <deque>

#include "IRenderable.h"

#pragma once


class HistoryGauge : public IRenderable
{
public:
   HistoryGauge(
   const std::string& title, 
   const std::string& units,
   size_t history
   ) 
   : mTitle(title), mUnits(units), mHistory(history, 0)
   {
   }
   
   HistoryGauge() = delete;

   
   void Render(cairo_t *cr) override final
   {
      const double span = Max - Min;
      const double width = Size.X / double(mHistory.size());
      
      
      cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 0.5);
      cairo_rectangle(cr, Position.X, Position.Y, Size.X, Size.Y);
      cairo_fill (cr);

      mHistory.push_front(CurrentValue);
      mHistory.pop_back();

      size_t itemNo = 0;

      for (auto item = mHistory.end()-1; item >= mHistory.begin(); item--)
      {
         double percentage = double(*item - Min) / double(span);
         double x = Position.X + itemNo*width;
 
         double height = Size.Y * percentage;
         double y = Position.Y + Size.Y - height;

         cairo_set_source_rgba (cr, 0, 1, 0, 1);
         cairo_rectangle(cr, x, y, width-1, y+height);
         //cairo_stroke (cr);
         cairo_fill (cr);

         itemNo++;
      }

      //cairo_stroke (cr);

/*
      
      cairo_arc (cr, Position.X, Position.Y, Size.X/2.0, start*(M_PI/180.0), end*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0.1, 0.1, 0.1, 0.5);
      cairo_set_line_width (cr, Size.X/20.0);
      cairo_stroke (cr);
      
      float total = start + (end - start)*p;
      
      cairo_arc (cr, Position.X, Position.Y, Size.X/2.0, start*(M_PI/180.0), total*(M_PI/180.0));
     
      cairo_set_source_rgba (cr, 0, 1, 0, 0.5);
      cairo_set_line_width (cr, Size.X/10.0);
      cairo_stroke (cr);

      cairo_select_font_face (cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, 30.0);
      
      char sText[32];

      sprintf(sText, "%f%s", CurrentValue, mUnits.c_str());
      
      // Draw the numerical printout
      cairo_text_extents_t xt;
      cairo_text_extents(cr, sText, &xt);
      
      cairo_move_to (cr, Position.X-xt.width/2, Position.Y-xt.height/2);
      cairo_show_text (cr, sText);
      
      // end the stroke
      cairo_stroke (cr);
      
      // Draw the gauge title below
      cairo_set_font_size (cr, 20.0);
      
      cairo_text_extents(cr, mTitle.c_str(), &xt);
      cairo_move_to (cr, Position.X-xt.width/2, Position.Y+150.0-xt.height/2);
      cairo_show_text (cr, mTitle.c_str());
      
      // end the stroke
      cairo_stroke (cr);*/
   }
   
private:
   std::deque<double> mHistory;
   const std::string mTitle;
   const std::string mUnits;
};
