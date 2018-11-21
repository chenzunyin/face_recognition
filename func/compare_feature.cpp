#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "RSFaceSDK.h"
#include <pthread.h>
#include <opencv2/opencv.hpp>
using namespace cv;

//global parameter
const char * APPID = "b59c5851eea4408f833f24b31b1fae9e";
const char * APPSECRET = "6ee59303feec5c73ca7f518bee7846ca410d9e53";
const char * face_db = "./database/czy_face.db";

RSHandle lm = NULL;
RSHandle fd = NULL;
RSHandle fr = NULL;

Mat global_cvsourceface;
Mat global_cvcompareface;

rs_face  *rsface_array_source = NULL;
rs_face  *rsface_array_compare = NULL;

rs_face_feature global_sourcefacefeature;
rs_face_feature global_comparefacefeature;

int global_facecnt = 0;
int global_ret = 0;
float global_sim = 0;
//end for global parameter

int init_func(void)
{
	//init liscense manager handle
	global_ret = rsInitLicenseManager(&lm, APPID, APPSECRET);

	if (global_ret) {
		printf("init func run error = %d\n", global_ret);
		return -1;
	}	

	if (lm == NULL) {
		printf("liscense manager init error\n");
		return -1;
	}
	else
		printf("liscense manager init ok!\n");

	//init face detect handle
	rsInitFaceDetect(&fd, lm);

	if (fd == NULL) {
		printf("FaceDetect init error\n");
		return -1;
	}
	else
		printf("FaceDetect init ok!\n");

	//init face recognition handle
	rsInitFaceRecognition(&fr, lm, face_db);	

	if (fr == NULL) {
		printf("FaceRecognition init error\n");
		return -1;
	}
	else
		printf("FaceRecognition init ok!\n");

	return 0;
}

void free_func(void)
{
	//free handle
	rsUnInitFaceRecognition(&fr);
	rsUnInitFaceDetect(&fd);
	rsUnInitLicenseManager(&lm);
}

int compare_func(void)
{

/*********************source face*************************************/

	//run face detect
	global_ret = rsRunFaceDetect(fd, global_cvsourceface.data,
		PIX_FORMAT_BGR888,
		global_cvsourceface.cols,
		global_cvsourceface.rows,
		global_cvsourceface.step,
		RS_IMG_CLOCKWISE_ROTATE_0,
		&rsface_array_source,
		&global_facecnt);
	
	if (global_ret) {
		printf("run source face detect error\n");
		return -1;
	} else if (global_facecnt > 1) {
		printf("too many face , find %d in pic\n", global_facecnt);
		return global_facecnt;
	}

	//run face recognition
	global_ret = rsRecognitionGetFaceFeature(fr, global_cvsourceface.data,
        	PIX_FORMAT_BGR888,
        	global_cvsourceface.cols,
        	global_cvsourceface.rows,
        	global_cvsourceface.step,
        	rsface_array_source,
        	&global_sourcefacefeature
        );
	
	if (global_ret) {
		printf("run source face recognition func return %d\n", global_ret);
		return global_ret;
	} else {
		printf("run source face recognition ok\n");
	}

	//free face detect result
	releaseFaceDetectResult(rsface_array_source, global_facecnt);

/***********************compare face************************************************************/

        global_ret = rsRunFaceDetect(fd, global_cvcompareface.data,
                PIX_FORMAT_BGR888,
                global_cvcompareface.cols,
                global_cvcompareface.rows,
                global_cvcompareface.step,
                RS_IMG_CLOCKWISE_ROTATE_0,
                &rsface_array_compare,
                &global_facecnt);


        if (global_ret) {
                printf("run compare face detect error\n");
		return -1;
        } else if (global_facecnt > 1) {
		printf("too many face , find %d in pic\n", global_facecnt);
		return global_facecnt;
        }

        global_ret = rsRecognitionGetFaceFeature(fr, global_cvcompareface.data,
                PIX_FORMAT_BGR888,
                global_cvcompareface.cols,
                global_cvcompareface.rows,
                global_cvcompareface.step,
                rsface_array_compare,
                &global_comparefacefeature
        );

        if (global_ret) {
                printf("run compare face recognition func return %d\n", global_ret);
                return global_ret;
        } else {
                printf("run compare face recognition ok\n");
        }

	//free face detect result
	releaseFaceDetectResult(rsface_array_compare, global_facecnt);

/**************************compare result********************************************************/

	//compare two face similairy
	global_sim = rsRecognitionFaceVerification(fr, &global_sourcefacefeature, &global_comparefacefeature);

	printf("\n**********************************************\n");
	printf("source img VS compare img sim is %f \n", global_sim);
	printf("\n**********************************************\n");

	return 0;
}

int main(int argc, char * argv[])
{
	int get_c;
    	char *sourceFile = NULL;
    	char *compareFile = NULL;

	//get command line parameter
    	while ((get_c = getopt(argc, argv, ":s:c:")) != -1) {   
        	switch (get_c) {
        		case 's':
            			sourceFile = optarg;
            			printf("source image file path: %s\n", sourceFile);
            			break;
        		case 'c':
            			compareFile = optarg;
            			printf("compare image file path: %s\n", compareFile);
            			break;
        		case '?':
            			printf("Usage : \n"
                   			"-s    : source image file path. \n"
                   			"-c    : compare image file path. \n");
            			return -1;
        	}
	}

    	if (sourceFile == NULL) {
        	printf("invalid source image file path !!!\n");
        	return -1;
    	} else if (compareFile == NULL) {
		printf("invalid compare image file path !!!\n");
		return -1;
	}

	//opencv load image
	//global_cvsourceface = imread(sourceFile, CV_LOAD_IMAGE_UNCHANGED);
	//global_cvcompareface = imread(compareFile, CV_LOAD_IMAGE_UNCHANGED);
	global_cvsourceface = imread(sourceFile);
	global_cvcompareface = imread(compareFile);

	global_ret = init_func();

	if (global_ret) {
		printf("algo init error!\n");
		return -1;
	}

	global_ret = compare_func();

	if (global_ret) {
		printf("compare func error!\n");
		return -1;
	}

	free_func();	
	
	return 0;
}
