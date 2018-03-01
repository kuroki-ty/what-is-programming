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
#include "ShellSort.h"

@interface ShellSort_Test : XCTestCase
@end

@implementation ShellSort_Test

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

- (void)checkShellSort:(NSArray*) testData
            answerData:(NSArray*) answerData
                  size:(int) size {
    int array[size];
    int answer[size];

    for (int i = 0; i < size; i++) {
        array[i] = [[testData objectAtIndex:i] intValue];
        answer[i] = [[answerData objectAtIndex:i] intValue];
    }
    ShellSort::shell_sort(array, size);
    [self log:array size:size];
    [self log:answer size:size];
    XCTAssertTrue([self isMatchedArrays:array answer:answer size:size]);
}

- (void)testShellSort {
    struct TestCase {
        NSArray* testData;
        NSArray* answerData;
        int size;
    };

    std::vector<TestCase> testList = {
        {[NSArray arrayWithObjects:@1, nil], [NSArray arrayWithObjects:@1, nil], 1},
        {[NSArray arrayWithObjects:@1, @1, @1, nil], [NSArray arrayWithObjects:@1, @1, @1, nil], 3},
    };

    for (auto testData : testList) {
        [self checkShellSort:testData.testData answerData:testData.answerData size:testData.size];
    }
}

@end
