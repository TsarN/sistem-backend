#include <iostream>
#include <vector>

std::vector<std::vector<int> > ml (5000, std::vector<int> (5000));

int main()
{
    for (int i = 0; i < 5000; ++i)
        std::cout << ml[i][i] << " ";
    return 0;
}
