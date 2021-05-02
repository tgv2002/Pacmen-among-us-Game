#include "game_character.h"
#include "main.h"

GLfloat GameCharacter::vertex_list[660];
GLfloat GameCharacter::color_list[660];

GameCharacter::GameCharacter(float x, float y, int color, int health, int tasks_left, float speed, int in_cell_number) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->color = color;
    this->speed = speed;
    this->health = health;
    this->tasks_left = tasks_left;
    this->x_length = 0.20f;
    this->y_length = 0.42f;
    this->in_cell_number = in_cell_number;
    this->semi_circle_vertices = 0;
    this->impostor_can_change_direction = 1;
    speed = 1;
    int ctr = 0;
    int ctr2 = 0;
    this->do_display = 1;
    
    // Top of player
    getSemiCircle(0.0f, 0.0f, 0.09f);
    for(int i=0;i<(3*this->semi_circle_vertices);i++){
        this->vertex_list[ctr++] = this->tempSemiCircle[i];
        this->color_list[ctr2++] = this->tempSemiCircleColor[i];
    }

    // Body and legs of player
    getRectangle(0.075f, 0.33f, -0.09f, 0.0f, 2);
    for(int i=0;i<18;i++){
        this->vertex_list[ctr++] = this->tempRectangle[i];
        this->color_list[ctr2++] = this->tempRectangleColor[i];
    }

    getRectangle(0.075f, 0.33f, 0.015f, 0.0f, 2);
    for(int i=0;i<18;i++){
        this->vertex_list[ctr++] = this->tempRectangle[i];
        this->color_list[ctr2++] = this->tempRectangleColor[i];
    }

    getRectangle(0.03f, 0.28f, -0.015f, 0.0f, 2);
    for(int i=0;i<18;i++){
        this->vertex_list[ctr++] = this->tempRectangle[i];
        this->color_list[ctr2++] = this->tempRectangleColor[i];
    }

    // Back of player
    getRectangle(0.02f, 0.18f, -0.11f, -0.085f, 0);
    for(int i=0;i<18;i++){
        this->vertex_list[ctr++] = this->tempRectangle[i];
        this->color_list[ctr2++] = this->tempRectangleColor[i];
    }

    // Glass helmet
    getRectangle(0.12f, 0.10f, -0.06f, 0.02f, 1);
    for(int i=0;i<18;i++){
        this->vertex_list[ctr++] = this->tempRectangle[i];
        this->color_list[ctr2++] = this->tempRectangleColor[i];
    }
    this->object = create3DObject(GL_TRIANGLES, 30 + this->semi_circle_vertices, this->vertex_list, this->color_list, GL_FILL);
}

void GameCharacter::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void GameCharacter::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void GameCharacter::tick() {
    this->rotation += speed;
}

void GameCharacter::getSemiCircle(float x, float y, float r) {
    // Obtains semi circle for given centre and radius
    float prev_x = (x + r);
    float prev_y = y;
    int ctr = 0;
    int ctr2 = 0, j;
    float theta = 0.0;
    float theta_diff = 0.05;

    float semi_circ_color[3];

    semi_circ_color[2] = 0.0f;
    if(this->color == 0){
        // player
        semi_circ_color[1] = 1.0f;
        semi_circ_color[0] = 0.0f;
    }
    else{
        // impostor
        semi_circ_color[0] = 1.0f;
        semi_circ_color[1] = 0.0f;        
    }

    for(theta = theta_diff; theta <= 3.19159; theta += theta_diff) {
        this->tempSemiCircle[ctr++] = x + (r * cos(theta));
        this->tempSemiCircle[ctr++] = y + (r * sin(theta));
        this->tempSemiCircle[ctr++] = 0.0;
        this->tempSemiCircle[ctr++] = prev_x;
        this->tempSemiCircle[ctr++] = prev_y;
        this->tempSemiCircle[ctr++] = 0.0;
        this->tempSemiCircle[ctr++] = x;
        this->tempSemiCircle[ctr++] = y;
        this->tempSemiCircle[ctr++] = 0.0;

        for(j=0;j<9;j++){
            this->tempSemiCircleColor[ctr2] = semi_circ_color[ctr2 % 3];
            ctr2++;
        }
        this->semi_circle_vertices += 3;
        prev_x = this->tempSemiCircle[ctr - 9];
        prev_y = this->tempSemiCircle[ctr - 8];
    }
}

