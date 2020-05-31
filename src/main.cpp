#include "Renderer.h"
#include "ArcGauge.h"
#include "HistoryGauge.h"
#include "Mpu9250.h"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[])
{
   using namespace std::literals::chrono_literals;
   Mpu9250 m;
   Renderer r;

   
   //ArcGauge ag(0, 32768, "Z Acc", "cts");
   //ag.SetPosition({200, 200}, {200, 200});
   HistoryGauge th("Temperature", "ºF", 20);
   th.SetRange(0, 100);
   th.SetPosition({200, 200}, {200, 200});
   r.AddItem(&th);
   
   ArcGauge tg("Temperature", "ºF");
   tg.SetPosition({600, 200}, {200, 200});
   tg.SetRange(0, 100);
   r.AddItem(&tg);
   
   bool run = true;
    
   r.AddHandler(SDL_QUIT, [&run](SDL_Event e){run = false;});
   
   while (run)
   {
      th.SetValue(m.SampleTemperature());
      tg.SetValue(m.SampleTemperature());
      r.Draw();
      std::this_thread::sleep_for(1000ms);
   }
	
   return 0;
}

