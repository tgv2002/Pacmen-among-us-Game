#include "main.h"
#include "timer.h"
#include "game_character.h"
#include "power_up.h"
#include "obstacle.h"
#include "button.h"
#include "maze.h"
#include "helper_methods.h"
#include <ft2build.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H

using namespace std;

#define RED_CHARACTER 1
#define GREEN_CHARACTER 0
#define POWERUP_AND_OBSTACLES 0
#define VAPORIZE_IMPOSTOR 1


GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;
unsigned int TEXT_VAO, TEXT_VBO;
char HUD[100];
char game_title[100];

/**************************
* Customizable functions *
**************************/

Maze maze;
GameCharacter player;
GameCharacter impostor;
PowerUp coin[5];
Obstacle bomb[5];
Button vaporize_enemy_button;
Button coins_and_bombs_button;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;

Timer t60(1.0 / 60);

// Heads up Display
clock_t prev_time;
double duration_elapsed_in_seconds;
int seconds_left = 210;
int active_coins_count = 0;
int game_over = 0;

void displayTextContent(Shader &shader, string text, float x, float y, float scale, glm::vec3 color);

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

map<GLchar, Character> Characters;

/* Render the scene with openGL */
void draw(GLuint shader_to_use_ID) {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    glUseProgram (shader_to_use_ID);

    // Eye - Location of camera.
    glm::vec3 eye (0, 0, 1);
    // Target - Where is the camera looking at. 
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera. 
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render

    if(game_over == 0) {
        // display coins and bombs if present
        if(coins_and_bombs_button.do_display == 0){
            for(int i=0;i<5;i++){
                if(coin[i].do_display == 1)
                    coin[i].draw(VP);
                if(bomb[i].do_display == 1)
                    bomb[i].draw(VP);
            }
        }

        // Display player, maze and impostor
        player.draw(VP);
        maze.draw(VP);
        if(impostor.do_display == 1)
            impostor.draw(VP);
        // Buttons display
        if(vaporize_enemy_button.do_display == 1)
            vaporize_enemy_button.draw(VP);
        if(coins_and_bombs_button.do_display == 1)
            coins_and_bombs_button.draw(VP);
    }
}

void tick_input(GLFWwindow *window) {

    // Moving player
    int move_player_left = glfwGetKey(window, GLFW_KEY_LEFT);
    int move_player_right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int move_player_up = glfwGetKey(window, GLFW_KEY_UP);
    int move_player_down = glfwGetKey(window, GLFW_KEY_DOWN);

    // Move the player based on arrow keys, continuous motion
    if (move_player_left) {
        player.move_character_carefully(1, maze, player);
    }
    if (move_player_right) {
        player.move_character_carefully(2, maze, player);
    }
    if (move_player_up) {
        player.move_character_carefully(3, maze, player);
    }
    if (move_player_down) {
        player.move_character_carefully(4, maze, player);
    }
}

void tick_elements() {
    if(impostor.do_display)
        impostor.chase_player(player, maze, impostor);
    camera_rotation_angle += 1;
}

/* Initialize the OpenGL rendering properties */
void initGL(GLFWwindow *window, int width, int height) {
    // Create all the models of maze game

    player                  = GameCharacter(-2.87f, 2.89f, GREEN_CHARACTER, 10, 2, 0.016f, 0);
    impostor                = GameCharacter(2.88f, 2.89f, RED_CHARACTER, 100000, 0, 0.0067f, 23);
    vaporize_enemy_button   = Button(-2.88f, 2.76f, VAPORIZE_IMPOSTOR);
    coins_and_bombs_button  = Button(-2.88f, 2.76f, POWERUP_AND_OBSTACLES);
    maze                    = Maze(0, 0);
    setup_impostor(impostor, maze);
    setup_buttons(vaporize_enemy_button, coins_and_bombs_button, maze);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

    reshapeWindow (window, width, height);
    glClearDepth (1.0f);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;
    int player_trying_to_exit;
    int player_hit_impostor;
    window = initGLFW(width, height);

    initGL (window, width, height);
    prev_time = clock();

    // compile and setup the shader for text rendering
    // ----------------------------
    Shader shader("../source/shaders/text.vert", "../source/shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../source/amatraca-grotesque/amatraca-grotesque-normal.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        return -1;
    }
    
    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure TEXT_VAO/TEXT_VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &TEXT_VAO);
    glGenBuffers(1, &TEXT_VBO);
    glBindVertexArray(TEXT_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, TEXT_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    sprintf(game_title, "PACMEN AMONG US");

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers
        if (t60.processTick()) {
            if (game_over == 0) {
                // 60 fps
                sprintf(HUD, "Health: %d, Tasks left: %d, Time left: %ds, Light: On", player.health, player.tasks_left, seconds_left);
                draw(programID);

                glEnable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Heads up display
                displayTextContent(shader, HUD, 56.0f, 540.0f, 0.5f, glm::vec3(0.988f, 0.371f, 0.0f));
                displayTextContent(shader, game_title, 185.0f, 15.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

                glDisable(GL_CULL_FACE);
                glDisable(GL_BLEND);

                if(player.health < 0){
                    game_over = -1; // health dropped below 0, player lost
                }

                // Updating countdown time
                duration_elapsed_in_seconds = (clock() - prev_time) / (double) CLOCKS_PER_SEC;
                if(duration_elapsed_in_seconds >= 1.0) {
                    prev_time = clock();
                    seconds_left--;
                    if(seconds_left <= 0) {
                        game_over = -1;  // time up, player lost
                    }
                }

                // Swap Frame Buffer in double buffering
                glfwSwapBuffers(window);
                tick_input(window);
                tick_elements();

                // Check if player reached the impostor
                player_hit_impostor = player.did_player_hit_impostor(impostor);
                if(player_hit_impostor == 1){
                    game_over = -1; // impostor reached player, player lost
                }

                // Check for collisions between players and other game objects
                handle_player_and_button_collision(player, impostor, vaporize_enemy_button, coins_and_bombs_button, coin, bomb, active_coins_count, maze);
                if(coins_and_bombs_button.do_display == 0){
                    handle_player_and_bomb_collision(player, bomb);
                    if(active_coins_count > 0)
                        handle_player_and_coin_collision(player, coin, active_coins_count);
                }

                // Check if player is about to exit the maze
                player_trying_to_exit = player_exit_check(player, maze);
                if(player_trying_to_exit == 1){
                    game_over = 1;  // Player won
                }
            }
            else {
                draw(programID);
                glEnable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                displayTextContent(shader, "GAME OVER", 70.0f, 420.0f, 1.65f, glm::vec3(0.984f, 0.828f, 0.066f));

                if(game_over == 1) {
                    displayTextContent(shader, "YOU WON :)", 155.0f, 240.0f, 1.05f, glm::vec3(0.0f, 1.0f, 0.0f));
                }
                else {
                    displayTextContent(shader, "YOU LOST :(", 155.0f, 240.0f, 1.05f, glm::vec3(1.0f, 0.0f, 0.0f));
                }

                displayTextContent(shader, "Close the window or Press q to exit the game", 95.0f, 65.0f, 0.4f, glm::vec3(0.3, 0.7f, 0.9f));
                glDisable(GL_CULL_FACE);
                glDisable(GL_BLEND);   
                glfwSwapBuffers(window);
            }
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    printf("\n");
    quit(window);
}

void displayTextContent(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(TEXT_VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update TEXT_VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of TEXT_VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, TEXT_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
