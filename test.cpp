#include "src/Renderer.h"
#include "src/Mpu9250.h"

int main(int argc, char* args[])
{
   Mpu9250 m;
   Renderer r;

   
   ArcGauge ag(0, 32768);
   ag.SetPosition(200, 200);
   
   ArcGauge tg(0, 100);
   tg.SetPosition(600, 200);
   
   r.AddItem(&ag);
   r.AddItem(&tg);
   
   for (int i = 0; i < 10000; i++)
   {
      ag.Update(m.SampleAccelerometer());
      tg.Update(m.SampleTemperature());
   }
	
   return 0;
}

