#include "button.h"
#include "main.h"

GLfloat Button::vertex_list[1200];
GLfloat Button::color_list[1200];

Button::Button(float x, float y, int color) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->color = color;
    this->circle_vertices = 0;
    this->x_length = 0.24f;
    this->y_length = 0.24f;
    this->do_display = 1;
    speed = 1;
    int ctr = 0;
    int ctr2 = 0;
    
    getCircle(0.0f, 0.0f, 0.12f);
    this->object = create3DObject(GL_TRIANGLES, this->circle_vertices, this->vertex_list, this->color_list, GL_FILL);
}

void Button::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Button::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Button::tick() {
    this->rotation += speed;
    this->rotation = 0;
}

void Button::getCircle(float x, float y, float r) {
    // get circle when centre and radius are given
    float prev_x = (x + r);
    float prev_y = y;
    int ctr = 0;
    int ctr2 = 0, j;
    float theta = 0.0;
    float theta_diff = 0.05;
    float circ_color[3] = { 0.439216f, 0.85f, 0.858824f };  

    if(this->color == 0){
        circ_color[0] = 1.0f;
        circ_color[1] = 0.5f;
        circ_color[2] = 0.0f;
    }  

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

int Button::player_reached_button(GameCharacter &player, GameCharacter &impostor){
    // handling collisions when player reached button
    float player_x1, player_x2, player_y1, player_y2;
    player_x1 = player.position.x - 0.11f;
    player_y1 = player.position.y + 0.10f;
    player_x2 = player.position.x + 0.10f;
    player_y2 = player.position.y - 0.31f;
    float button_x1, button_x2, button_y1, button_y2;
    button_x1 = this->position.x - 0.12f;
    button_y1 = this->position.y + 0.12f;
    button_x2 = this->position.x + 0.12f;
    button_y2 = this->position.y - 0.12f;
    
    if ((player_y1 < button_y2) || (player_y1 >= button_y2 && player_y2 > button_y1))
        return 0;
    if ((player_x1 > button_x2) || (player_x1 <= button_x2 && player_x2 < button_x1))
        return 0;
    this->do_display = 0;
    if (this->color == 1){
        // vaporize impostor
        impostor.do_display = 0;
        player.tasks_left--;
    }
    return 1;  
}