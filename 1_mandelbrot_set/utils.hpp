#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>

#include "png.h"

#define MAX_ITER 10000

template <typename T>
class Complex;
template <typename T>
std::ostream& operator<<(std::ostream& os, Complex<T>& rhs);
template <typename T>
Complex<T> operator+(Complex<T>& lhs, Complex<T>& rhs);
template <typename T>
Complex<T> operator*(Complex<T>& lhs, Complex<T>& rhs);

template <typename T>
class Complex {
public:
    Complex(T real, T img) : real(real), img(img) {}
    friend std::ostream& operator<<<T>(std::ostream& os, Complex<T>& rhs);
    friend Complex<T> operator+<T>(Complex<T>& lhs, Complex<T>& rhs);
    friend Complex<T> operator*<T>(Complex<T>& lhs, Complex<T>& rhs);
    float size() { return real * real + img * img; }

private:
    T real, img;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, Complex<T>& rhs) {
    os << "real = " << rhs.real << ", "
       << "img = " << rhs.img  << ", "
       << "size = " << rhs.size();
    return os;
}

template <typename T>
Complex<T> operator+(Complex<T>& lhs, Complex<T>& rhs) {
    return Complex<T>(static_cast<T>(lhs.real + rhs.real),
                      static_cast<T>(lhs.img + rhs.img));
}

template <typename T>
Complex<T> operator*(Complex<T>& lhs, Complex<T>& rhs) {
    return Complex<T>(static_cast<T>(lhs.real * rhs.real -
                                     lhs.img * rhs.img),
                      static_cast<T>(2 * lhs.real * rhs.img));
}

template <typename T>
int mandelbrot_set(Complex<T> c0) {
    int iter_count = 0;
    Complex<T> c(0, 0);
    while (c.size() <= 4.0 && iter_count < MAX_ITER) {
        c = c * c;
        c = c + c0;
        ++iter_count;
    }
    return iter_count;
}

void write_png(const char* filename, const int width, const int height, const int* buffer) {
    FILE* fp = fopen(filename, "wb");
    assert(fp);
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr);
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);
    size_t row_size = 3 * width * sizeof(png_byte);
    png_bytep row = (png_bytep)malloc(row_size);
    for (int y = 0; y < height; ++y) {
        memset(row, 0, row_size);
        for (int x = 0; x < width; ++x) {
            int p = buffer[(height - 1 - y) * width + x];
            png_bytep color = row + x * 3;
            if (p != MAX_ITER) {
                if (p & 16) {
                    color[0] = 240;
                    color[1] = color[2] = p % 16 * 16;
                } else {
                    color[0] = p % 16 * 16;
                }
            }
        }
        png_write_row(png_ptr, row);
    }
    free(row);
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}