/*
* PROJECT		: IAD - Assignment 2 - File Transfer
* FILENAME		: FlowControl.h
* PROGRAMMER	: Austin Che
* DATE			: 2017/02/28
* DESCRIPTION	: Header file contains the FlowControl class definition.
* CREDIT		:
*/
#include <stdio.h>

#ifndef __FLOWCONTROL_H__
#define __FLOWCONTROL_H__


//=============================
// CONSTANTS
//=============================
#define kDeltaTime		1.0f / 30.0f
#define kSendRate		1.0f / 30.0f
#define kTimeOut		5.0f


/**
* \class FlowControl
* \brief
*/
class FlowControl
{
private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;


public:

	FlowControl(void)
	{
		printf("flow control initialized\n");
		Reset();
	}

	void Reset(void)
	{
		mode = Bad;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}

	void Update(float deltaTime, float rtt)
	{
		const float RTT_Threshold = 250.0f;

		if (mode == Good)
		{
			if (rtt > RTT_Threshold)
			{
				printf("*** dropping to bad mode ***\n");
				mode = Bad;
				if (good_conditions_time < 10.0f && penalty_time < 60.0f)
				{
					penalty_time *= 2.0f;
					if (penalty_time > 60.0f)
						penalty_time = 60.0f;
					printf("penalty time increased to %.1f\n", penalty_time);
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}

			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;

			if (penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f)
			{
				penalty_time /= 2.0f;
				if (penalty_time < 1.0f)
					penalty_time = 1.0f;
				printf("penalty time reduced to %.1f\n", penalty_time);
				penalty_reduction_accumulator = 0.0f;
			}
		}

		if (mode == Bad)
		{
			if (rtt <= RTT_Threshold)
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;

			if (good_conditions_time > penalty_time)
			{
				printf("*** upgrading to good mode ***\n");
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = Good;
				return;
			}
		}
	}

	float GetSendRate(void)
	{
		return mode == Good ? 30.0f : 10.0f;
	}


	static void wait_seconds(float seconds)
	{
		Sleep((int)(seconds * 1000.0f));
	}
};


#endif // ! __FLOWCONTROL_H__