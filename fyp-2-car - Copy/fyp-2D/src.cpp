
#include<opencv/cvaux.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv/cxcore.h>
#include <time.h>

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>

#include<Windows.h>
using namespace std;

//------Constants------
#define PI 3.14159265;

//------Constants------
int main()
{
    
    //---------------------Veriables------------------
    
    CvSize size640x480=cvSize(640,480);
    CvSize size640x480_v1=cvSize(640,480);
    CvCapture* p_capWebcam;
    IplImage* p_imgOriginal;
    IplImage* p_imgProcessed;
    IplImage* p_imgProcessed_v1;
    CvMemStorage* p_strStorage;
    CvMemStorage* p_strStorage_v1;
    CvSeq* p_seqCircles;
    CvSeq* p_seqCircles_v1;
    float* p_fltxyRadius;
    float* p_fltxy;
    int i,j;
	int dest=0;
	int src=0;
				
    char charCheckForEscKey;
    //---------------------Veriables------------------
    //-------------------communication------------------
			// Setup serial port connection and needed variables.
	HANDLE hSerial = CreateFile(L"COM4", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hSerial !=INVALID_HANDLE_VALUE)
    {
        printf("Port opened! \n");

        DCB dcbSerialParams;
        GetCommState(hSerial,&dcbSerialParams);

        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.Parity = NOPARITY;
        dcbSerialParams.StopBits = ONESTOPBIT;

        SetCommState(hSerial, &dcbSerialParams);
	}
	else
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("Serial port doesn't exist! \n");
		}

		printf("Error while setting up serial port! \n");
	}

	char outputChars[] = "c";
	DWORD btsIO;
	//-------------------------------------------------
    //-------------Capture Cam Vid Stream--------------
    p_capWebcam=cvCaptureFromCAM(1);
    
    //to serr cam frame size
    cvSetCaptureProperty( p_capWebcam, CV_CAP_PROP_FRAME_WIDTH, 640 );
    cvSetCaptureProperty( p_capWebcam, CV_CAP_PROP_FRAME_HEIGHT, 480 );
    
    //-------------Stream Checking--------------
    if(p_capWebcam==NULL)
    {
        printf("Error:Capture is NULL\n");
        getchar();
        return(-1);
    }
	else
	{
		cout<<"Camera Opened... n working...";
	//-------------CV Processing and Orignal Win Defination--------------
    cvNamedWindow("Original",CV_WINDOW_AUTOSIZE);
    cv::moveWindow("Original", 700, 20);
    cvNamedWindow("Processed Red",CV_WINDOW_AUTOSIZE);
    cv::moveWindow("Processed Red", 20, 20);
    cvNamedWindow("Processed Blue",CV_WINDOW_AUTOSIZE);
    cv::moveWindow("Processed Blue", 20, 350);
    
    p_imgProcessed=cvCreateImage(size640x480,IPL_DEPTH_8U,1);
    p_imgProcessed_v1=cvCreateImage(size640x480_v1,IPL_DEPTH_8U,1);
    while(1)
    {
        p_imgOriginal=cvQueryFrame(p_capWebcam);
        if(p_imgOriginal==NULL)
        {
            printf("Error: Frame is NULL\n");
            getchar();
            break;
            
        }
        //red rgb ;-)
        cvInRangeS(p_imgOriginal,CV_RGB(140,0,0),CV_RGB(256,100,100),p_imgProcessed);
        p_strStorage=cvCreateMemStorage(0);
        cvSmooth(p_imgProcessed,p_imgProcessed,CV_GAUSSIAN,9,9);
        
        p_seqCircles=cvHoughCircles(p_imgProcessed,p_strStorage,CV_HOUGH_GRADIENT,2,p_imgProcessed->height/4,100,50,10,400);
        
        //blue rgb :-)
        cvInRangeS(p_imgOriginal,CV_RGB(0,0,120),CV_RGB(100,100,180),p_imgProcessed_v1);
        p_strStorage_v1=cvCreateMemStorage(0);
        cvSmooth(p_imgProcessed_v1,p_imgProcessed_v1,CV_GAUSSIAN,9,9);
        
        p_seqCircles_v1=cvHoughCircles(p_imgProcessed_v1,p_strStorage_v1,CV_HOUGH_GRADIENT,2,p_imgProcessed_v1->height/4,100,50,10,400);
        
		
		//contoure creatrion for red ball
        for(i=0; i<p_seqCircles->total;i++)
        {
            p_fltxyRadius=(float*)cvGetSeqElem(p_seqCircles,i);
            printf("\n red: Position x=%f | y=%f | r=%f", p_fltxyRadius[0],p_fltxyRadius[1],p_fltxyRadius[2]);
            cvCircle(p_imgOriginal,cvPoint(cvRound(p_fltxyRadius[0]),cvRound(p_fltxyRadius[1])),3,CV_RGB(0,255,0),CV_FILLED);
            cvCircle(p_imgOriginal, cvPoint(cvRound(p_fltxyRadius[0]),cvRound(p_fltxyRadius[1])), cvRound(p_fltxyRadius[2]), CV_RGB(0,255,0),3);
        }
        
        //conture creation for blue ball
        for(j=0; j<p_seqCircles_v1->total;j++)
        {
            p_fltxy=(float*)cvGetSeqElem(p_seqCircles_v1,j);
            printf("\n blue: Position x=%f | y=%f | r=%f", p_fltxy[0],p_fltxy[1],p_fltxy[2]);
            cvCircle(p_imgOriginal,cvPoint(cvRound(p_fltxy[0]),cvRound(p_fltxy[1])),3,CV_RGB(0,255,0),CV_FILLED);
            cvCircle(p_imgOriginal, cvPoint(cvRound(p_fltxy[0]),cvRound(p_fltxy[1])), cvRound(p_fltxy[2]), CV_RGB(0,255,0),3);
        }
        cvShowImage("Original",p_imgOriginal);
        cvShowImage("Processed Red",p_imgProcessed);
        cvShowImage("Processed Blue",p_imgProcessed_v1);
		if(p_seqCircles_v1->total==1 && p_seqCircles->total==1)
		{
			p_fltxy=(float*)cvGetSeqElem(p_seqCircles_v1,1);
			p_fltxyRadius=(float*)cvGetSeqElem(p_seqCircles,1);
			dest=p_fltxy[0];
			src=p_fltxyRadius[0];
			cout<<"\a";
		}
		if(dest!=0 && src!=0)
		{
			if (src>=dest)
			{
				outputChars[0] = 'H';
				WriteFile(hSerial, outputChars, strlen(outputChars), &btsIO, NULL);
			
			}
			
			else
			{
				outputChars[0] = 'Q';
				WriteFile(hSerial, outputChars, strlen(outputChars), &btsIO, NULL);
			}


		}
		cout<<"\n Destnation"<<dest<<"\n";
        
        
        cvReleaseMemStorage(&p_strStorage);
        charCheckForEscKey=cvWaitKey(10);
        if(charCheckForEscKey==27)
            break;
    }
    cvReleaseCapture(&p_capWebcam);
    cvDestroyWindow("Original");
    cvDestroyWindow("Processed");
	}
    return(0);
}
