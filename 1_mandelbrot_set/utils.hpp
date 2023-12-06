#include <iostream>

template <typename T>
class Complex;
template <typename T>
std::ostream& operator<<(std::ostream& os, Complex<T>& rhs);
template <typename T>
Complex<T>& operator*(Complex<T>& lhs, Complex<T>& rhs);

template <typename T>
class Complex {
public:
    Complex(T real, T img) : real(real), img(img) {}
    friend std::ostream& operator<<<T>(std::ostream& os, Complex<T>& rhs);
    friend Complex<T>& operator*<T>(Complex<T>& lhs, Complex<T>& rhs);

private:
    T real, img;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, Complex<T>& rhs) {
    os << rhs.real << " " << rhs.img;
    return os;
}

template <typename T>
Complex<T>& operator*(Complex<T>& lhs, Complex<T>& rhs) {
    T real = lhs.real * rhs.real - lhs.img * rhs.img;
    T img = lhs.real * rhs.img + lhs.img * rhs.real;
    lhs.real = real, lhs.img = img;
    return lhs;
}

template <typename T>
int mandelbrot_set(T x, T y) {
    Complex<T> point(x, y);
    std::cout << point << std::endl;
    std::cout << point * point << std::endl;
    return 0;
}