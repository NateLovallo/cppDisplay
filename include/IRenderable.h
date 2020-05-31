
#include <assert.h>

#pragma once

struct _cairo;
typedef struct _cairo cairo_t;

class IRenderable
{
public:

   struct Rect
   {
      double X = 0.0;
      double Y = 0.0;
   };

   virtual void Render(cairo_t* cr) = 0;
   
   void SetPosition(Rect position, Rect size)
   {
      Position = position;
      Size = size;
   }

   void SetRange(double min, double max)
   {
      assert(min < max);
      Min = min;
      Max = max; 
   }

   void SetValue(double val)
   {
      CurrentValue = val;

      if (CurrentValue < Min)
      {
         CurrentValue = Min;
      } 
      
      if (CurrentValue > Max)
      {
         CurrentValue = Max;
      }
   }

protected:
   Rect Position;
   Rect Size;
   double CurrentValue = 0.0;
   double Min = 0.0;
   double Max = 1.0;
};

