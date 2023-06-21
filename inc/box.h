#ifndef SEHA_BOX
#define SEHA_BOX

#define BOX_CREATION 1
#define BOX_ASSIGNMENT 2
#define BOX_INVENTION 3
#define BOX_IF 4
#define BOX_WHILE 5
#define BOX_CALL 6
#define BOX_ECHO 7

struct box_op
{
    char** shards;
    int type;
};
typedef struct box_op box_op;

char* box_load(const char*);
char** box_shardify(const char*);
box_op* box_split(char**);

void box_convert_c(box_op*);
char* box_generate_c(const box_op*);

#endif /* SEHA_BOX */

