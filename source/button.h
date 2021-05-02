#include "main.h"
#include "game_character.h"

#ifndef BUTTON_H
#define BUTTON_H


class Button {
public:
    Button() {}
    Button(float x, float y, int color);
    glm::vec3 position;
    float rotation;
    int circle_vertices;
    int color;
    int x_length;
    int y_length;
    int do_display;

    // Vertex and color buffers
    static GLfloat vertex_list[1200];
    static GLfloat color_list[1200];

    void getCircle(float x, float y, float r);
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    int player_reached_button(GameCharacter &player, GameCharacter &impostor);

    double speed;
private:
    VAO *object;
};

#endif // BUTTON_H