#include "main.h"
#include "maze.h"
#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H


class GameCharacter {
public:
    GameCharacter() {}
    GameCharacter(float x, float y, int color, int health, int tasks_left, float speed, int in_cell_number);
    glm::vec3 position;
    float rotation;
    int color;
    int semi_circle_vertices;
    int x_length;
    int y_length;
    int health;
    int tasks_left;
    int in_cell_number;
    int impostor_can_change_direction;
    int impostor_prev_direction;
    int do_display;

    // Vertex and color buffers
    static GLfloat vertex_list[660];
    static GLfloat color_list[660];

    float tempSemiCircleColor[570];
    float tempSemiCircle[570];
    float tempRectangle[18];
    float tempRectangleColor[18];

    // Move character
    int move_character_carefully(int direction, Maze &maze, GameCharacter &gc);
    int get_cell_number_of_character(GameCharacter &gc);
    int chase_player(GameCharacter &player, Maze &maze, GameCharacter &impostor);
    int move_via_ideal_positions(int direction, int curr_cell, int next_cell, Maze &maze);
    int did_player_hit_impostor(GameCharacter& impostor);

    void getSemiCircle(float x, float y, float r);
    void getRectangle(float l, float b, float x, float y, int color);
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed;
private:
    VAO *object;
};

#endif // GAME_CHARACTER_H
