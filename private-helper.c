#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "private-helper.h"
#include "raylib-ui.h"

#define RAYLIB_UI_DEFAULT_CHILD_RESERVE 5

rlu_element* ru_add_ui_element_children(rlu_element* parent) {
    int add_count = 1;
    if (parent->children == NULL && parent->type != ROOT) {
        add_count = RAYLIB_UI_DEFAULT_CHILD_RESERVE;
    }
    if (parent->child_reserve <= parent->child_count) {
        rlu_element* old_elements = parent->children;
        parent->children = calloc(add_count + parent->child_reserve, sizeof(rlu_element));
        memmove(parent->children, old_elements, parent->child_count * sizeof(rlu_element));
        parent->child_reserve += add_count;
        free(old_elements);
    }
    rlu_element* new_element = &parent->children[parent->child_count];
    parent->child_count++;
    new_element->id = rlu_get_new_id();
    new_element->parent = parent;
    return new_element;
}

void init_root_element(rlu_scene* scene) {
    scene->depth = 0;
    scene->root_element.parent = NULL;
    scene->root_element.child_reserve = RAYLIB_UI_DEFAULT_CHILD_RESERVE;
    scene->root_element.children = calloc(RAYLIB_UI_DEFAULT_CHILD_RESERVE, sizeof(rlu_element));
    scene->root_element.id = rlu_get_new_id();
    scene->root_element.hide = true;
    scene->root_element.can_hide = true;
    scene->root_element.enabled = true;

}

//todo write tests
rlu_element* ru_search_for_element(rlu_element* root, int id) {
    rlu_element* current = root;
    while (current != NULL) {
        if (current->id == id)
            return current;
        if (current->child_count > 0 && current->children != NULL) {
            for (int i = 0; i < current->child_count; i++) {
                if (current->children[i].id == id)
                    return &current->children[i];
            }
            current = &current->children[0];
        }else if (current->parent) {
            rlu_element* siblings = current->parent->children;
            rlu_element* next_sibling = NULL;
            bool next = false;
            for (int i = 0; i < current->parent->child_count; i++) {
                if (next) {
                    next_sibling = &(siblings[i]);
                }
                if (current->id == siblings[i].id) {
                    next = true;
                }
            }
            current = next_sibling;
        } else {
            current = NULL;
        }
    }
    return NULL;
}

rlu_scene* ru_search_for_scene(rlu_context* context, int scene_id) {
    for (int i = 0; i < context->scene_count; i++) {
        if (context->scenes[i].id == scene_id)
            return &context->scenes[i];
    }
    return NULL;
}

int rlu_get_new_id() {
    static int id = 0;
    return ++id;
}


// todo finish? already implemented?
bool rlu_handle_scene_input(rlu_element* root) {
    rlu_element* current = root;
    int id = root->id;
    while (current->child_count != 0) {
        current = &current->children[current->child_count - 1];
        
    }
    while (current != NULL) {
        for (int i = 0; i < current->child_count; i++) {

        }
    }
    return false;
}

static Rectangle rectangle_addition(Rectangle one, Rectangle two) {
    if (two.x < one.x) {
        one.x = two.x;
    }
    if (two.y < one.y) {
        one.y = two.y;
    }

    float width_combined_one = one.x + one.width;
    float width_combined_two = two.x + two.width;
    float height_combined_one = one.y + one.height;
    float height_combined_two = two.y + two.height;

    if (width_combined_two > width_combined_one) {
        one.width +=  width_combined_two - width_combined_one;
    }
    if (height_combined_two > height_combined_one) {
        one.height = height_combined_two - height_combined_one;
    }
    return one;
}

static Rectangle rebuild_combined_clicksize(rlu_element* element) {
    Rectangle combined_rectangle = {0,0,0,0};
    
    if (element->child_count == 0) {
        explicit_bzero(&element->click_size_children_combined, sizeof(Rectangle));
        return element->click_size;
    } else {
        for (int i = 0; i < element->child_count; i++) {
            Rectangle addition = rebuild_combined_clicksize(&element->children[i]);
            combined_rectangle = rectangle_addition(combined_rectangle, addition);
        }
    }
    element->click_size_children_combined = combined_rectangle;
    // for parent add it's own clicksize
    return rectangle_addition(combined_rectangle, element->click_size);
}

void rlu_rebuild_click_size(rlu_context* context, int scene_id) {
    
    if (scene_id != 0) {
        rlu_scene* scene = ru_search_for_scene(context, scene_id);
        scene->ui_click_size = rebuild_combined_clicksize(&scene->root_element);
    } else {
        for (int i = 0; i < context->scene_count; i++) {
            context->scenes[i].ui_click_size = rebuild_combined_clicksize(&context->scenes[i].root_element);
        }
    }
}

void text_field_resize(rlu_element* text_field, size_t new_size) {
    char* replacement = calloc(new_size, 1);
    memcpy(replacement, text_field->user_data, text_field->text_size);
    free(text_field->user_data);
    text_field->user_data = replacement;
    text_field->text_size = new_size;
}

void text_field_control_mod(rlu_element* text_field, int key, size_t str_size) {
    if (key == KEY_V) {
        const char * clipboard = GetClipboardText();
        size_t clipboard_length = strlen(clipboard);

        if (clipboard_length > text_field->text_size - str_size - 1) {
            text_field_resize(text_field, str_size + strlen(clipboard) + 1);
            memmove(text_field->user_data + text_field->cursor_pos + clipboard_length, text_field->user_data + text_field->cursor_pos, strlen(text_field->user_data + text_field->cursor_pos));
            memcpy(text_field->user_data + text_field->cursor_pos, clipboard, clipboard_length);
        }
    } if (key == KEY_C) {
        SetClipboardText(text_field->user_data);
    }
}

void text_field_edit(rlu_element* text_field, int (*all_pressed_keys)[MAX_PRESSED_KEYS_AT_ONCE]) {
    size_t str_size = strlen(text_field->user_data);

    if (str_size + MAX_PRESSED_KEYS_AT_ONCE <= text_field->text_size) {
        text_field_resize(text_field, str_size + MAX_PRESSED_KEYS_AT_ONCE + 1);
    }

    char* text = text_field->user_data;
    bool control_flag = false;

    for (int i = 0; i < MAX_PRESSED_KEYS_AT_ONCE; i++) {
        
        if ((*all_pressed_keys)[i] == KEY_LEFT_CONTROL || (*all_pressed_keys)[i] == KEY_RIGHT_CONTROL) {
            control_flag = true;
            continue;
        }
        if (control_flag) {
            text_field_control_mod(text_field, (*all_pressed_keys)[i], str_size);
            control_flag = false;
        }
        //normal text input; TODO unicode?
        if ((*all_pressed_keys)[i] >= 32
            && (*all_pressed_keys)[i] <= 126) {
                //if cursorposition is equal to str_size chars will be appended
                if (text_field->cursor_pos == str_size) {
                    text[text_field->cursor_pos] = (*all_pressed_keys)[i];
                    text[text_field->cursor_pos + 1] = '\0';
                    text_field->cursor_pos++;
                    str_size++;
                } else {
                    
                }
        }

        // todo specials like CTRL-C CTRL-V CTRL-A
    }
}