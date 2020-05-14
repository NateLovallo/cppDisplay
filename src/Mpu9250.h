
#pragma once

#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "MPU9250_RegisterMap.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class Mpu9250
{
public:
	///////////////////////////////////////////////////////////////////////////////
   Mpu9250(bool debug = true) :
		mFd(open("/dev/i2c-1", O_RDWR))
   {
		assert(mFd >= 0);
      
      if (debug)
      {
         mDebugOut = stderr;
      }
      else
      {
         mDebugOut = fopen("/dev/null", "RW");
      }

		Reset();

		unsigned char ID;
		int r = I2CRead(MPU9250_WHO_AM_I, 1, &ID);
		assert(r == 0);
		(void)0;

		fprintf(mDebugOut, "WHOAMI reported \t0x%x\n", ID);

		if (ID != 0x71)
		{
			assert(0);
		}
		

		SetAccelFsr();

		SetSampleRate();

		std::this_thread::sleep_for(100ms);

   }

	///////////////////////////////////////////////////////////////////////////////
   ~Mpu9250()
   {
		close(mFd);
   }

	///////////////////////////////////////////////////////////////////////////////
   int SampleAccelerometer()	
   {
		unsigned char data[6] = {};
		int r = I2CRead(MPU9250_ACCEL_XOUT_H, 6, data);
		assert(r == 0);
		(void)0;

		int x = (data[0] << 8) | data[1];
		int y = (data[2] << 8) | data[3];
		int z = (data[4] << 8) | data[5];
		//fprintf(mDebugOut, "X \t%d \n", x);
		//fprintf(mDebugOut, "Y \t%d \n", y);
		//fprintf(mDebugOut, "Z \t%d \n", z);

		return z;
	}

	///////////////////////////////////////////////////////////////////////////////
	void SampleTemperature()
	{
		unsigned char data[2] = {};
		int r = I2CRead(MPU9250_TEMP_OUT_H, 2, data);
		assert(r == 0);
		(void)0;
		int temp = (data[0] << 8) | data[1];

		// data sheet says TEMP_degC = ((TEMP_OUT  RoomTemp_Offset) / Temp_Sensitivity) + 21degC
		// RoomTemp_Offset is not defined anywhere in reg map or data sheet
		// TDK driver uses 0?
		// TDK driver uses 321 for temp sens

		temp = (temp - 0) / (321) + 21;

		// dungarees frankenstein
		temp = (temp * 9.0f / 5.0f) + 32.0f;

		//fprintf(mDebugOut, "Temp \t%d \n", temp);
	}

	///////////////////////////////////////////////////////////////////////////////
	void Reset()
	{

		unsigned char data = PWR_MGMT_1_H_RESET;

		int r = I2CWrite(MPU9250_PWR_MGMT_1, 1, &data);
		assert(r == 0);
		(void)r;

		std::this_thread::sleep_for(100ms);

		data = 0;
		r = I2CWrite(MPU9250_PWR_MGMT_1, 1, &data);
		assert(r == 0);
		(void)r;

		data = 0;
		r = I2CWrite(MPU9250_PWR_MGMT_2, 1, &data);
		assert(r == 0);
		(void)r;
	}

	///////////////////////////////////////////////////////////////////////////////
	void SetAccelFsr()
	{
		// TODO make configurable

		unsigned char data = (01 << 3); // 4G range
		int r = I2CWrite(MPU9250_ACCEL_CONFIG, 1, &data);
		assert(r == 0);
		(void)r;

		// Bypass filters
		data = ACCEL_CONFIG_2_ACCEL_FCHOICE_B;
		// 0 does bypass, but its inverted
		r = I2CWrite(MPU9250_ACCEL_CONFIG_2, 1, &data);
		assert(r == 0);
		(void)r;
	}

	
	///////////////////////////////////////////////////////////////////////////////
	void SetSampleRate ()
	{
		// TODO make configurable

		unsigned char data = 19; // 20 Mhz / (1 + 19)
		int r = I2CWrite(MPU9250_SMPLRT_DIV, 1, &data);
		assert(r == 0);
		(void)r;
	}

private:


	///////////////////////////////////////////////////////////////////////////////
	int I2CWrite(
		unsigned char reg_addr,
		unsigned char length, 
		unsigned char* data)
	{
		int retval = 0;
		//fprintf(mDebugOut, "write \t%x \t%u\n", reg_addr, length);

		unsigned char send_data[256 + 1] = {};

		assert(mFd != -1);

		struct i2c_msg msg;

		// All the messages in a write need to be sent in one packet with not start condition
		// after the address
		//
		// So prepend the addr
		// 
		send_data[0] = reg_addr;

		memcpy(&send_data[1], data, length);

		length++;

		msg.addr = mSlaveAddress;
		msg.flags = 0; //RDWR?
		msg.len = length;
		msg.buf = send_data;

		struct i2c_rdwr_ioctl_data msgset;
		msgset.nmsgs = 1;
		msgset.msgs = &msg;

		if (ioctl(mFd, I2C_RDWR, &msgset) < 0)
		{
			fprintf(mDebugOut, "Oh shit write %d %s\n", errno, strerror(errno));
			retval = -1;
		}

		return retval;
	}

	///////////////////////////////////////////////////////////////////////////////
	int I2CRead(
		unsigned char reg_addr,
		unsigned char length, 
		unsigned char* data)
	{
		int retval = 0;

		//fprintf(mDebugOut, "read \t%x \t%u\n", reg_addr, length);

		assert(mFd != -1);

		// when doing a read, write the address first then read the stuff as a read
		struct i2c_msg msgs[2];

		// register address
		msgs[0].addr = mSlaveAddress;
		msgs[0].flags = 0; //RDWR?
		msgs[0].len = 1;
		msgs[0].buf = &reg_addr;

		// actual data
		msgs[1].addr = mSlaveAddress;
		msgs[1].flags = I2C_M_RD;
		msgs[1].len = length;
		msgs[1].buf = data;

		struct i2c_rdwr_ioctl_data msgset;
		msgset.nmsgs = 2;
		msgset.msgs = msgs;

		if (ioctl(mFd, I2C_RDWR, &msgset) < 0)
		{
			fprintf(mDebugOut, "Oh shit read %d %s\n", errno, strerror(errno));
			retval = -1;
		}

		return retval;
	}



private:

   FILE* mDebugOut = NULL;

	int mFd = -1;
	const unsigned char mSlaveAddress = 0x68;

};
