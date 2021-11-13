#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"
#define N 255

void show_px(uint8_t r,uint8_t g,uint8_t b){
	printf("px: (%x,%x,%x)\n",r & 0xff,g & 0xff,b & 0xff);
}

void show_metadata(int w, int h, int chs){
	printf("\n\n--- Showing metadata ---\n");
	printf("Width: %dpx \n", w);
	printf("Height: %dpx \n", h);
	printf("Channels: %d \n", chs);
	printf("NumPixels: %d \n", w*h);
}

uint8_t* rgb_2_gray(uint8_t *data, int w, int h, int chs){
	// declare variables red, green and blue
	uint8_t r;
	uint8_t g;
	uint8_t b;

	// declare gray variable (one channel)
	uint8_t gray;
	uint8_t *gray_img = (uint8_t*)malloc(w*h);

	// declare time indicators
	double before = omp_get_wtime();
	for(int i=0; i < w*h; i++){
		r = *(data + chs*i);
		g = *(data + chs*i + 1);
		b = *(data + chs*i + 2);
		// show_px(r, g, b);
		gray= (r+g+b)/3;
		*(gray_img + i) = gray;
	}
	double after = omp_get_wtime();
	printf("%f seconds\n", after-before);
	return gray_img;
}

uint8_t* rgb_2_gray_parallel(uint8_t *data, int w, int h, int chs){
	int num_procs = omp_get_num_procs();
	printf("%d cores\n", num_procs);

	uint8_t r;
	uint8_t g;
	uint8_t b;

	// declare gray variable (one channel)
	uint8_t gray;
	uint8_t *gray_img = (uint8_t*)malloc(w*h);

	double before = omp_get_wtime();
	#pragma omp parallel for private(r,g,b,gray) 
	for(int i=0; i < w*h; i++){
		r = *(data + chs*i);
		g = *(data + chs*i + 1);
		b = *(data + chs*i + 2);
		// show_px(r, g, b);
		gray= (r+g+b)/3;
		*(gray_img + i) = gray;
	}
	double after = omp_get_wtime();
	printf("%f seconds\n", after - before);
	return gray_img;
}

int main(){
	// declare image data variables
	int width, height, channels;
	// input image path
	char image_path[128], gray_path[128], gray_path_parallel[128];
	printf("Image path: ");
	scanf("%s", image_path);

	// load image data
	uint8_t *img = stbi_load(image_path, &width, &height,&channels, 0);
	// if img doesnt exist
	if(!img){
		printf("Error in loading\n");
		exit(1);
	}
	printf("Gray image name: ");
	scanf("%s", gray_path);
	// load image data
	show_metadata(width, height, channels);
	
	uint8_t *gray_img;

	int pxs = width * height;
	strcpy(gray_path_parallel, gray_path);
	printf("\n\n--- Secuential RGB2GRAY ---\n");
	gray_img = rgb_2_gray(img, width,height,channels);
	printf("Saving image gray ... \n");
	stbi_write_jpg(strcat(gray_path,"_gray.jpg"),width ,height , 1 , gray_img, 100); 
	memset(gray_img, 0, pxs);

	printf("\n\n--- Parallel RGB2GRAY ---\n");
	gray_img = rgb_2_gray_parallel(img, width, height, channels);
	stbi_write_jpg(strcat(gray_path_parallel,"_gray_parallel.jpg"), width ,height , 1 , gray_img, 100); 
	printf("Saving image gray parallel... \n");
	memset(gray_img, 0, pxs);
	printf("\n\nFinished :)\n");
	return 0;
}
