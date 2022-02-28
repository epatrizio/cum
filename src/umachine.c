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
    bc.a = ((unsigned int)word >> 6) & 0b111;
    bc.b = ((unsigned int)word >> 3) & 0b111;
    bc.c = (unsigned int)word & 0b111;

    return bc;
}

load_command load_command_from_word(uint32_t word)
{
    load_command lc;
    lc.r = ((unsigned int)word >> 25) & 0b0000111;
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

bool inst_exec(universal_machine *um, instruction inst)
{
    if (inst.is_base_cmd) {
        int a = inst.inst.base_cmd.a;
        int b = inst.inst.base_cmd.b;
        int c = inst.inst.base_cmd.c;
        switch (inst.inst.base_cmd.op) {
            case COND_MOVE:
                if (um->reg[c] != 0) um->reg[a] = um->reg[b];
                break;
            case ARRAY_INDEX:
                um->reg[a] = uint32_t_vector_get(memory_component_vector_get(um->memory, um->reg[b]), um->reg[c]);
                break;
            case ARRAY_SET:
                uint32_t_vector_set(memory_component_vector_get(um->memory, um->reg[a]), um->reg[b], um->reg[c]);
                break;
            case ADD:
                um->reg[a] = um->reg[b] + um->reg[c];
                break;
            case MUL:
                um->reg[a] = um->reg[b] * um->reg[c];
                break;
            case DIV:
                um->reg[a] = um->reg[b] / um->reg[c];
                break;
            case NOT_AND:
                um->reg[a] = ~(um->reg[b] & um->reg[c]);
                break;
            case STOP:
                return false;
            case ALLOC: ;
                uint32_t_vector mem_elt = uint32_t_vector_create(um->reg[c]);
                for (unsigned int i=0 ; i<um->reg[c] ; i++)
                    uint32_t_vector_add(mem_elt, uint32_t_vector_size(mem_elt), 0);
                
                if (int_vector_size(um->free_id) > 0) {
                    int fid = int_vector_remove(um->free_id, int_vector_size(um->free_id)-1);
                    um->reg[b] = fid;
                    memory_component_vector_set(um->memory, fid, mem_elt);    
                }
                else {
                    int fid = memory_component_vector_size(um->memory);
                    um->reg[b] = fid;
                    memory_component_vector_add(um->memory, fid, mem_elt);
                }
                break;
            case FREE:
                uint32_t_vector_destroy(memory_component_vector_get(um->memory, um->reg[c]));
                int_vector_add(um->free_id, int_vector_size(um->free_id), um->reg[c]);
                break;
            case OUTPUT:
                printf("%c", um->reg[c]);
                break;
            case INPUT: ;
                char input;
                scanf("%c", &input);
                um->reg[c] = input;
                break;
            case LOAD_EXEC:
                if (um->reg[b] != 0) {
                    uint32_t_vector source = memory_component_vector_get(um->memory, um->reg[b]);
                    size_t s_size = uint32_t_vector_size(source);
                    uint32_t_vector dest = uint32_t_vector_create(MEMORY_INITIAL_SIZE);
                    for (unsigned int i=0 ; i<s_size ; i++) {
                        uint32_t_vector_add(dest, uint32_t_vector_size(dest), uint32_t_vector_get(source, i));
                    }
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
        um->reg[inst.inst.load_cmd.r] = inst.inst.load_cmd.value;
    }

    return true;
}

universal_machine *um_create()
{
    memory_component_vector mem = memory_component_vector_create(MEMORY_COMPONENT_SIZE);
    uint32_t_vector mem_elt = uint32_t_vector_create(MEMORY_INITIAL_SIZE);
    memory_component_vector_add(mem, 0, mem_elt);

    universal_machine *um = (universal_machine *) malloc(sizeof(universal_machine));
    um->memory = mem;
    for (short i=0 ; i<8 ; i++)
        um->reg[i] = 0;
    um->pc = 0;
    um->free_id = int_vector_create(0);

    return um;
}

void um_destroy(universal_machine *um)
{
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
    bool exec_in_progress = true;
    while (exec_in_progress) {
        uint32_t word = uint32_t_vector_get(memory_component_vector_get(um->memory, 0), um->pc);
        instruction inst = inst_create_from_word(word);
        um->pc++;
        exec_in_progress = inst_exec(um, inst);
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

    while ( !feof(in_file) ) {
        word = 0;
        word |= (uint32_t)fgetc(in_file);
        word <<= 8;
        word |= (uint32_t)fgetc(in_file);
        word <<= 8;
        word |= (uint32_t)fgetc(in_file);
        word <<= 8;
        word |= (uint32_t)fgetc(in_file);
        um_load_word(um, word);
    }

    fclose(in_file);

    um_exec(um);
    printf("Execution complete!\n");
    um_destroy(um);
    printf("Free memory.\n");
}

vector_init_fct(uint32_t)
vector_init_fct(memory_component)
