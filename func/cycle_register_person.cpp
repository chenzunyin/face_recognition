#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "RSFaceSDK.h"
#include <pthread.h>
#include <opencv2/opencv.hpp>
using namespace cv; 

//global parameter
#define LEN 1000
const char * APPID = "b59c5851eea4408f833f24b31b1fae9e";
const char * APPSECRET = "6ee59303feec5c73ca7f518bee7846ca410d9e53";
const char * face_db = "./database/czy_face.db";

RSHandle lm = NULL;
RSHandle fd = NULL;
RSHandle fr = NULL;

Mat global_cvface;

rs_face  *rsface_array = NULL;
rs_face_feature global_facefeature;

int global_ret = 0;
int global_cyccnt = 0;
int global_facecnt = 0;
//end global parameter

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
        global_cvface = imread(inputFile, CV_LOAD_IMAGE_UNCHANGED);

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
        } else if (global_facecnt > 1) {
		printf("find %d face, not make feature\n", global_facecnt);	
        	releaseFaceDetectResult(rsface_array, global_facecnt);
		return -1;
	} else {
                printf("run face detect find  %d  face !\n", global_facecnt);
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

/*
        //print feature
        printf("fr:");
        for (global_numcnt = 0; global_numcnt < 512; global_numcnt++) {
                printf("  %f  ",global_facefeature.feature[global_numcnt]);
        }
        printf("\n----------------************--------------------------\n");
*/

        //add person to facelib
        global_ret = rsRecognitionPersonCreate(fr, &global_facefeature);

        if (global_ret) {
                printf("register person ok!,total person in face.db is %d\n", global_ret);
        }

        return global_ret;
}


//read dir func
int ReadFileList (char * dirpath)
{
	DIR *dir;
	struct dirent *ptr;
	char base[LEN];
	char filepath[LEN];

	if ((dir = opendir(dirpath)) == NULL) {
		perror("Open dir error...");
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		global_cyccnt++;
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
			continue; //current or parrent dir
		} else if (ptr->d_type == 8) { //file
			printf("file d_name: %s/%s \n", dirpath, ptr->d_name);
			memset(filepath, '\0', sizeof(filepath));
			strcpy(filepath, dirpath);
			strcat(filepath, "/");
			strcat(filepath, ptr->d_name);
        		global_ret = run_face_add(filepath);
        		if (global_ret == 0) {
                		printf("get person feature error!\n");
                		return -1;
        		}		
		} else if (ptr->d_type == 10) { //link file
			printf("link d_name: %s/%s \n", dirpath, ptr->d_name);
		} else if (ptr->d_type == 4) { //dir
			memset(base, '\0', sizeof(base));
			strcpy(base, dirpath);
			strcat(base, "/");
			strcat(base, ptr->d_name);
			ReadFileList(base);
		}
	}

	closedir(dir);

	return 0;
}

int main(int argc, char * argv[])
{
	int get_c = 0;
	char * inputpath = NULL;

	if ((get_c = getopt(argc, argv, ":f:")) != -1) {
		switch (get_c) {
			case 'f':
				inputpath = optarg;
				printf("dir path is %s\n", inputpath);
				break;
			default:
				printf("use: -f for input dir path\n");
				return -1;
		}
	} else {
		printf("use: -f for input dir path\n");
		return -1;
	}

	if (inputpath == NULL) {
		printf("input dir path is NULL\n");
		return -1;
	}

	global_ret = init_func();

	if(global_ret) {
		printf("init func error!\n");
		return -1;
	}

	global_ret = ReadFileList(inputpath);

	if (global_ret) {
		printf("read file fail, ret = %d\n", global_ret);
	} else {
		printf("read file ok!\n");
	}

	printf("total cycle is %d\n", global_cyccnt);

	free_func();

	return 0;
}

