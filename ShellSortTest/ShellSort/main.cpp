#include <stdio.h>
#include "ShellSort.h"

// コマンドライン引数をソートしてprintする
int main(int argc, char *argv[])
{
    return ShellSort::sort_print(argc, argv);
}
