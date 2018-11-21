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
//const char * save_feature = "./save_opencv_data";
//FILE *save_fp = NULL;

RSHandle lm = NULL;
RSHandle fd = NULL;
RSHandle fr = NULL;

Mat global_cvface;
rs_face  *rsface_array = NULL;
rs_face_feature global_facefeature;

int global_numcnt = 0;
int global_ret = 0;
int global_facecnt = 0;
float global_norm = 0;

#define LEN 512
#define NORMALIZE_EPS 0.00001
//end for global parameter

//init function
int init_func(void)
{
	//init liscense manager
	global_ret = rsInitLicenseManager(&lm, APPID, APPSECRET);

	if (lm == NULL) {
		printf("Liscense Manager init error ! errno = %d\n", global_ret);
		return -1;
	} else {
		printf("Liscense Manager init ok!\n");
	}

	//init face detect
	rsInitFaceDetect(&fd, lm);

	if (fd == NULL) {
		printf("Face Detect init error\n");
		return -1;
	} else {
		printf("Face Detect init ok!\n");
	}

	//init face recognition
	rsInitFaceRecognition(&fr, lm, face_db);	

	if (fr == NULL) {
		printf("Face Recognition init error\n");
		return -1;
	} else {
		printf("Face Recognition init ok!\n");
	}

	return 0;
}

void free_func(void)
{
	rsUnInitFaceRecognition(&fr);	
	rsUnInitFaceDetect(&fd);
	rsUnInitLicenseManager(&lm);
}

int run_face_add(char * inputFile)
{
	//opencv load image
	//global_cvface = imread(inputFile, CV_LOAD_IMAGE_UNCHANGED);
	global_cvface = imread(inputFile);

	//debug
	/*
	printf("cols: %d \n", global_cvface.cols);
	printf("rows: %d \n", global_cvface.rows);
	printf("step: %d \n", global_cvface.step);

        save_fp = fopen(save_feature, "wb");
        if (save_fp == NULL) {
                printf("cannot open %s\n", save_feature);
                return -1; 
        }
        fwrite(global_cvface.data, global_cvface.cols*global_cvface.rows*3, 1, save_fp);
        fclose(save_fp);
	*/
	//end debug

	//run face detect
	global_ret = rsRunFaceDetect(fd, global_cvface.data,
		PIX_FORMAT_BGR888,
		global_cvface.cols,
		global_cvface.rows,
		global_cvface.step,
		RS_IMG_CLOCKWISE_ROTATE_0,
		&rsface_array,
		&global_facecnt);

	
	if (global_ret) {
		printf("run face detect func error ! errno = %d\n", global_ret);
		return global_ret;
	} else if (global_facecnt > 1){
		printf("too many face in pic, face num is %d, func will return!", global_facecnt);
		return 0;
	}

	//run face get feature
	global_ret = rsRecognitionGetFaceFeature(fr, global_cvface.data,
        	PIX_FORMAT_BGR888,
        	global_cvface.cols,
        	global_cvface.rows,
        	global_cvface.step,
        	rsface_array,
        	&global_facefeature
        );
	
	if (global_ret) {
		printf("run face recognition error ! errno =%d \n", global_ret);
		return global_ret;
	} else {
		printf("run face recognition ok\n");
	}

	//free result face array
	releaseFaceDetectResult(rsface_array, global_facecnt);

	//print feature
	/*
	printf("fr:");
	for (global_numcnt = 0; global_numcnt < 512; global_numcnt++) {
		printf("%f, ",global_facefeature.feature[global_numcnt]);
	}
	printf("\n----------------************--------------------------\n");
	*/

	//guiyihua
	for (global_numcnt = 0; global_numcnt < 512; global_numcnt++) {
		global_norm += global_facefeature.feature[global_numcnt] * global_facefeature.feature[global_numcnt];
	}
	for (global_numcnt = 0; global_numcnt < 512; global_numcnt++) {
		global_facefeature.feature[global_numcnt] /= sqrt(global_norm + NORMALIZE_EPS);
	}

	//add to file
	/*
	save_fp = fopen(save_feature, "wb");
        if (save_fp == NULL) {
                printf("cannot open %s\n", save_feature);
                return -1; 
        }
        fwrite(global_facefeature.feature, LEN*sizeof(float), 1, save_fp);
        fclose(save_fp);
	*/

	//add person to facelib
	global_ret = rsRecognitionPersonCreate(fr, &global_facefeature);

	if (global_ret) {
		printf("register person ok!,total person in face.db is %d\n", global_ret);
	}
	
	return global_ret;
}


int main(int argc, char * argv[])
{
	int get_c;
    	char *inputFile = NULL;

	//get commandline parameter
    	while ((get_c = getopt(argc, argv, ":f:")) != -1) {   
        	switch (get_c) {
        		case 'f':
            			inputFile = optarg;
            			printf("image file path: %s\n", inputFile);
            			break;
        		default :
            			printf("Usage : \n"
                   			"-f    : image file path. \n");
            			return -1;
        	}
    	}

    	if (inputFile == NULL) {
        	printf("invalid image file path!!!\n");
        	return -1;
    	}

	//init func
	global_ret = init_func();

	if (global_ret) {
		printf("algo init error!\n");
		return -1;
	}

	global_ret = run_face_add(inputFile);
	
	if (global_ret == 0) {
		printf("get person feature error!\n");
		return -1;
	}

	free_func();

	return 0;
}