int GameCharacter::get_cell_number_of_character(GameCharacter &gc){
    // gets current cell number of game character
    int row_num = (int)((3.0f - gc.position.y) / 0.48f);
    int col_num = (int)((gc.position.x + 3.0f) / 0.25f);
    return ((24*row_num) + col_num);
}

int GameCharacter::move_character_carefully(int direction, Maze &maze, GameCharacter &gc) {
    float new_x = gc.position.x, new_y = gc.position.y;
    float old_x = gc.position.x, old_y = gc.position.y;
    float prev_x1, prev_x2, prev_y1, prev_y2;
    prev_x1 = new_x - 0.11f;
    prev_y1 = new_y + 0.10f;
    prev_x2 = new_x + 0.10f;
    prev_y2 = new_y - 0.31f;
    
    // calculate new position
    if (direction == 1) {
        new_x -= gc.speed;                   // left
    } else if (direction == 2) {
        new_x += gc.speed;                   // right
    } else if (direction == 3) {
        new_y += gc.speed;                   // up
    } else {
        new_y -= gc.speed;                   // down
    }

    int cell1, cell2, maxc, minc;
    float p1_x, p1_y, p2_x, p2_y;
    float PR = 0.00001;
    float x1, y1, x2, y2;
    int is_vertical;

    for(int i=0;i<587;i++) {

        // calculate cells dividied by the wall
        cell1 = maze.complete_wall_list[i][0];
        cell2 = maze.complete_wall_list[i][1];
        maxc = (cell1 > cell2) ? cell1 : cell2;
        minc = (cell1 < cell2) ? cell1 : cell2; 
        
        if((maxc - minc) == 1){
            p1_x = maze.cell[minc][0] + 0.25f;
            p1_y = maze.cell[minc][1];
            p2_x = maze.cell[minc][0] + 0.25f;
            p2_y = maze.cell[minc][1] - 0.48f;
        }
        else {
            p1_x = maze.cell[minc][0];
            p1_y = maze.cell[minc][1] - 0.48f;
            p2_x = maze.cell[minc][0] + 0.25f;
            p2_y = maze.cell[minc][1] - 0.48f;
        }

        x1 = new_x - 0.11f;
        y1 = new_y + 0.10f;
        x2 = new_x + 0.10f;
        y2 = new_y - 0.31f;
    
        is_vertical = (abs(p1_x - p2_x) < PR) ? 1 : 0;

        if(maze.wall_is_visible[i] == 1) {

            // check collisions with wall if wall exists
            if(is_vertical == 0) {
                // Handle collisions if wall is vertical
                if(y1 > p1_y && y2 < p1_y && !(x2 < p1_x || x1 > p2_x)){
                    if(direction == 3) {
                        new_y = p1_y - 0.101f; 
                        // printf("Moved up and hit a wall. my_y1: %f, my_y2: %f, p1_y: %f\n", y1, y2, p1_y);
                    }
                    else if(direction == 4){
                        new_y = p1_y + 0.316f;
                        // printf("Moved down and hit a wall. my_y1: %f, my_y2: %f, p1_y: %f\n", y1, y2, p1_y);
                    }
                    else if(direction == 1 && x1 < p2_x) {
                        new_x = (p2_x + 0.111f); 
                        // printf("** Moved left and hit a wall. my_x1: %f, my_x2: %f, p1_x: %f\n", x1, x2, p2_x);
                    }
                    else if(direction == 2 && x2 > p1_x){
                        new_x = (p1_x - 0.101f); 
                        // printf("** Moved right and hit a wall. my_x1: %f, my_x2: %f, p1_x: %f\n", x1, x2, p1_x);    
                    }
                }
            }
            else {
                // Handle collisions if wall is horizontal
                if(x1 < p1_x && x2 > p1_x && !(y2 > p1_y || y1 < p2_y)){
                    if(direction == 1) {
                        new_x = (p1_x + 0.111f); 
                        // printf("Moved left and hit a wall. my_x1: %f, my_x2: %f, p1_x: %f\n", x1, x2, p1_x);
                    }
                    else if(direction == 2){
                        new_x = (p1_x - 0.101f); 
                        // printf("Moved right and hit a wall. my_x1: %f, my_x2: %f, p1_x: %f\n", x1, x2, p1_x);
                    }
                    else if(direction == 3 && y1 > p2_y){
                        new_y = p2_y - 0.101f;
                        // printf("** Moved up and hit a wall. my_y1: %f, my_y2: %f, p1_y: %f\n", y1, y2, p1_y);
                    }
                    else if(direction == 4 && y2 < p1_y){
                        new_y = p1_y + 0.316f;
                        // printf("** Moved down and hit a wall. my_y1: %f, my_y2: %f, p1_y: %f\n", y1, y2, p1_y);
                    }
                }
            }
        }
        // else {
        //     if(is_vertical == 0){
        //         if(x1 >= p1_x && x2 <= p2_x){
        //             if(direction == 3 && prev_y2 < p1_y && y2 > p1_y) {
        //                 gc.in_cell_number -= 24;
        //             }
        //             else if(direction == 4 && prev_y1 > p1_y && y1 < p1_y){
        //                 gc.in_cell_number += 24;
        //             }
        //         }                
        //     }
        //     else {
        //         if(y1 <= p1_y && y2 >= p2_y){
        //             if(direction == 1 && prev_x2 > p1_x && x2 < p1_x) {
        //                 gc.in_cell_number--;
        //             }
        //             else if(direction == 2 && prev_x1 < p1_x && x1 > p1_x){
        //                 gc.in_cell_number++;
        //             }
        //         }
        //     }
        // }
    }

    // Update new position to fit constraints
    float border_y = 3.0f + (-0.48f * 12) - 0.12f;
    if(new_x < -2.88f)
        new_x = -2.88f;
    if(new_x > 2.88f && new_y > border_y)
        new_x = 2.88f;
    if(new_y > 2.88f)
        new_y = 2.88f;
    if(new_y < border_y)
        new_y = border_y;
    gc.position.x = new_x;
    gc.position.y = new_y;
    // printf("Player: x=%f, y=%f\n", gc.position.x, gc.position.y);
    // printf("Current cell of player is %d\n", gc.in_cell_number);
    return 0;
}

