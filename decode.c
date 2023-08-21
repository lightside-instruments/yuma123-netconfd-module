#include "stdio.h"
#include "jpeglib.h"

int image_decode(unsigned char* image, size_t image_size, unsigned char* rgb)
{
    int rc;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    unsigned char linebuf[1920*3]; 
    unsigned int width, height, row_stride, pixel_size;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, image, image_size);
    rc = jpeg_read_header(&cinfo, TRUE);
    if (rc != 1) {
	fprintf(stderr, "File does not seem to be a normal JPEG");
	return -1;
    }
    jpeg_start_decompress(&cinfo);
    width = cinfo.output_width;
    height = cinfo.output_height;
    pixel_size = cinfo.output_components;
    row_stride = 1920 * 3;
    //width* pixel_size;


    while (cinfo.output_scanline < cinfo.output_height) {
	unsigned char* buffer_array[1];
	unsigned char* dst;
	buffer_array[0] = linebuf;
        dst=rgb + (cinfo.output_scanline) * 1920*4;
	jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i=0;i<width;i++) {
            dst[i*4+0]=linebuf[i*3+0];
            dst[i*4+1]=linebuf[i*3+1];
            dst[i*4+2]=linebuf[i*3+2];
            dst[i*4+3]=0;
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return 0;
}
