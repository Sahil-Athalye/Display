#pragma once

// An AVL tree node
struct Node
{
    int key;
    char* string;
    struct Node *left;
    struct Node *right;
    int height;
};

struct Node *insert(struct Node *node, char *string);
struct Node * minValueNode(struct Node* node);
struct Node* deleteNode(struct Node* root, int key);

void preOrder(struct Node *root);
