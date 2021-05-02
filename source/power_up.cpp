#include "power_up.h"
#include "main.h"

GLfloat PowerUp::vertex_list[1200];
GLfloat PowerUp::color_list[1200];

PowerUp::PowerUp(float x, float y) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->do_display = 1;
    this->circle_vertices = 0;
    speed = 1;
    int ctr = 0;
    int ctr2 = 0;
    this->x_length = 0.24f;
    this->y_length = 0.24f;
    
    // Powerup appears like a bright gold coin
    getCircle(0.0f, 0.0f, 0.12f);
    this->object = create3DObject(GL_TRIANGLES, this->circle_vertices, this->vertex_list, this->color_list, GL_FILL);
}

void PowerUp::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void PowerUp::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void PowerUp::tick() {
    this->rotation += speed;
    this->rotation = 0;
}

void PowerUp::getCircle(float x, float y, float r) {
    // Draw full circle with given centre and radius
    float prev_x = (x + r);
    float prev_y = y;
    int ctr = 0;
    int ctr2 = 0, j;
    float theta = 0.0;
    float theta_diff = 0.05;
    float circ_color[3] = { 0.984f, 0.828f, 0.066f };    

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

int PowerUp::player_reached_coin(GameCharacter &player){
    // Handle collision between player and powerup
    float player_x1, player_x2, player_y1, player_y2;
    player_x1 = player.position.x - 0.11f;
    player_y1 = player.position.y + 0.10f;
    player_x2 = player.position.x + 0.10f;
    player_y2 = player.position.y - 0.31f;
    float coin_x1, coin_x2, coin_y1, coin_y2;
    coin_x1 = this->position.x - 0.12f;
    coin_y1 = this->position.y + 0.12f;
    coin_x2 = this->position.x + 0.12f;
    coin_y2 = this->position.y - 0.12f;
    
    if ((player_y1 < coin_y2) || (player_y1 >= coin_y2 && player_y2 > coin_y1))
        return 0;
    if ((player_x1 > coin_x2) || (player_x1 <= coin_x2 && player_x2 < coin_x1))
        return 0;
    this->do_display = 0;
    // Increase player health on reaching powerup
    player.health += 10;
    return 1;  
}