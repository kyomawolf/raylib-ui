#include "test_helper.h"
#include "private-helper.h"
#include <string.h>
#include <sys/random.h>
#include <stdlib.h>

rlu_element* element_new() {
    rlu_element* new_element = calloc(1, sizeof(rlu_element));
    new_element->id = rlu_get_new_id();
    new_element->enabled = true;
    new_element->type = NONE;
    return new_element;
}

rlu_element* element_copy(rlu_element* original, int id) {
    if (id == -1)
        id = rlu_get_new_id();
    
    rlu_element* copy = calloc(1, sizeof(rlu_element));
    memcpy(copy, original, sizeof(rlu_element));
    copy->id = id;
}

void element_copy_existing_range(rlu_element* buffer, rlu_element* original, int range) {
    for (int i = 0; i < range; i++) {
        memcpy(&buffer[i], original, sizeof(rlu_element));
        buffer[i].id = rlu_get_new_id();
    }
}

void element_create_children(int child_range, rlu_element* parent) {
    int child_count = 0;

    getrandom(child_count, sizeof(int), GRND_RANDOM);

    child_count = child_count % child_range;

    parent->child_count = child_count;
    parent->child_reserve = 0;
    parent->children = calloc(child_count, sizeof(rlu_element));

    rlu_element* first = &parent->children[0];
    first->id = rlu_get_new_id();
    first->enabled = true;
    first->type = NONE;
    first->parent = parent;
    element_copy_existing_range(&parent->children[1], first, child_count - 1);
}

void create_children_recursive(int max_depth, int child_range, rlu_element* parent) {
    element_create_children(child_range, parent);
    if (max_depth <= 0)
        return;
    for (int i = 0; i < parent->child_count; i++) {
        create_children_recursive(max_depth - 1, child_range, &parent->children[i]);
    }
}

rlu_element* element_create_random_tree(int max_depth, int child_range) {
    
    rlu_element* root = element_new();

    root->type = ROOT;
    rlu_element* parent = root;
    create_children_recursive(max_depth -1, child_range, parent);
    return root;
}