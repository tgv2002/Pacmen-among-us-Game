#include "obstacle.h"
#include "main.h"

GLfloat Obstacle::vertex_list[1220];
GLfloat Obstacle::color_list[1220];

Obstacle::Obstacle(float x, float y) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->circle_vertices = 0;
    speed = 1;
    this->do_display = 1;
    this->x_length = 0.2f;
    this->y_length = 0.36f;
    
    getCircle(0.0f, 0.0f, 0.12f);
    int ctr = 3*this->circle_vertices;
    float flame_color[3] = { 1.0f, 1.0f, 0.0f };
    for(int i = 0; i < 9; i++){
        this->color_list[ctr + i] = flame_color[i % 3];
    }
    // Circle with small triangle on top (Bomb with fire on top)
    this->vertex_list[ctr++] = -0.04f;
    this->vertex_list[ctr++] = 0.12f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 0.04f;
    this->vertex_list[ctr++] = 0.12f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 0.24f;
    this->vertex_list[ctr++] = 0.0f;

    this->object = create3DObject(GL_TRIANGLES, 3 + this->circle_vertices, this->vertex_list, this->color_list, GL_FILL);
}

void Obstacle::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Obstacle::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Obstacle::tick() {
    this->rotation += speed;
}

void Obstacle::getCircle(float x, float y, float r) {
    // Draw full circle with given centre and radius
    float prev_x = (x + r);
    float prev_y = y;
    int ctr = 0;
    int ctr2 = 0, j;
    float theta = 0.0;
    float theta_diff = 0.05;
    float circ_color[3] = { 0.44f, 0.57f, 0.86f };    

    for(theta = theta_diff; theta <= 2*3.14159 + theta_diff; theta += theta_diff) {
        this->vertex_list[ctr++] = x + (r * cos(theta));
        this->vertex_list[ctr++] = y + (r * sin(theta));
        this->vertex_list[ctr++] = 0.0;
        this->vertex_list[ctr++] = prev_x;
        this->vertex_list[ctr++] = prev_y;
        this->vertex_list[ctr++] = 0.0;
        this->vertex_list[ctr++] = x;
        this->vertex_list[ctr++] = y;
        this->vertex_list[ctr++] = 0.0;

        for(j=0;j<9;j++){
            this->color_list[ctr2] = circ_color[ctr2 % 3];
            ctr2++;
        }
        this->circle_vertices += 3;
        prev_x = this->vertex_list[ctr - 9];
        prev_y = this->vertex_list[ctr - 8];
    }
}

int Obstacle::player_reached_bomb(GameCharacter &player){
    // Handle collision between player and bomb
    float player_x1, player_x2, player_y1, player_y2;
    player_x1 = player.position.x - 0.11f;
    player_y1 = player.position.y + 0.10f;
    player_x2 = player.position.x + 0.10f;
    player_y2 = player.position.y - 0.31f;
    float bomb_x1, bomb_x2, bomb_y1, bomb_y2;
    bomb_x1 = this->position.x - 0.12f;
    bomb_y1 = this->position.y + 0.24f;
    bomb_x2 = this->position.x + 0.12f;
    bomb_y2 = this->position.y - 0.12f;
    
    if ((player_y1 < bomb_y2) || (player_y1 >= bomb_y2 && player_y2 > bomb_y1))
        return 0;
    if ((player_x1 > bomb_x2) || (player_x1 <= bomb_x2 && player_x2 < bomb_x1))
        return 0;
    this->do_display = 0;
    // reduce player health if he hit it
    player.health -= 10;
    return 1;  
}