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
        FILE* in_file = fopen(argv[1], "rb");
        if (!in_file) {
            perror("[error] Universal Machine input file fopen");
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
    } else {
        printf("[error] No input file for the Universal Machine, exit!\n");
        exit(EXIT_FAILURE);
    }
}
