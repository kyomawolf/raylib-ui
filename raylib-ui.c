#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "private-helper.h"

rlu_context* rlu_create_new_context() {
    rlu_context* new_context = calloc(1, sizeof(rlu_context));
    new_context->scenes = calloc(4, sizeof(rlu_scene));
    new_context->scene_reserve = 4;
    new_context->scene_count = 0;
    return new_context;
}

int rlu_add_scene(rlu_context* context) {
    if (context->scene_reserve <= context->scene_count) {
        rlu_scene *old_scenes = context->scenes;
        context->scenes = calloc(context->scene_count * 2, sizeof(rlu_scene));
        context->scene_reserve *= 2;

        memmove(context->scenes, old_scenes, context->scene_count * sizeof(rlu_scene));
        free(old_scenes);
    }
    context->scene_count++;
    int new_id = rlu_get_new_id();
    context->scenes[context->scene_count - 1].id = new_id;
    context->scenes[context->scene_count - 1].root_element.type = ROOT;
    context->scenes[context->scene_count - 1].enabled = true;
    return new_id;
}

// todo rename
rlu_element* rlu_add_element_base(rlu_context* context, int parent_id, int scene_id, Vector2 position, Texture2D ui_texture, enum rlu_ui_type type) {
    rlu_element* new_element = NULL;
    int element_id = 0;
    for (int it = 0; context->scene_count > it && context->scenes != NULL; it++) {
        if (context->scenes[it].id == scene_id) {
            if (parent_id == 0) {
                new_element = ru_add_ui_element_children(&context->scenes[it].root_element);
                if (new_element)
                    element_id = new_element->id;
                break;
            } else {
                rlu_element* parent = ru_search_for_element(&context->scenes[it].root_element, parent_id);
                if (!parent) {
                    return 0;
                }
                new_element = ru_add_ui_element_children(parent);
                if (new_element)
                    element_id = new_element->id;
                break;
            }
        }
    }
    
    if (!element_id || !new_element) {
        return 0;
    }
    
    new_element->type = type;
    new_element->ui_texture = ui_texture;
    new_element->click_size.x = position.x;
    new_element->click_size.y = position.y;
    new_element->click_size.width = (float)ui_texture.width;
    new_element->click_size.height = (float)ui_texture.height;
    new_element->texture_position.x = position.x;
    new_element->texture_position.y = position.y;
    new_element->enabled = true;
    new_element->layer = 0;
    return new_element;
}

bool rlu_ui_add_callback(rlu_context* context, int scene_id, int target_id, bool(*callback)(void*)) {
    rlu_scene* target_scene = ru_search_for_scene(context, scene_id);
    if (!target_scene) {
        return false;
    }
    rlu_element *target_element = ru_search_for_element(&target_scene->root_element, target_id);
    
    if (target_element->callback) {
        return false;
    }
    target_element->callback = callback;
    return true;
}

bool rlu_trigger_ui_element_click(rlu_context* context, rlu_element* root, Vector2 position) {
    rlu_element* current = root;
    if (root == NULL)
        return false;
    if (CheckCollisionPointRec(position, root->click_size)) {
            context->current_focus = current;
            if (current->children[i].callback != NULL)
                return current->callback(current->user_data);
    }
    rlu_element* entrance = current;
    do {
        for (int i = 0; i < current->child_count; i++) {
            if (CheckCollisionPointRec(position, current->children[i].click_size_children_combined)) {
                current = &current->children[i];
                break;
            } else if (CheckCollisionPointRec(position, current->children[i].click_size)) {
                context->current_focus = &current->children[i];
                if (current->children[i].callback != NULL)
                    return current->children[i].callback(current->children[i].user_data);
            }
        }
    } while (current != NULL && entrance != current);
    return false;
}

void rlu_handle_mouse_input(rlu_context* context) {
    Vector2 mouse_position = GetMousePosition();
    printf("mouse button pressed at: %f %f\n", mouse_position.x, mouse_position.y);
    for (int i = 0; i < context->scene_count; i++) {
        if (context->scenes[i].enabled) {
            printf("ui scene click size: x%f y%f w%f h%f\n", context->scenes[i].ui_click_size.x, context->scenes[i].ui_click_size.y, context->scenes[i].ui_click_size.width, context->scenes[i].ui_click_size.height);
            if (CheckCollisionPointRec(mouse_position, context->scenes[i].ui_click_size)) {
                if (rlu_trigger_ui_element_click(context, &context->scenes[i].root_element, mouse_position)) {
                    break;
                }
            }
            // (context->scenes[i].root_element);
            // check if ui area is in scene
        }
    }
}

