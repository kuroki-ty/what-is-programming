#include <iostream>
#include <vector>

struct WorkData
{
    enum {
        MESSAGE_TERMINATE,
        MESSAGE_CALCSUM,
    } message;
    std::vector<int> vec;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
