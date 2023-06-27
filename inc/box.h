#ifndef SEHA_BOX
#define SEHA_BOX

#define BOX_ASSIGNMENT 1
#define BOX_CHECKPOINT 2
#define BOX_CONDITION 3
#define BOX_JUMP 4
#define BOX_PRINT 5

/*

a = 3; BOX_ASSIGNMENT

point; BOX_CHECKPOINT

if(a > 5) point; BOX_CONDITION

goto(point); BOX_JUMP

print(a); BOX_PRINT

*/

struct box_op
{
    const char** shards;
    int type;
};
typedef struct box_op box_op;

char* box_load(const char*);
char** box_shardify(const char*);
box_op* box_parse(const char**);

int* box_execute(box_op);

void box_free(box_op*);

#endif /* SEHA_BOX */

