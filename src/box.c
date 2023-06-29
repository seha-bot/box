#include "box.h"
#include "nec.h"
#include "nic.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void box_error(const char* text)
{
    printf("\033[31m");
    printf("%s\n", text);
    printf("\033[0m");
}

char* box_load(const char* path)
{
    FILE * fp = fopen(path, "r");
    if(!fp)
    {
        box_error("File error: failed to open.");
        return 0;
    }

    char* raw = 0;
    int read = 0;
    while((read = getc(fp)) != EOF)
    {
        if(read == 10) continue;
        if(read < 32 || read > 126)
        {
            nec_free(raw);
            fclose(fp);
            box_error("File error: unknown character.");
            return 0;
        }
        nec_push(raw, read);
    }
    nec_push(raw, 0);

    fclose(fp);
    return raw;
}

char* next_shard(const char** raw)
{
    if(**raw == 0) return 0;
    char* shard = 0;
    while(isspace(**raw)) (*raw)++;

    if(**raw == '(' || **raw == ')' || **raw == '{' || **raw == '}' || **raw == ';')
    {
        nec_push(shard, *(*raw)++);
        nec_push(shard, 0);
        return shard;
    }

    while(**raw != 0 && !isspace(**raw) && **raw != '(' && **raw != ')' && **raw != '{' && **raw != '}' && **raw != ';')
    {
        nec_push(shard, *(*raw)++);
    }
    nec_push(shard, 0);

    return shard;
}

char** box_shardify(const char* raw)
{
    char** shards = 0;
    char* shard = 0;

    while((shard = next_shard(&raw)))
    {
        nec_push(shards, shard);
    }
    return shards;
}

int assign_type(box_op* op)
{
    if(!op->shards)
    {
        box_error("Syntax error: empty statement.");
        return 1;
    }

    if(nec_size(op->shards) == 1)
    {
        op->type = BOX_CHECKPOINT;
    }
    else if(strcmp(op->shards[0], "if") == 0)
    {
        op->type = BOX_CONDITION;
    }
    else if(strcmp(op->shards[0], "goto") == 0)
    {
        op->type = BOX_JUMP;
    }
    else if(strcmp(op->shards[0], "print") == 0)
    {
        op->type = BOX_PRINT;
    }
    else if(op->shards[1][0] == '=')
    {
        op->type = BOX_ASSIGNMENT;
    }

    return 0;
}

box_op* box_parse(const char** shards)
{
    box_op* ops = 0;

    box_op op = { 0, 0 };
    for(int i = 0; i < nec_size(shards); i++)
    {
        if(shards[i][0] == ';')
        {
            if(assign_type(&op))
            {
                nec_free(op.shards);
                nec_free(ops);
                return 0;
            }
            nec_push(ops, op);
            op.shards = 0;
            continue;
        }
        nec_push(op.shards, shards[i]);
    }

    return ops;
}

nic_define(int, int)

nic_int positions = { 0, 0, 0 };
nic_int values = { 0, 0, 0 };
int pc = 0;

int* box_execute(box_op op)
{
    switch(op.type)
    {
    case BOX_ASSIGNMENT:
        char* expr = str_cpy(0);
        for(int i = 2; i < nec_size(op.shards); i++)
        {
            int* value = nic_map_find_int(&values, op.shards[i]);
            if(value)
            {
                char str[30];
                sprintf(str, "%d", *value);
                str_append(&expr, str);
            }
            else str_append(&expr, op.shards[i]);
        }
        nic_map_int(&values, op.shards[0], eval(expr));
        break;
    case BOX_CHECKPOINT:
        if(!nic_map_int(&positions, op.shards[0], pc))
        {
            box_error("Syntax error: cannot redefine checkpoints.");
            return 0;
        }
        break;
    case BOX_CONDITION: break;
    case BOX_JUMP:;
        int* position = nic_map_find_int(&positions, op.shards[2]);
        if(!position)
        {
            box_error("Runtime error: referencing undeclared checkpoint.");
            return 0;
        }
        pc = *position;
        break;
    case BOX_PRINT:;
        int* value = nic_map_find_int(&values, op.shards[2]);
        if(!value)
        {
            box_error("Runtime error: referencing undeclared value.");
            return 0;
        }
        printf("%d\n", *value);
        break;
    }
    pc++;
    return &pc;
}

void box_free(box_op* ops)
{
    if(!ops) return;
    for(int i = 0; i < nec_size(ops); i++)
    {
        nec_free(ops[i].shards);
    }
    nec_free(ops);
}

