//
//  ShellSort.cpp
//  ShellSort
//
//  Created by tomotaka.yamasaki on 2018/03/01.
//  Copyright © 2018年 tomotaka.yamasaki. All rights reserved.
//

#include "ShellSort.h"
#include <string.h>
#include <stdlib.h>

// 引数をソートして返す
// a ソートする配列
// size 配列のサイズ
void ShellSort::shell_sort(int a[], int size)
{
    int i, j;
    int h = 1;

    do {
        h = h * 3 + 1;
    } while (h <= size);

    do {
        h /= 3;
        for (i = 0; i < size; i++)
        {
            int v = a[i];
            for (j = i; j >= h && a[j - h] > v; j -= h)
                a[j] = a[j - h];
            if (i != j)
                a[j] = v;
        }
    } while (h != 1);
}


// 引数となる文字列をソートして標準出力へprintする
// argc 文字列の個数
// argv 文字列へのポインタ配列
int ShellSort::sort_print(int argc, char *argv[])
{
    int *a;
    int i;

    a = (int *)malloc((argc - 1) * sizeof(int));
    for (i = 0; i < argc - 1; i++)
        a[i] = atoi(argv[i + 1]);

    shell_sort(a, argc);

    printf("Output : ");
    for (i = 0; i < argc - 1; i++)
        printf("%d ", a[i]);

    printf("\n");

    free(a);

    return 0;
}
