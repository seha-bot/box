#include "box.h"
#include "nec.h"
#include <stdio.h>
#include <unistd.h>

void temp_free_shards(char** shards)
{
    for(int i = 0; i < nec_size(shards); i++)
    {
        nec_free(shards[i]);
    }
    nec_free(shards);
}

int main()
{
    char* raw = box_load("examples/main.box");
    if(!raw) return 1;

    char** shards = box_shardify(raw);

    box_op* ops = box_parse((const char**)shards);
    if(!ops)
    {
        temp_free_shards(shards);
        return 1;
    }

    for(int i = 0; i < nec_size(ops); i++)
    {
        printf("%d. operation:\n", i + 1);
        printf("\tType: %d\n", ops[i].type);
        printf("\tShards:\n\t\t");
        for(int j = 0; j < nec_size(ops[i].shards); j++)
        {
            printf("%s ", ops[i].shards[j]);
        }
        printf("\n");
    }

    int i = 0;
    while(i < nec_size(ops))
    {
        int* pc = box_execute(ops[i]);
        if(!pc)
        {
            box_free(ops);
            return 1;
        }
        i = *pc;
    }

    return 0;
}

