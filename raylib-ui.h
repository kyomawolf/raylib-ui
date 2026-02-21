#ifndef RAYLIB_UI_H
#define RAYLIB_UI_H

#if RAYLIB_VERSION_MAJOR < 5
# error "WRONG RAYLIB MAYOR VERSION"
#elif RAYLIB_VERSION_MINOR < 5
# error "WRONG RAYLIB MINOR VERSION"
#endif


#include "raylib.h"

enum rlu_ui_type {NONE, ROOT, BUTTON, TEXTFIELD};
enum rlu_hotkey_mods {RLU_HK_NONE = 0, RLU_HK_CTRL = 1, RLU_HK_SHIFT = 2, RLU_HK_ALT = 4};

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

    //only if obj button
    bool (*callback)(void*);

    //if button: will be give into callback as argument
    //if text field: contains text to be displayed in text field
    void* user_data;

    // only if obj text field
    bool writable;
    size_t cursor_pos;
    size_t text_size;

    //todo font stuff

    // only if obj container
} rlu_element;

typedef struct ui_hotkey {
    //? 1 Ctrl | 2 Shift | 4 Alt
    int modifier;
    int* keys;
    int key_count;
    int id;
    
    rlu_element* related_ui_element;
    bool (*callback)(void*);
    bool enabled;
    void* user_data;
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

    rlu_hotkey* hotkey_list;
    int hotkey_count;

    rlu_element* current_focus;
    //todo insert mode
} rlu_context;


//! External

/**
 * @return a new context needed for (almost) all raylib ui calls, caller is owner of the memory
 */
rlu_context* rlu_create_new_context();

int rlu_add_scene(rlu_context* context);


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
 * @return 0 if no button could be added
 */
rlu_element* rlu_add_element_base(rlu_context* context, int parent_id, int scene_id, 
                         Vector2 position, Texture2D ui_texture, enum rlu_ui_type type);

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


rlu_element* rlu_add_text_field(rlu_context* context, int parent_id, int scene_id, 
                                Vector2 position, Texture2D ui_texture, const char* text);



#endif /*RAYLIB_UI_H*/