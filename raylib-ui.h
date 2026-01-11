#ifndef RAYLIB_UI_H
#define RAYLIB_UI_H

#include "raylib.h"

enum rlu_ui_type {NONE, ROOT, BUTTON, TEXTFIELD};

typedef struct base_element {
    struct base_element* parent;
    struct base_element* children;
    int child_count;
    int child_reserve;
    int id;
    int layer;
    enum rlu_ui_type type;

    Rectangle click_size;
    Rectangle click_size_children_combined;
    bool enabled;
    
    bool can_hide;
    bool hide;

    Texture2D ui_texture;
    Vector2 texture_position;
    bool (*callback)(void*);
    void* user_data;
} rlu_element;

typedef struct ui_hotkey {
    int* keys;
    int id;
    
    rlu_element* related_ui_element;
    bool enabled;
} rlu_hotkey;

typedef struct ui_scene {
    // todo change to pointer lel
    rlu_element root_element;

    int depth;

    int id;
    bool enabled;
    Rectangle ui_click_size;
} rlu_scene;


typedef struct ui_state {
    rlu_scene* scenes;
    int scene_count;
    int scene_reserve;

} rlu_context;


//! External

/**
 * @return a new context needed for (almost) all raylib ui calls, caller is owner of the memory
 */
rlu_context* rlu_create_new_context();

int rlu_add_scene(rlu_context* context);

/**
 * @return 0 if no button could be added
 */
int rlu_add_button(rlu_context* context, int scene_id, int parent_id);

/**
 * @return false if a callback has already been configured, true if it has been successfully set
 */
bool rlu_ui_add_callback(rlu_context* context, int scene_id, int target_id, bool(*callback)(void*) );

/**
 * handles input for the scene, calls callbacks
 */
void rlu_handle_frame_input(rlu_context* context);

/**
 * renders all non hidden ui elements with a texture available
 */
void rlu_render(rlu_context* context);


/**
 * @param context the programs global raylib-ui context
 * @param parent_id the parent of the new button
 * @param scene_id the scene where to find the parent
 * @param position the position relative to the parents position
 * @param ui_texture the default texture to be rendered in the render loop
 * @param callback the logical callback to be used when clicked.
 * you can set user data in the button for the function to be used.
 * return true, when input was handled, and false if not. default should be true
 * @return gives you back the buttons pointer
 */
rlu_element* rlu_add_button_full(rlu_context* context, int parent_id, int scene_id, 
                         Vector2 position, Texture2D ui_texture, bool (*callback)(void*));

#endif /*RAYLIB_UI_H*/