int GameCharacter::move_via_ideal_positions(int direction, int curr_cell, int next_cell, Maze &maze){
    // move the impostor such that he always follows the player and doesn't collide with wall
    float curr_x = this->position.x;
    float curr_y = this->position.y;
    float new_x = maze.ideal_positions[next_cell][0];
    float new_y = maze.ideal_positions[next_cell][1];
    float diff = this->speed, final_diff;
    float PR = 0.000001;
    int flag = 0;

    if(direction == 1) {
        final_diff = (diff > (curr_x - new_x)) ? (curr_x - new_x) : diff;
        if(!(abs(final_diff - diff) < PR)) {
            this->in_cell_number--;
            flag = 1;
        }
        this->position.x -= final_diff;
    }
    else if(direction == 2){
        final_diff = (diff > (new_x - curr_x)) ? (new_x - curr_x) : diff;
        if(!(abs(final_diff - diff) < PR)) {
            this->in_cell_number++;
            flag = 1;
        }
        this->position.x += final_diff;
    }
    else if(direction == 3){
        final_diff = (diff > (new_y - curr_y)) ? (new_y - curr_y) : diff;
        if(!(abs(final_diff - diff) < PR)) {
            this->in_cell_number -= 24;
            flag = 1;
        }
        this->position.y += final_diff;
    }
    else{
        final_diff = (diff > (curr_y - new_y)) ? (curr_y - new_y) : diff;
        if(!(abs(final_diff - diff) < PR)) {
            this->in_cell_number += 24;
            flag = 1;
        }
        this->position.y -= final_diff;
    }

    return flag;
}

int GameCharacter::did_player_hit_impostor(GameCharacter& impostor){
    // check if impostor reached the player
    float player_x1, player_x2, player_y1, player_y2;
    player_x1 = this->position.x - 0.11f;
    player_y1 = this->position.y + 0.10f;
    player_x2 = this->position.x + 0.10f;
    player_y2 = this->position.y - 0.31f;
    float impostor_x1, impostor_x2, impostor_y1, impostor_y2;
    impostor_x1 = impostor.position.x - 0.11f;
    impostor_y1 = impostor.position.y + 0.10f;
    impostor_x2 = impostor.position.x + 0.10f;
    impostor_y2 = impostor.position.y - 0.31f;
    
    if ((player_y1 < impostor_y2) || (player_y1 >= impostor_y2 && player_y2 > impostor_y1))
        return 0;
    if ((player_x1 > impostor_x2) || (player_x1 <= impostor_x2 && player_x2 < impostor_x1))
        return 0;
    if(impostor.do_display == 0)
        return 0;
    return 1;
}

