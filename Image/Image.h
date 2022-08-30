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

template <class T>
class Image {

public:
    // constructors
    inline Image();
    // copy constructor
    Image(const Image<T>& aCopyFrom);


    ~Image();


    // read PNG into Image
    void readFromPNG(const char *filename);
    void writeToPNG(const char *filename);

    // Utilities
    inline int getWidth() const;
    inline int getHeight() const;
    inline int getDepth() const;

    // operators
    operator

protected:
    T *data;
    int width, height, depth;
};


// IMPLEMENTATION----------------------------------------------------------------------------



template<class T>
Image<T>::Image(const Image<T> &aCopyFrom) {
    if (aCopyFrom.data == 0) data=0;
    else{
        std::cout<<"TODO"<<std::endl;
        data=0;
        width = aCopyFrom.width;
        height = aCopyFrom.height;
        depth = aCopyFrom.depth;
    }
}

template<class T>
Image<T>::Image() {
    width = height = depth = 0;
    data = nullptr;
}

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


// IO functions
template<class T>
void Image<T>::readFromPNG(const char *filename) {
    png_byte color_type, bit_depth;
    png_bytep *row_pointers = nullptr;

    FILE *fileStream;
    fileStream = std::fopen(filename, "rb");
    if (fileStream == nullptr) abort();

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             nullptr, nullptr, nullptr);
    if (!png) abort();
    png_infop info = png_create_info_struct(png);

    if(!info) abort();

    png_init_io(png, fileStream);

    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    depth = 3;
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    data = new T[depth*width*height];


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

    for (int i=0;i<height;i++) {
        png_bytep row = row_pointers[i];
        for (int j = 0; j < width; j++) {
            png_bytep px = &(row[j * 4]);
            data[0][i][j] = px[0];
            data[1][i][j] = px[1];
            data[2][i][j] = px[2];
        }
    }
    png_destroy_read_struct(&png, &info, nullptr);

}

template<class T>
void Image<T>::writeToPNG(const char *filename) {
    int y;
    png_bytep *row_pointers = nullptr;


    FILE *fileStream = fopen(filename, "wb");
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
    for (int i=0;i<height;i++) {
        png_bytep row = row_pointers[i];
        for (int j = 0; j < width; j++) {
            *row++ = data[0][i][j];
            *row++ = data[1][i][j];
            *row++ = data[2][i][j];
        }
        png_write_row(png, row);
    }
    png_write_image(png, row_pointers);
    png_write_end(png, nullptr);

    for(int i = 0; i < height; i++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fileStream);

    png_destroy_write_struct(&png, &info);

}




#endif //DIFFUSIONINOF_IMAGE_H
