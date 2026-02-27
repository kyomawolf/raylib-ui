#include "raylib-ui.h"
#include "raylib.h"
#include "stdio.h"

bool callback_test(void* userdata) {
    printf("wow test button has been pressed!\n");
    return true;
}

int main() {
    InitWindow(800, 800, "example program");
    SetTargetFPS(60);
    rlu_context *ui_context;

    ui_context = rlu_create_new_context();
    int main_scene = rlu_add_scene(ui_context);

    
    Image button_image = GenImageColor(100, 50, RED);
    Texture2D button_texture = LoadTextureFromImage(button_image);
    Vector2 position = {400, 400};


    rlu_element* test_button = rlu_add_button_full(ui_context, 0, main_scene, position, button_texture, callback_test);
    
    position.x = 400;
    position.y = 600;
    Image text_image = GenImageColor(100, 100, WHITE);
    Texture2D text_texture = LoadTextureFromImage(text_image);
    rlu_element* text_field = rlu_add_text_field(ui_context, 0, main_scene, position, text_texture, "Hello there!");
    test_button->hide = false;

    while (!WindowShouldClose()) {
        rlu_handle_frame_input(ui_context);
        BeginDrawing();
        ClearBackground(GRAY);
        rlu_render(ui_context);
        EndDrawing();
    }
    return 0;
}