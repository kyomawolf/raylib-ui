#ifndef PRIVATE_HELPER_H
#define PRIVATE_HELPER_H

#include "raylib.h"
#include "raylib-ui.h"

rlu_element* ru_add_ui_element_children(rlu_element* parent);
rlu_element* ru_search_for_element(rlu_element* root, int id);
rlu_scene* ru_search_for_scene(rlu_context* context, int scene_id);

// rebuild combined click size area. If the scenen id is 0 all combined clicksizes are rebuild.
void rlu_rebuild_click_size(rlu_context* context, int scene_id);

int rlu_get_new_id();

void init_root_element(rlu_scene* scene);

#endif