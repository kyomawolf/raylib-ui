#ifndef TEST_HELPER_H
#define TEST_HELPER_H
#include "raylib-ui.h"

rlu_element* element_new();
rlu_element* element_copy(rlu_element* original, int id );
rlu_element* element_create_random_tree(int max_depth, int child_range);

#endif