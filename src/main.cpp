#include "Renderer.h"
#include "ArcGauge.h"
#include "Mpu9250.h"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* args[])
{
   Mpu9250 m;
   Renderer r;

   
   ArcGauge ag(0, 32768, "Z Accel", "cts");
   ag.SetPosition(200, 200);
   
   ArcGauge tg(0, 100, "Temperature", "ºF");
   tg.SetPosition(600, 200);
   
   r.AddItem(&ag);
   r.AddItem(&tg);
   
   while (!r.Input())
   {
      ag.Update(m.SampleAccelerometer());
      tg.Update(m.SampleTemperature());
      r.Draw();
   }
	
   return 0;
}

