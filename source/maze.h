#include "main.h"

#ifndef MAZE_H
#define MAZE_H


class Maze {
public:
    Maze() {}
    Maze(float x, float y);
    glm::vec3 position;
    float rotation;
    int maze_vertices;

    // Vertex and color buffers
    static GLfloat vertex_list[4000];
    static GLfloat color_list[4000];

    // Helper arrays
    int complete_wall_list[587][2];
    int wall_is_visible[587];
    float complete_points_list[350][2];
    int set_size[312];
    int set_par[312];
    int shuffled_walls[587];
    int shortest_distance[312][312];
    int shortest_path_successor[312][312];
    float ideal_positions[312][2];

    float cell[312][2];

    // dsu for kruskal
    int find(int node);
    void shuffle_walls();
    void Union(int node_1, int node_2);

    // obtaining points and walls list
    void getPointsAndWallsLists();
    void generateMaze();
    void compute_all_shortest_paths();

    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed;
private:
    VAO *object;
};

#endif // MAZE_H