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

int rlu_add_button(rlu_context* context, int scene_id, int parent_id) {
    rlu_element* new_button = NULL;
    int ret = 0;
    for (int it = 0; context->scene_count > it && context->scenes != NULL; it++) {
        if (context->scenes[it].id == scene_id) {
            if (parent_id == 0) {
                new_button = ru_add_ui_element_children(&context->scenes[it].root_element);
                if (new_button)
                    ret = new_button->id;
                break;
            } else {
                rlu_element* parent = ru_search_for_element(&context->scenes[it].root_element, parent_id);
                if (!parent) {
                    return 0;
                }
                new_button = ru_add_ui_element_children(parent);
                if (new_button)
                    ret = new_button->id;
                break;  
            }
        }
    }
    return ret;
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

bool rlu_trigger_ui_element_click(rlu_element* root, Vector2 position) {
    rlu_element* current = root;
    if (root == NULL)
        return false;
    if (CheckCollisionPointRec(position, root->click_size)) {
            return current->callback(current->user_data);
    }
    rlu_element* entrance = current;
    do {
        for (int i = 0; i < current->child_count; i++) {
            if (CheckCollisionPointRec(position, current->children[i].click_size_children_combined)) {
                current = &current->children[i];
                break;
            } else if (CheckCollisionPointRec(position, current->children[i].click_size)) {
                return current->children[i].callback(current->children[i].user_data);
            }
        }
    } while (current != NULL && entrance != current);
    return false;
}

void rlu_handle_frame_input(rlu_context* context) {
    bool mouse_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    int pressedKey = GetKeyPressed();
    Vector2 mouse_position = GetMousePosition();

    //todo hovermode
    if (!mouse_pressed && pressedKey == KEY_NULL) {
        return;
    }

    for (int i = 0; i < context->scene_count; i++) {
        if (context->scenes[i].enabled) {
            if (CheckCollisionPointRec(mouse_position, context->scenes[i].ui_click_size)) {
                // todo hit and trigger child elements.
                if (rlu_trigger_ui_element_click(&context->scenes[i].root_element, mouse_position)) {
                    break;
                }
            }
            // (context->scenes[i].root_element);
            // check if ui area is in scene
        }
    }
}

void rlu_draw_element(rlu_element* element) {
    //printf("[drawing texture] drawing texture with id: %i\n", element->id);
    DrawTexture(element->ui_texture, element->texture_position.x, element->texture_position.y, WHITE);
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
    int new_button_id = rlu_add_button(context, scene_id, parent_id);
    if (!new_button_id) {
        return 0;
    }
    rlu_scene* target_scene = ru_search_for_scene(context, scene_id);
    rlu_element* new_element = ru_search_for_element(&target_scene->root_element, new_button_id);
    if (!new_element) {
        return 0;
    }
    

    new_element->ui_texture = ui_texture;
    new_element->callback = callback;
    new_element->click_size.x = position.x;
    new_element->click_size.y = position.y;
    new_element->texture_position.x = position.x;
    new_element->texture_position.y = position.y;
    new_element->enabled = true;
    new_element->layer = 0;
    rlu_rebuild_click_size(context, scene_id);
    return new_element;
}

// todo create text field
// todo create container
// todo create tests
