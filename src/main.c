#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/vector.h"
#include "umachine.h"

int main(int argc, char *argv[])
{
    FILE* in_file = fopen("sandmark.umz", "rb");
    if (!in_file) {
        perror("[sandmark.umz fopen]");
        exit(EXIT_FAILURE);
    }

    universal_machine *um = um_create();
    uint32_t word;

    while (fread(&word,sizeof(word),1,in_file) != 0)
    {
        um_load_word(um, word);
    }

    um_exec(um);

    um_destroy(um);
    fclose(in_file);

    return EXIT_SUCCESS;
}
