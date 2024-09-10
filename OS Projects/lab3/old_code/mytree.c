#include "rbtree.h"

struct myline {
    struct rb_node node;
    int position;
    char* string;
}

int my_insert(struct rb_root *root, struct myline *data)
{
struct rb_node **new = &(root->rb_node), *parent = NULL;

/* Figure out where to put new node */
while (*new) {
    struct myline *this = container_of(*new, struct myline, node);
    int result = strcmp(data->keystring, this->keystring);

    parent = *new;
    if (result < 0)
        new = &((*new)->rb_left);
    else if (result > 0)
        new = &((*new)->rb_right);
    else
        return FALSE;
}

/* Add new node and rebalance tree. */
rb_link_node(&data->node, parent, new);
rb_insert_color(&data->node, root);

return TRUE;
}
