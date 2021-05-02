#include "main.h"
#include "game_character.h"

#ifndef POWER_UP_H
#define POWER_UP_H


class PowerUp {
public:
    PowerUp() {}
    PowerUp(float x, float y);
    glm::vec3 position;
    float rotation;
    int circle_vertices;
    int x_length;
    int y_length;
    int do_display;

    // Vertex and color buffers
    static GLfloat vertex_list[1200];
    static GLfloat color_list[1200];

    void getCircle(float x, float y, float r);
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    int player_reached_coin(GameCharacter &player);
    void tick();
    double speed;
private:
    VAO *object;
};

#endif // POWER_UP_H