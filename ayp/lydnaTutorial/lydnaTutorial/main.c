//
//  main.c
//  lydnaTutorial
//
//  Created by Kunal Mishra on 8/18/15.
//  Copyright (c) 2015 Kunal Mishra. All rights reserved.
//

#include <stdio.h>
#include <math.h>


void printHelloWorld() {
    printf("Hello World!\n");
}

//This will not work
void incPointer(int *p) {
    p += 1;
}

//Must pass in &ogPointer for this to work, as it dereferences then advances the pointer
void incPointer2(int **p) {
    *p += 1;
}

void explorePointers() {
    int A[3] = {1, 2, 3};
    int* q = A;
    incPointer(q);
    printf("%d\n", *q);
}

void pointersAndArrays() {
    int arr[] = {2, 4, 6, 8};
    int *p = arr;
    int **pp = &p;
    
    (*pp)++;
    (*(*pp))++;
    printf("%d\n", *p);
    
}

int hailstone(int n) {
    int count = 0;
    
    while(n != 1) {
        count += 1;
        if (n % 2 == 0)
            n /= 2;
        else
            n = 3*n + 1;
        printf("%d\n", n);
    }
    return count;
}



int main() {
    printf("Hello, World!\n");
    
    //Escape characters
    printf("Thrilled to meet you, my name is \"Kunal\"\n");
    
    //Values and placeholders (asks what the value of a + b is)
    int a = 5, b = 3;
    printf("What is the value of %d + %d?\n", a, b);
    
    //Standard input (character)
    printf("Type a letter:\n");
    //char input = getchar();
    //printf("You typed %c", input);
    
    //Standard input (string)
    char name[15];
    //scanf("%s", name);          //Scans until first whitespace char into the name variable
    
    char instructions[64];
    //fgets(instructions, 64, stdin);     //Scans even past whitespace
    
    //Using library functions (must declare math above)
    float r;
    r = sqrtf(2.0);
    printf("%f\n", r);
    
    //Calling a function (prototyped above main and defined below)
    printHelloWorld();
    
    //Playing with pointers
    explorePointers();
    
    pointersAndArrays();
    
    //Quick hailstone problem
    hailstone(10);
    
    return 0;
}