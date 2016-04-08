#include <iostream>
#include <algorithm>

const int n = 30;

int main()
{
    int a[n] = {1324, 8083, 4411, 8380, 5712, 5955, 5140, 6540, 3463, 4273, 6049, 8590, 7169, 9430, 1977, 6282, 6379, 8270, 6551, 4285, 9296, 1726, 7031, 4449, 954, 4804, 2943, 3001, 2727, 8745};
    std::sort(a, a + n);
    for (int i = 0; i < n; ++i)
        std::cout << a[i] << " ";
        std::cout << std::endl;
    return 0;
}
