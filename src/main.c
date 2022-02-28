#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/vector.h"
#include "umachine.h"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        um_start(argv[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        printf("[error] No input file for the Universal Machine, exit!\n");
        exit(EXIT_FAILURE);
    }
}
