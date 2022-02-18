#ifndef UNIVERSAL_MACHINE_H
#define UNIVERSAL_MACHINE_H

#define MEMORY_INITIAL_SIZE 2048
#define MEMORY_COMPONENT_SIZE 256

enum op_code {
   COND_MOVE,
   ARRAY_INDEX,
   ARRAY_SET,
   ADD,
   MUL,
   DIV,
   NOT_AND,
   STOP,
   ALLOC,
   FREE,
   OUTPUT,
   INPUT,
   LOAD_EXEC,
   LOAD,
};

typedef struct _base_command {
   int op;
   int a;
   int b;
   int c;
} base_command;

typedef struct _load_command {
   int r;
   int value;
} load_command;

typedef struct _instruction {
   bool is_base_cmd;
   union _inst {
      base_command base_cmd;
      load_command load_cmd;
   } inst;
} instruction;

// Mémoire = Vector de vector de uint32_t (qui sont les mots de base à décoder)
vector_init(uint32_t)
typedef uint32_t_vector memory_component;
vector_init(memory_component)

typedef struct _universal_machine {
   memory_component_vector memory;
   uint32_t reg[8];
   int pc;
   int_vector free_id;
} universal_machine;

void um_start(char*);

#endif
