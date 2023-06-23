#include "box.h"
#include "nec.h"
#include "str.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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

void assignType(box_op* op)
{
    if(strcmp(op->shards[0], "echo") == 0)
    {
        op->type = BOX_ECHO;
    }
    else if(op->shards[1][0] == '=')
    {
        op->type = BOX_ASSIGNMENT;
    }
    else if(strcmp(op->shards[0], "if") == 0)
    {
        op->type = BOX_IF;
    }
    else if(op->shards[2][0] == '=')
    {
        op->type = BOX_CREATION;
    }
}

box_op* box_split(char** shards)
{
    box_op* ops = 0;

    box_op op = { 0, 0 };
    for(int i = 0; i < nec_size(shards); i++)
    {
        if(shards[i][0] == ';')
        {
            assignType(&op);
            nec_push(ops, op);
            op.shards = 0;
            continue;
        }
        nec_push(op.shards, shards[i]);
    }

    return ops;
}

char* convert_type(const char* type)
{
    if(strcmp(type, "Int") == 0) return "int32_t";
    if(strcmp(type, "Uint") == 0) return "uint32_t";
    box_error("Unknown type");
    return 0;
}

void box_convert_c(box_op* ops)
{
    for(int i = 0; i < nec_size(ops); i++)
    {
        if(ops[i].type == BOX_CREATION)
        {
            ops[i].shards[0] = convert_type(ops[i].shards[0]);
        }
        if(ops[i].type == BOX_ECHO)
        {
            char* shard = str_cpy("printf(\"%d\",");
            str_append(&shard, ops[i].shards[1]);
            str_append(&shard, ")");
            ops[i].shards = 0;
            nec_push(ops[i].shards, shard);
        }
    }
}

char* box_generate_c(const box_op* ops)
{
    char* raw = str_cpy("#include <stdint.h>\nint main(){");

    for(int i = 0; i < nec_size(ops); i++)
    {
        for(int j = 0; j < nec_size(ops[i].shards); j++)
        {
            str_append(&raw, ops[i].shards[j]);
            str_append(&raw, " ");
        }
        str_append(&raw, ";");
    }

    str_append(&raw, "return 0;}");
    return raw;
}

