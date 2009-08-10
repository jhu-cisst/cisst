#include <stdio.h>
#include <conio.h>

/* Sensable's includes */
#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

HDSchedulerHandle gCallbackHandle = 0;

/* Global handles to devices */
HHD hHD[4];

void mainLoop();
HDCallbackCode HDCALLBACK Controller(void *pUserData);

FILE *datafile;
#define LOGFILENAME "log.txt"

double kp; /**< Proportional control gain */
int ff; /**< Proportional control gain */

/******************************************************************************
 Main function.
******************************************************************************/
int main(int argc, char* argv[])
{  
    HDErrorInfo error;
	
	hHD[0] = hdInitDevice("Omni1");
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        _getch();
        return -1;
    }

	
	hHD[1] = hdInitDevice("Omni2");
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        _getch();
        return -1;
    }

    hHD[2] = hdInitDevice("Omni3");
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        _getch();
        return -1;
    }

    hHD[3] = hdInitDevice("Omni4");
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        _getch();
        return -1;
    }

	/* open a file to log data --- "w+" : overwrite file if existing */
	datafile = fopen( LOGFILENAME, "w+" );
	if( datafile == NULL )
	{
        fprintf(stderr, "\nFailed to open file for data logging.\n");
        _getch();
        return -1;
    }

	kp = 0.15;
	ff = 0;

    /* Schedule the haptic callback function for continuously monitoring the
       button state and rendering the anchored spring force. */
    gCallbackHandle = hdScheduleAsynchronous(
        Controller, 0, HD_MAX_SCHEDULER_PRIORITY);

    /* no force output */
	hdMakeCurrentDevice(hHD[0]);	hdEnable(HD_FORCE_OUTPUT);
	hdMakeCurrentDevice(hHD[1]);	hdEnable(HD_FORCE_OUTPUT);
    hdMakeCurrentDevice(hHD[2]);	hdEnable(HD_FORCE_OUTPUT);
    hdMakeCurrentDevice(hHD[3]);	hdEnable(HD_FORCE_OUTPUT);


    /* Start the haptic rendering loop. */
	hdSetSchedulerRate( 500 /* Hz */); 
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        _getch();
        return -1;
    }

    /* Start the main application loop. */
    mainLoop();

    /* Cleanup by stopping the haptics loop, unscheduling the asynchronous
       callback, disabling the device. */
    hdStopScheduler();
    hdUnschedule(gCallbackHandle);
    hdDisableDevice(hHD[0]);
    hdDisableDevice(hHD[1]);
    hdDisableDevice(hHD[2]);
    hdDisableDevice(hHD[3]);

	fclose(datafile);

    return 0;
}


/******************************************************************************
 Main loop.  
 Detects and interprets keypresses. 
******************************************************************************/
inline void print_status(void)
{
	if(ff)
		printf("\rKp = %f, Force feedback: ON ", kp);
	else
		printf("\rKp = %f, Force feedback: OFF", kp);
}