bool rlu_handle_key_input_is_hotkey(int* pressed_keys, rlu_hotkey* hotkey) {
    for (int i = 0; i < MAX_PRESSED_KEYS_AT_ONCE && pressed_keys[i] != KEY_NULL; i++) {
        bool key_entry_exists = false;
        for (int j = 0; j < hotkey->key_count; j++) {
            if (hotkey->keys[j] == pressed_keys[i]) {
                key_entry_exists = true;
            }
        }
        if (!key_entry_exists)
            return false;
    }
    return true;
}

void rlu_handle_key_input(rlu_context* context, int first_pressed) {
    int pressed_keys[MAX_PRESSED_KEYS_AT_ONCE];
    int all_pressed_keys[MAX_PRESSED_KEYS_AT_ONCE];
    int modifier = RLU_HK_NONE;
    int current_key = first_pressed;
    int index = 0;
    int pressed_non_mod_keys = 0;
    while (current_key != KEY_NULL && index < MAX_PRESSED_KEYS_AT_ONCE) {
        switch (current_key) {
            case KEY_LEFT_CONTROL:
            case KEY_RIGHT_CONTROL:
                modifier |= RLU_HK_CTRL;
            break;
            case KEY_LEFT_SHIFT:
            case KEY_RIGHT_SHIFT:
                modifier |= RLU_HK_SHIFT;
            break;
            case KEY_LEFT_ALT:
            case KEY_RIGHT_ALT:
                modifier |= RLU_HK_ALT;
            break;
            default:
            pressed_non_mod_keys++;
            pressed_keys[index] = current_key;
            break;
        }
        index++;
        all_pressed_keys[index] = current_key;
        current_key = GetKeyPressed();
    }

    // TODO add special mode for setting hotkeys
    //check for hotkeys
    for (int hk_counter = 0; hk_counter < context->hotkey_count; hk_counter++) {
        rlu_hotkey hotkey = context->hotkey_list[hk_counter];
        if (hotkey.modifier != modifier && pressed_non_mod_keys != hotkey.key_count) {
            continue;
        }
        if (rlu_handle_key_input_is_hotkey(pressed_keys, &hotkey)) {
            hotkey.callback(hotkey.user_data);
            printf("[DEBUG]: called hotkey with ID %i\n", hotkey.id);
            return;
        }
    }

    //todo check for focus and writable (text) field
    if (context->current_focus != NULL
        && context->current_focus->type == TEXTFIELD
        && context->current_focus->writable) {
            text_field_edit(context->current_focus, &all_pressed_keys);
    }
}

void rlu_handle_frame_input(rlu_context* context) {
    bool mouse_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int pressedKey = GetKeyPressed();

    //todo hovermode
    if (mouse_pressed) {
        rlu_handle_mouse_input(context);
    }
    if (pressedKey == KEY_NULL) {
        rlu_handle_key_input(context, pressedKey);
    }
}

void rlu_draw_element(rlu_element* element) {
    //printf("[drawing texture] drawing texture with id: %i\n", element->id);
    DrawTexture(element->ui_texture, (int)element->texture_position.x, (int)element->texture_position.y, WHITE);
}

void rlu_render_scene(rlu_scene* scene) {

    if (!scene->enabled) {
        return;
    }

    rlu_element* current = &scene->root_element;

    while (current != NULL) {
        
        if (!current->hide) {
            rlu_draw_element(current);
        }

        // select next element to be drawn
        if (current->child_count > 0 && current->children != NULL
            && !current->hide) {
            current = &current->children[0];
        } else if (current->parent) {
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
}


void rlu_render(rlu_context* context) {

    // render the last scene first, as it is in the back
    for (int i = context->scene_count - 1; i >=0; i--) {
        rlu_render_scene(&context->scenes[i]);
    }

}

rlu_element* rlu_add_button_full(rlu_context* context, int parent_id, int scene_id, 
                         Vector2 position, Texture2D ui_texture, bool (*callback)(void*)) {
    rlu_element* new_element = rlu_add_element_base(context, parent_id, scene_id, position, ui_texture, BUTTON);
    if (!new_element) {
        return 0;
    }

    new_element->callback = callback;

    return new_element;
}

rlu_element* rlu_add_text_field(rlu_context* context, int parent_id, int scene_id, 
                                Vector2 position, Texture2D ui_texture, const char *text) {
    rlu_element* new_element = rlu_add_element_base(context, parent_id, scene_id, position, ui_texture, TEXTFIELD);

     if (!new_element) {
        return 0;
    }

    new_element->user_data = calloc(strlen(text) + 1, 1);
    strncpy(new_element->user_data, text, strlen(text));

    return new_element;
}

// todo create text field
// todo create container
// todo create layering for faster rendering speed - and with it a static ui state
// todo create tests
