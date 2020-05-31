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
         cairo_rectangle(cr, x, y, width-1, height);
         cairo_fill (cr);

         itemNo++;
      }
   }
   
private:
   std::deque<double> mHistory;
   const std::string mTitle;
   const std::string mUnits;
};
