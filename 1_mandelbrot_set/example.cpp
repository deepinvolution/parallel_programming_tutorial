#include "utils.hpp"
#include <vector>

int main(int argc, char** argv) {
    float x0 = 0.0, x1 = 2.0, y0 = 0.0, y1 = 2.0, m = 10.0, n = 10.0;
    float gx = (x1 - x0) / m, gy = (y1 - y0) / n;
    std::vector<std::vector<int>> plot(m, std::vector<int>(n));
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            plot[i][j] = mandelbrot_set(x0 + gx * i, y0 + gy * j);
    std::cout << "plot:" << std::endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            std::cout << plot[i][j] << " ";
        std::cout << std::endl;
    }
    return 0;
}