#include "main.h"
#include "game_character.h"

#ifndef OBSTACLE_H
#define OBSTACLE_H


class Obstacle {
public:
    Obstacle() {}
    Obstacle(float x, float y);
    glm::vec3 position;
    float rotation;
    int circle_vertices;
    int x_length;
    int y_length;
    int do_display;

    // Vertex and color buffers
    static GLfloat vertex_list[1220];
    static GLfloat color_list[1220];

    void getCircle(float x, float y, float r);
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    int player_reached_bomb(GameCharacter &player);

    double speed;
private:
    VAO *object;
};

#endif // OBSTACLE_H