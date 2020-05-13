#include "src/Renderer.h"
#include "src/Mpu9250.h"

int main(int argc, char* args[])
{
   Mpu9250 m;
   Renderer r;
   r.RenderLoop();
	
   return 0;
}

