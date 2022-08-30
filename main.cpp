#include <iostream>
#include <vector>
#include <png.h>

using namespace std;

void readFromPNG(const char *filename, vector<vector<vector<float>>>& image) {
    int width, height;

    png_byte color_type, bit_depth;
    png_bytep *row_pointers = nullptr;

    FILE *fileStream;
    fileStream = fopen(filename, "rb");
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
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);




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
            image[0][i][j] = px[0];
            image[1][i][j] = px[1];
            image[2][i][j] = px[2];

        }
    }
    png_destroy_read_struct(&png, &info, nullptr);

}

void writeToPNG(const char *filename, vector<vector<vector<float>>>& image) {
    int width = (int)image[0].size();
    int height = (int)image[0][0].size();
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
    for(int i=0; i<height; i++) {
        row_pointers[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }
    for (int i=0;i<height;i++) {
        png_bytep row = row_pointers[i];
        for (int j = 0; j < width; j++) {
            png_bytep px = &(row[j * 4]);
            px[0] = (uint8_t) image[0][i][j];
            px[1] = (uint8_t) image[1][i][j];
            px[2] = (uint8_t) image[2][i][j];
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

int main() {
    cout << "Hello World"<<endl;
    vector<vector<vector<float>>> image1(3,vector<vector<float>>
            (256,vector<float>(256)));
    vector<vector<vector<float>>> image2(3,vector<vector<float>>
            (256,vector<float>(256)));
    vector<vector<vector<float>>> flow(2,vector<vector<float>>
            (256,vector<float>(256)));
    vector<vector<float>> mask(256,vector<float>(256));

    readFromPNG("./img1.png", image1);
    readFromPNG("./img2.png", image2);


    writeToPNG("A.png", image1);
    writeToPNG("B.png", image2);
    return 0;
}
