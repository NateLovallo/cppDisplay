#pragma once

struct _cairo;
typedef struct _cairo cairo_t;

class IRenderable
{
   public:
   virtual void Render(cairo_t* cr) = 0;
   
   void SetPosition(double x, double y)
   {
      X = x;
      Y = y;
   }
   protected:
   double X = 0;
   double Y = 0;
};

