#include "axx.hpp"

using std::cout;
using std::endl;

template<int N>
struct X : X<N - 1>
{
    X() { cout << " " << N; }
};

template<>
struct X<0> {};

int main()
{
    std::string str("1, 2, 3, 4, 5, 6, 2, 3, 4, 5");
    std::regex r(R"---(\d+)---");
    std::sregex_iterator begin(str.begin(), str.end(), r);
    std::sregex_iterator end;
    for (auto it = begin; it != end; ++it)
        cout << it->str() << endl;

    return 0;
}
