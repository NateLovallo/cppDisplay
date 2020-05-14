#include "src/Renderer.h"
#include "src/Mpu9250.h"

int main(int argc, char* args[])
{
   Mpu9250 m;
   Renderer r;

   
   ArcGauge ag(0, 32768);
   r.AddItem(&ag);
   
   for (int i = 0; i < 10000; i++)
   {
      ag.Update(m.SampleAccelerometer());
   }
	
   return 0;
}

