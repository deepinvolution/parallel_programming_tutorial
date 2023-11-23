#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <algorithm>
#include "odd_even_sort.hpp"

TEST(OddEvenSort, SingleThread) {
    int n = 256;
    std::vector<int> nums(n), golden(n);
    srand(0);
    for (int i = 0; i < n; i++)
        nums[i] = golden[i] = rand() % 10;
    odd_even_sort(nums);
    std::sort(golden.begin(), golden.end());
    for (int i = 0; i < n; i++)
        EXPECT_EQ(nums[i], golden[i]) << "Vectors x and y differ at index " << i;
}

TEST(OddEvenSort, MultiThread) {
    int n = 256;
    std::vector<int> nums(n), golden(n);
    srand(0);
    for (int i = 0; i < n; i++)
        nums[i] = golden[i] = rand() % 10;
    odd_even_sort_mpi(nums);
    std::sort(golden.begin(), golden.end());
    for (int i = 0; i < n; i++)
        EXPECT_EQ(nums[i], golden[i]) << "Vectors x and y differ at index " << i;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}