#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define LEN 1000

//global parameter
int global_ret = 0;
int global_cyccnt = 0;
//end global parameter

//read dir func
int ReadFileList (char * dirpath)
{
	DIR *dir;
	struct dirent *ptr;
	char base[LEN];

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
		printf(" default use: -f for input dir path\n");
		return -1;
	}

	if (inputpath == NULL) {
		printf("input dir path is NULL\n");
		return -1;
	}

	global_ret = ReadFileList(inputpath);

	if (global_ret) {
		printf("read file fail, ret = %d\n", global_ret);
	} else {
		printf("read file ok!\n");
	}

	printf("total cycle is %d\n", global_cyccnt);

	return 0;
}