int GameCharacter::chase_player(GameCharacter &player, Maze &maze, GameCharacter &impostor){
    // impostor chases player here, on shortest path, using cell number of player and current cell number. Motion is continuous.
    if(impostor.do_display == 0)
        return -1;

    int player_node = get_cell_number_of_character(player);
    int impostor_node = impostor.in_cell_number;

    if(player_node == impostor_node)
        return 1;
    if(maze.shortest_path_successor[impostor_node][player_node] == -1)
        return -1;
    int next_node = maze.shortest_path_successor[impostor_node][player_node];
    // Evaluating next move's direction
    int direction = 1;
    // printf("Impostor is in cell %d, Next target is %d, player is in cell %d, the direction for next move is %d\n", impostor_node, next_node, player_node, direction);
    int diff = next_node - impostor_node;
    if(diff == 1) {
        direction = 2;
    } else if(diff == 24) {
        direction = 4;
    } else if(diff == -24) {
        direction = 3;
    }
    if(impostor.impostor_can_change_direction == 1){
        impostor.impostor_can_change_direction = 0;
        impostor.impostor_prev_direction = direction;
    }
    int moved_to_next_cell = move_via_ideal_positions(impostor.impostor_prev_direction, impostor_node, next_node, maze);
    if(moved_to_next_cell == 1){
        // Shift direction only when impostor completely enters the new cell completely
        impostor.impostor_can_change_direction = 1;
    }
    return moved_to_next_cell;
}

void GameCharacter::getRectangle(float l, float b, float x, float y, int color) {
    float left_top_x = x;
    float left_top_y = y;
    float right_top_x = x + l;
    float right_top_y = y;
    float left_bottom_x = x;
    float left_bottom_y = y - b;
    float right_bottom_x = x + l;
    float right_bottom_y = y - b;
    int ctr = 0;
    float rect_color[3];

    if(color == 0){
        rect_color[2] = 0.0f;
        if(this->color == 0){
            // player
            rect_color[1] = 0.9f;
            rect_color[0] = 0.0f;
        }
        else{
            // impostor
            rect_color[0] = 0.9f;
            rect_color[1] = 0.0f;        
        }
    }
    else if(color == 2){
        rect_color[2] = 0.0f;
        if(this->color == 0){
            // player
            rect_color[1] = 1.0f;
            rect_color[0] = 0.0f;
        }
        else{
            // impostor
            rect_color[0] = 1.0f;
            rect_color[1] = 0.0f;        
        }       
    }
    else{
        // helmet glass
        rect_color[0] = 0.22f;
        rect_color[1] = 0.69f;
        rect_color[2] = 0.87f;
    }

    // Top left triangle
    this->tempRectangle[ctr++] = left_top_x;
    this->tempRectangle[ctr++] = left_top_y;
    this->tempRectangle[ctr++] = 0.0;
    this->tempRectangle[ctr++] = right_top_x;
    this->tempRectangle[ctr++] = right_top_y;
    this->tempRectangle[ctr++] = 0.0;
    this->tempRectangle[ctr++] = left_bottom_x;
    this->tempRectangle[ctr++] = left_bottom_y;
    this->tempRectangle[ctr++] = 0.0;

    // Bottom right triangle
    this->tempRectangle[ctr++] = left_bottom_x;
    this->tempRectangle[ctr++] = left_bottom_y;
    this->tempRectangle[ctr++] = 0.0;
    this->tempRectangle[ctr++] = right_bottom_x;
    this->tempRectangle[ctr++] = right_bottom_y;
    this->tempRectangle[ctr++] = 0.0;
    this->tempRectangle[ctr++] = right_top_x;
    this->tempRectangle[ctr++] = right_top_y;
    this->tempRectangle[ctr++] = 0.0;

    for(int j=0;j<18;j++){
        this->tempRectangleColor[j] = rect_color[j % 3];
    } 
}

