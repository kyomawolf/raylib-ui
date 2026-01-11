#include "testing.h"
#include "private-helper.h"
#include "test_helper.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

bool find_root(rlu_element* test_tree) {
    return (ru_search_for_element(test_tree, test_tree->id) != NULL);
}

bool find_last_lower_element(rlu_element* test_tree) {
    rlu_element* current = test_tree;
    int target_id = 0;
    while(current != NULL) {
        if (current->child_count != 0) {
            current = &current->children[current->child_count - 1];
        } else {
            target_id = current->id;
            break;
        }
    }
    if (!current) {
        printf("WARNING: Test may be wrong, current pointer is NULL.");
    } 
    return (ru_search_for_element(test_tree, target_id) != current);
}

bool find_last_highest_element(rlu_element* test_tree) {
    rlu_element* current = test_tree;
    int target_id = 0;
    while(current != NULL) {
        if (current->child_count != 0) {
            current = &current->children[0];
        } else {
            target_id = current->id;
            break;
        }
    }
    if (!current) {
        printf("WARNING: Test may be wrong, current pointer is NULL.");
    }
    return (ru_search_for_element(test_tree, target_id) != current);
}

bool find_middle_element(rlu_element* test_tree, int max_depth) {
    int target_depth = (int) max_depth / 2;
    int target_id = 0;
    rlu_element* current = test_tree;
    for (int current_depth = 0; current_depth < target_depth; current_depth++) {
        if (current->child_count != 0) {
            current = &current->children[(int) current->child_count / 2];
        } else {
            target_id = current->id;
            break;
        }
    }
    if (!current) {
        printf("WARNING: Test may be wrong, current pointer is NULL.");
    }
    return (ru_search_for_element(test_tree, target_id) != current);
}


void main_test_find_element() {
    int max_depth = 5;
    int max_children = 3;

    rlu_element* tree = element_create_random_tree(max_depth, max_children);

    assert(find_root(tree) == true);
    assert(find_last_lower_element(tree) == true);
    assert(find_last_highest_element(tree) == true);
    assert(find_middle_element(tree, max_depth) == true);

}