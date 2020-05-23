#include "Renderer.h"
#include "ArcGauge.h"
#include "Mpu9250.h"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[])
{
   using namespace std::literals::chrono_literals;
   Mpu9250 m;
   Renderer r;

   
   ArcGauge ag(0, 32768, "Z Acc", "cts");
   ag.SetPosition(200, 200);
   
   ArcGauge tg(0, 100, "Temperature", "ºF");
   tg.SetPosition(600, 200);
   
   r.AddItem(&ag);
   r.AddItem(&tg);
   
   
   bool run = true;
   
   r.AddHandler(SDL_QUIT, [&run](SDL_Event e){run = false;});
   
   while (run)
   {
      ag.Update(m.SampleAccelerometer());
      tg.Update(m.SampleTemperature());
      r.Draw();
      std::this_thread::sleep_for(100ms);
   }
	
   return 0;
}

