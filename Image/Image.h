//
// Created by tom on 30.08.22.
//

#ifndef DIFFUSIONINOF_IMAGE_H
#define DIFFUSIONINOF_IMAGE_H

#include <string>
#include <png.h>
#include <pngconf.h>
#include <pnglibconf.h>
#include <iostream>
#include "Derivative.h"

template <class T>
class Image {

public:
    // constructors
    inline Image();
    inline explicit Image(int channels);
    inline explicit Image(int width, int height, int channels);
    // copy constructor
    Image(const Image<T>& aCopyFrom);
    // Destructor
    inline ~Image();

    // read PNG into Image
    inline void readFromPNG(const std::string& filename);
    inline void writeToPNG(const std::string& filename);

    // Utilities
    inline int getWidth() const;
    inline int getHeight() const;
    inline int getDepth() const;

    // operators
    inline T& operator() (int d, int x, int y);
    inline T& operator() (int x, int y);

private:
    int width, height, depth;
    float *data;

};


// IMPLEMENTATION----------------------------------------------------------------------------



template<class T>
Image<T>::Image(const Image<T> &aCopyFrom) {
    if (aCopyFrom.data == nullptr) data= nullptr;
    else{
        width = aCopyFrom.width;
        height = aCopyFrom.height;
        depth = aCopyFrom.depth;
        int size = width * height * 2;
        for(int i=0;i<size;i++){
            data[i] = aCopyFrom.data[i];
        }
    }
}

template<class T>
Image<T>::Image():width(0),height(0),depth(0) {
    data= nullptr;
}

template<class T>
Image<T>::Image(int channels):width(0),height(0),depth(channels) {
    data = nullptr;
}

template<class T>
Image<T>::Image(int width, int height, int channels):width(width),height(height), depth(channels) {
    data = (float*)malloc(sizeof(T)*(width+2)*(height+2) * channels);
}

// Destructor
template<class T>
Image<T>::~Image() {
    delete [] data;
}

// Getter and Setter
template<class T>
int Image<T>::getWidth() const {
    return width;
}

template<class T>
int Image<T>::getHeight() const {
    return height;
}

template<class T>
int Image<T>::getDepth() const {
    return depth;
}

// Operators
template<class T>
T& Image<T>::operator()(int d, int x, int y) {   // read-write accessor
    return data[x + width * (y + d * height)];
}

template<class T>
T& Image<T>::operator()(int x, int y) {
    return operator()(0,x,y);
}

// IO functions
template<class T>
void Image<T>::writeToPNG(const std::string& filename) {
    png_bytep *row_pointers = nullptr;
    FILE *fileStream;
    
    fileStream = fopen(filename.c_str(), "wb");
    if(!fileStream) abort();
    
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              nullptr, nullptr, nullptr);
    if (!png) abort();
    
    png_infop info = png_create_info_struct(png);
    if (!info) abort();
    
    png_init_io(png, fileStream);
    
    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
            png,
            info,
            width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
    
    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*height);
    for(int i=0; i<height; i++) {
    row_pointers[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }
    for (int j=1;j<height+1;j++) {
        png_bytep row = row_pointers[(j-1)];
        for (int i = 1; i < width+1; i++) {
            png_bytep px = &(row[(i-1) * 4]);
            for (int d=0; d < depth; d++) {
                px[d] = (uint8_t) operator()(d,i,j);
            }
            px[3] = 255;
        }
    png_write_row(png, row);
    }
    png_write_image(png, row_pointers);
    png_write_end(png, nullptr);
    
    for(int i = 0; i < height; i++) {
        free(row_pointers[i]);
    }
    free(row_pointers);
    
    fclose(fileStream);
    
    png_destroy_write_struct(&png, &info);

}

template<class T>
void Image<T>::readFromPNG(const std::string& filename) {
    FILE *fileStream;
    png_byte color_type, bit_depth;
    png_bytep *row_pointers = nullptr;
    fileStream = fopen("img1.png", "rb");

    fileStream = fopen(filename.c_str(), "rb");
    if (fileStream == nullptr) abort();

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             nullptr, nullptr, nullptr);
    if (!png) abort();
    png_infop info = png_create_info_struct(png);

    if(!info) abort();

    png_init_io(png, fileStream);

    png_read_info(png, info);

    width = (int)png_get_image_width(png, info);
    height = (int)png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);
    if (color_type == 0) {
        depth = 1;
    }
    else {
        depth = 3;
    }
    data = (float*)malloc(sizeof(float) * (width+2) * (height+2) * depth);

    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*height);

    for(int i=0; i<height; i++) {
        row_pointers[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);

    fclose(fileStream);

    for (int j=1;j<height+1;j++) {
        png_bytep row = row_pointers[(j-1)];
        for (int i = 1; i < width+1; i++) {
            png_bytep px = &(row[(i-1) * 4]);
            for(int d=0; d < depth; d++){
                operator()(d,i,j) = px[d];
            }
        }
    }
    // Set boundary with reflective padding
    for (int y=0;y<height+2;y++){
        operator()(0,0,y) = operator()(0,1,y);
        operator()(1,0,y) = operator()(1,1,y);
        operator()(0,width+2,y) = operator()(0,width+1,y);
        operator()(1,width+2,y) = operator()(1,width+1,y);
    }
    for (int x=0;x<width+2;x++){
        operator()(0,x,0) = operator()(0,x,1);
        operator()(1,x,0) = operator()(1,x,1);
        operator()(0,x,height+2) = operator()(0,x,height+1);
        operator()(1,x,height+2) = operator()(1,x,height+1);
    }
    png_destroy_read_struct(&png, &info, nullptr);

}




#endif //DIFFUSIONINOF_IMAGE_H
