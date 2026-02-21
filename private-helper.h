#ifndef PRIVATE_HELPER_H
#define PRIVATE_HELPER_H

#include "raylib.h"
#include "raylib-ui.h"

// noone will press more than 10 keys at once...
const int MAX_PRESSED_KEYS_AT_ONCE = 10;

rlu_element* ru_add_ui_element_children(rlu_element* parent);
rlu_element* ru_search_for_element(rlu_element* root, int id);
rlu_scene* ru_search_for_scene(rlu_context* context, int scene_id);

// rebuild combined click size area. If the scenen id is 0 all combined clicksizes are rebuild.
void rlu_rebuild_click_size(rlu_context* context, int scene_id);

int rlu_get_new_id();

void init_root_element(rlu_scene* scene);

void text_field_edit(rlu_element* text_field, int (*all_pressed_keys)[MAX_PRESSED_KEYS_AT_ONCE]);

#endif