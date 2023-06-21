#include "box.h"
#include "nec.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    char* raw = box_load("examples/main.box");
    if(!raw) return 1;

    char** shards = box_shardify(raw);
    
    box_op* ops = box_split(shards);

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

    box_convert_c(ops);
    char* code = box_generate_c(ops);

    printf("%s\n", code);

    FILE * fp = fopen("build/examples/box.c", "w");
    fprintf(fp, "%s", code);
    fclose(fp);

    return 0;
}

