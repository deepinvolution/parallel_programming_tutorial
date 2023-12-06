#include <iostream>

#define INF 10000

template <typename T>
class Complex;
template <typename T>
std::ostream& operator<<(std::ostream& os, Complex<T>& rhs);
template <typename T>
Complex<T> operator*(Complex<T>& lhs, Complex<T>& rhs);

template <typename T>
class Complex {
public:
    Complex(T real, T img) : real(real), img(img) {}
    friend std::ostream& operator<<<T>(std::ostream& os, Complex<T>& rhs);
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
Complex<T> operator*(Complex<T>& lhs, Complex<T>& rhs) {
    return Complex<T>(static_cast<T>(lhs.real * rhs.real -
                                     lhs.img * rhs.img),
                      static_cast<T>(lhs.real * rhs.img +
                                     lhs.img * rhs.real));
}

template <typename T>
int mandelbrot_set(T x, T y) {
    Complex<T> point(x, y);
    int res = 0;
    while (point.size() <= 4.0 && res < INF) {
        point = point * point;
        // std::cout << point << std::endl;
        res++;
    }
    return res;
}