void mainLoop()
{
    int keypress;

	printf("Cyber-Physical Systems Demonstration\n");
	printf("July 9, 2009\n");
	printf("Johns Hopkins University\n\n\n");
	print_status();
    ff = ~ff;
    while (1)
    {       
        /* Check for keyboard input. */
        if (_kbhit())
        {
            keypress = _getch();
            keypress = toupper(keypress);
            
            if (keypress == '+')
            {
				kp *= 1.25;
				print_status();
            }

			if (keypress == '-')
            {
				kp *= 0.8;
				print_status();
            }

			if (keypress == 'F')
            {
				ff = ~ff;
				print_status();
				//printf("ff = %i\n", ff);
            }


            if (keypress == 'Q')
            {
                return;
            }
        }

        /* Check if the main scheduler callback has exited. */
        if (!hdWaitForCompletion(gCallbackHandle, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            _getch();
            return;
        }
    }
}


/******************************************************************************
 * Scheduler callback for reading the robot position.
 *****************************************************************************/
HDCallbackCode HDCALLBACK Controller(void *pUserData)
{
	int idx;
	HDErrorInfo error;
    hduVector3Dd joints[4], gimbal[4], force, force2;
	hduVector3Dd p1, p2, p3, p4;
    HDint nCurrentButtons[2];
	HDint updateRate;

    hdBeginFrame(hHD[0]);
    hdBeginFrame(hHD[1]);
    hdBeginFrame(hHD[2]);
    hdBeginFrame(hHD[3]);

		for( idx=0; idx < 3; idx++ )
		{
			hdMakeCurrentDevice(hHD[idx]);
			/* read the buttons */
			//hdGetIntegerv(HD_CURRENT_BUTTONS, nCurrentButtons + idx );

			/* sample and log the position, joint angles and scheduler update rate */
//			hdGetDoublev(HD_CURRENT_POSITION, position[idx] );
			hdGetDoublev(HD_CURRENT_JOINT_ANGLES, joints[idx]);
			hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, gimbal[idx] );
			hdGetIntegerv(HD_UPDATE_RATE, &updateRate);
		}
			
		hdMakeCurrentDevice(hHD[0]); hdGetDoublev(HD_CURRENT_POSITION, p1 );
		hdMakeCurrentDevice(hHD[1]); hdGetDoublev(HD_CURRENT_POSITION, p2 );
        hdMakeCurrentDevice(hHD[2]); hdGetDoublev(HD_CURRENT_POSITION, p3 );
        hdMakeCurrentDevice(hHD[3]); hdGetDoublev(HD_CURRENT_POSITION, p4 );


		//if ((nCurrentButtons[0] & HD_DEVICE_BUTTON_1) != 0 )
		{	
			//HDdouble mtr_torq[3];

			hduVecSubtract(force, p2, p1);
			hduVecScaleInPlace(force, kp); // Proportional Gain

            //HDdouble mtr_torq2[3];

			hduVecSubtract(force2, p4, p3);
			hduVecScaleInPlace(force2, kp); // Proportional Gain

			/*
			hduVecScaleInPlace(position[0], -0.5);
			joints[0][1] = joints[0][1] - 0.7854;
			joints[0][2] = joints[0][2] - 0.7854;
			hduVecScaleInPlace(joints[0], 500.0);
			*/
			/*
			mtr_torq[0] = -joints[0][0];
			mtr_torq[1] = -joints[0][1];
			mtr_torq[2] = -joints[0][2];
			*/

			if(ff ) //& joints[0][0] <=0 )
            {
				hdMakeCurrentDevice(hHD[0]);	hdSetDoublev(HD_CURRENT_FORCE, force);
			    hdMakeCurrentDevice(hHD[2]);	hdSetDoublev(HD_CURRENT_FORCE, force2);
			    
            }//hdSetDoublev(HD_CURRENT_JOINT_TORQUE, mtr_torq );

			hdMakeCurrentDevice(hHD[1]);	hdSetDoublev(HD_CURRENT_FORCE, -force);
			hdMakeCurrentDevice(hHD[3]);	hdSetDoublev(HD_CURRENT_FORCE, -force2);

			//printf( "%f, %f, %f\n", position[0], position[1], position[2] );
			/*fprintf( datafile, "%i, %f, %f, %f, %f, %f, %f, %f, %f, %f \n", 
				updateRate, position[idx][0], position[idx][1], position[idx][2],
				joints[idx][0], joints[idx][1], joints[idx][2],
				gimbal[idx][0], gimbal[idx][1], gimbal[idx][2]  ); */
		}

    hdEndFrame(hHD[0]);
    hdEndFrame(hHD[1]);
    hdEndFrame(hHD[2]);
    hdEndFrame(hHD[3]);

    /* Check if an error occurred while attempting to render the force */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        if (hduIsSchedulerError(&error))
		{
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}
