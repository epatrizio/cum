#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/vector.h"
#include "umachine.h"

int op_code_from_word(uint32_t word)
{
    return (word >> 28) & 0xf;
}

base_command base_command_from_word(uint32_t word)
{
    base_command bc;
    bc.op = op_code_from_word(word);
    bc.a = (word >> 6) & 0b111;
    bc.b = (word >> 3) & 0b111;
    bc.c = word & 0b111;

    return bc;
}

load_command load_command_from_word(uint32_t word)
{
    load_command lc;
    lc.r = (word >> 25) & 0b0000111;
    lc.value = word & 0x1ffffff;

    return lc;
}

instruction inst_create_from_word(uint32_t word)
{
    instruction inst;
    int op_code = op_code_from_word(word);
    if (op_code == LOAD) {
        inst.is_base_cmd = false;
        inst.inst.load_cmd = load_command_from_word(word);
    }
    else {
        inst.is_base_cmd = true;
        inst.inst.base_cmd = base_command_from_word(word);
    }

    return inst;
}

void inst_exec(universal_machine *um, instruction inst)
{
if (inst.is_base_cmd) printf("%i %i %i\n", inst.inst.base_cmd.a, inst.inst.base_cmd.b, inst.inst.base_cmd.c);
//return;
    if (inst.is_base_cmd) {
        int a = inst.inst.base_cmd.a;
        int b = inst.inst.base_cmd.b;
        int c = inst.inst.base_cmd.c;
        switch (inst.inst.base_cmd.op) {
            case COND_MOVE:
                printf("COND_MOVE\n");
                if (um->reg[c] != 0) um->reg[a] = um->reg[b];
                break;
            case ARRAY_INDEX:
                printf("ARRAY_INDEX\n");
                um->reg[a] = uint32_t_vector_get(memory_component_vector_get(um->memory, um->reg[b]), um->reg[c]);
                break;
            case ARRAY_SET:
                printf("ARRAY_SET\n");
printf("%i %i %i %i\n", um->memory->size, um->reg[a], um->reg[b], um->reg[c]);
printf("size %i\n", memory_component_vector_get(um->memory, um->reg[a])->size);
                uint32_t_vector_set(memory_component_vector_get(um->memory, um->reg[a]), um->reg[b], um->reg[c]);
                break;
            case ADD:
                printf("ADD\n");
                um->reg[a] = um->reg[b] + um->reg[c];
                break;
            case MUL:
                printf("MUL\n");
                um->reg[a] = um->reg[b] * um->reg[c];
                break;
            case DIV:
                printf("DIV\n");
                um->reg[a] = um->reg[b] / um->reg[c];
                break;
            case NOT_AND:
                printf("NOT_AND\n");
                um->reg[a] = !(um->reg[b] && um->reg[c]);   // check
                break;
            case STOP:
                printf("STOP\n");
                break;
            case ALLOC:
                printf("ALLOC\n");
                uint32_t_vector mem_elt = uint32_t_vector_create(um->reg[c]);
                for (unsigned int i=0 ; i<um->reg[c] ; i++)
                    uint32_t_vector_add(mem_elt, uint32_t_vector_size(mem_elt), 0);
                int fid = 1;    // 0 chargement init
                if (int_vector_size(um->free_id) > 0)
                    fid = int_vector_remove(um->free_id, int_vector_size(um->free_id)-1);
                um->reg[b] = fid;
                memory_component_vector_add(um->memory, fid, mem_elt);
                break;
            case FREE:
                printf("FREE\n");
                uint32_t_vector_destroy(memory_component_vector_get(um->memory, um->reg[c]));
                int_vector_add(um->free_id, int_vector_size(um->free_id), um->reg[c]);
                break;
            case OUTPUT:
                printf("Output %i\n", um->reg[c]);   // ? seules des valeurs comprises entre 0 et 255 sont tolérées
                break;
            case INPUT:
                printf("Enter an integer: ");
                int input;
                scanf("%d", &input);
                um->reg[c] = input;         // todo EOF
                break;
            case LOAD_EXEC:
                printf("LOAD_EXEC %i %i %i\n", um->reg[a], um->reg[b], um->reg[c]);
                if (um->reg[b] != 0) {
                    uint32_t_vector source = memory_component_vector_get(um->memory, um->reg[b]);
                    size_t len = sizeof(memory_component) * uint32_t_vector_size(source);
                    //size_t len = sizeof(source);
                    // reconstruction
                    uint32_t_vector dest = NULL;
                    memcpy(&dest, &source, len);
                    memory_component_vector_set(um->memory, 0, dest);
                }
                um->pc = um->reg[c];
                break;
            default:
                printf("op_code %i unsupported!\n", inst.inst.base_cmd.op);
                break;
        }
    }
    else {
printf("load:%i %i\n", inst.inst.load_cmd.r, inst.inst.load_cmd.value);
        um->reg[inst.inst.load_cmd.r] = inst.inst.load_cmd.value;
    }
}

universal_machine *um_create()
{
    memory_component_vector mem = memory_component_vector_create(MEMORY_COMPONENT_SIZE);

    // On créé juste le 0 pour la mémoire de base - le case ALLOC gère le reste
    uint32_t_vector mem_elt = uint32_t_vector_create(MEMORY_INITIAL_SIZE);
    memory_component_vector_add(mem, 0, mem_elt);

    universal_machine *um = (universal_machine *) malloc(sizeof(universal_machine));
    um->memory = mem;
    um->free_id = int_vector_create(0);

    return um;
}

void um_destroy(universal_machine *um)
{
    // le case FREE gère le destroy des autres plateaux hors 0
    uint32_t_vector_destroy(memory_component_vector_get(um->memory, 0));

    memory_component_vector_destroy(um->memory);
    int_vector_destroy(um->free_id);
    free(um);
}

void um_load_word(universal_machine *um, uint32_t word)
{
    uint32_t_vector mem_elt = memory_component_vector_get(um->memory, 0);
    uint32_t_vector_add(mem_elt, uint32_t_vector_size(mem_elt), word);
}

void um_exec(universal_machine *um)
{
    //int cpt = 0;

    uint32_t_vector mem_elt = memory_component_vector_get(um->memory, 0);
    uint32_t nb_instr = uint32_t_vector_size(mem_elt);
//printf("%li %li\n", mem_elt->capacity, mem_elt->size);
    while (um->pc < nb_instr) {
//printf("%i - %i\n", cpt++, um->pc);
        uint32_t word = uint32_t_vector_get(mem_elt, um->pc);
        instruction inst = inst_create_from_word(word);
        um->pc++;
        inst_exec(um, inst);
    }
}

void um_start(char *file_name)
{
    FILE* in_file = fopen(file_name, "rb");
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
    fclose(in_file);

    um_exec(um);
    um_destroy(um);
}

vector_init_fct(uint32_t)
vector_init_fct(memory_component)
