//
//  ShellSort_Test.m
//  ShellSort-Test
//
//  Created by tomotaka.yamasaki on 2018/03/01.
//  Copyright © 2018年 tomotaka.yamasaki. All rights reserved.
//

#import <XCTest/XCTest.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include "ShellSort.h"

@interface ShellSort_Test : XCTestCase
@end

@implementation ShellSort_Test

struct TestCase {
    NSMutableArray* testData;
    NSMutableArray* answerData;
    int size;
};

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)log:(int*) array
       size:(int) size {
    NSLog(@"array ");
    for (int i = 0; i < size; i++) {
        NSLog(@"%d ", array[i]);
    }
}

- (bool)isMatchedArrays:(int*) array
                 answer:(int*) answer
                   size:(int)  size {
    return memcmp(array, answer, sizeof(int) * size) == 0;
}

- (void)checkShellSort:(NSMutableArray*) testData
            answerData:(NSMutableArray*) answerData
                  size:(int) size {
    int array[size];
    int answer[size];

    for (int i = 0; i < size; i++) {
        array[i] = [[testData objectAtIndex:i] intValue];
        answer[i] = [[answerData objectAtIndex:i] intValue];
    }
    ShellSort::shell_sort(array, size);
//    [self log:array size:size];
//    [self log:answer size:size];
    XCTAssertTrue([self isMatchedArrays:array answer:answer size:size]);
}

- (void)testShellSortIntBoundaryValue {
    std::vector<TestCase> testList = {
        {[NSMutableArray arrayWithObjects:@1, nil], [NSMutableArray arrayWithObjects:@1, nil], 1},
        {[NSMutableArray arrayWithObjects:@1, @1, @1, nil], [NSMutableArray arrayWithObjects:@1, @1, @1, nil], 3},
        {[NSMutableArray arrayWithObjects:@1, @3, @2, nil], [NSMutableArray arrayWithObjects:@1, @2, @3, nil], 3},
        {[NSMutableArray arrayWithObjects:@1, @-1, @2, nil], [NSMutableArray arrayWithObjects:@-1, @1, @2, nil], 3},
        {[NSMutableArray arrayWithObjects:@INT_MAX, @0, @1, nil], [NSMutableArray arrayWithObjects:@0, @1, @INT_MAX, nil], 3},
        {[NSMutableArray arrayWithObjects:@0, @INT_MIN, @1, nil], [NSMutableArray arrayWithObjects:@INT_MIN, @0, @1, nil], 3},
        {[NSMutableArray arrayWithObjects:@9, @8, @7, @6, @5, @4, @3, @2, @1, @0, nil], [NSMutableArray arrayWithObjects:@0, @1, @2, @3, @4, @5, @6, @7, @8, @9, nil], 10},

        // C0(命令網羅)
        {[NSMutableArray arrayWithObjects:@3, @2, @1, @0, nil], [NSMutableArray arrayWithObjects:@0, @1, @2, @3, nil], 4},

        // C1(分岐網羅)
        {[NSMutableArray arrayWithObjects:[NSNull null] , nil], [NSMutableArray arrayWithObjects:[NSNull null], nil], 0},
        {[NSMutableArray arrayWithObjects:@3, @2, @1, @0, nil], [NSMutableArray arrayWithObjects:@0, @1, @2, @3, nil], 4},

        // C2(条件網羅)
        {[NSMutableArray arrayWithObjects:[NSNull null] , nil], [NSMutableArray arrayWithObjects:[NSNull null], nil], 0},
        {[NSMutableArray arrayWithObjects:@3, @2, @1, @0, nil], [NSMutableArray arrayWithObjects:@0, @1, @2, @3, nil], 4},
        {[NSMutableArray arrayWithObjects:@1, @1, @1, @1, nil], [NSMutableArray arrayWithObjects:@1, @1, @1, @1, nil], 4},
    };

    for (auto testData : testList) {
        [self checkShellSort:testData.testData answerData:testData.answerData size:testData.size];
    }
}

- (void)testShellSortArraySize {
    std::vector<int> sizeList = {
        0,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
    };

    for (int size : sizeList) {
        TestCase testData;
        testData.testData = [NSMutableArray array];
        testData.answerData = [NSMutableArray array];
        testData.size = size;

        for (int i = 0; i < size; i++) {
            [testData.testData addObject:[NSNumber numberWithInt:testData.size - i - 1]];
            [testData.answerData addObject:[NSNumber numberWithInt:i]];
        }

        [self checkShellSort:testData.testData answerData:testData.answerData size:testData.size];
    }
}

@end
