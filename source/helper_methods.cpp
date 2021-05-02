#include "helper_methods.h"
#include "main.h"

// Contains all helper methods for the game functioning

void setup_impostor(GameCharacter &impostor, Maze &maze){
    // display impostor at random position on screen
    srand(time(0));
    int cell_num = 0;
    while(cell_num % 24 <= 19)
        cell_num = rand() % (312);
    impostor.position.x = maze.ideal_positions[cell_num][0];
    impostor.position.y = maze.ideal_positions[cell_num][1];
    impostor.in_cell_number = cell_num;
}

void setup_buttons(Button &vaporize_enemy_button, Button &coins_and_bombs_button, Maze& maze){
    // set up both buttons (for tasks) at two random positions
    srand(time(0));
    int cell_num = 0, cell_num2 = 0;
    while(cell_num < 240 || cell_num % 24 > 12)
        cell_num = rand() % (312);
    vaporize_enemy_button.position.x = maze.cell[cell_num][0] + 0.12f;
    vaporize_enemy_button.position.y = maze.cell[cell_num][1] - 0.24f;
    while(cell_num2 % 24 <= 15 || abs(cell_num - cell_num2) < 4)
        cell_num2 = rand() % (312);
    coins_and_bombs_button.position.x = maze.cell[cell_num2][0] + 0.12f;
    coins_and_bombs_button.position.y = maze.cell[cell_num2][1] - 0.24f;  
}

int can_be_accepted(int (&cell_nums)[10], int i){
    // helper function to check if given cell is immediate neighbour to existing cells
    int diff;
    for(int j=0;j<i;j++){
        diff = cell_nums[i] - cell_nums[j];
        if(diff == 1 || diff == -1 || diff == 24 || diff == -24 || diff == 0){
            return 0;
        }
    }
    return 1;
}

void spawn_coins_and_bombs(PowerUp (&coin)[5], Obstacle (&bomb)[5], int &active_coins_count, Maze &maze){
    // When button is collected, spawn 5 bombs (obstacles) and 5 coins (powerups) at random locations
    active_coins_count += 5;
    int cell_nums[10]; 

    for(int i=0;i<10;i++){
        cell_nums[i] = rand() % (312);
        while((can_be_accepted(cell_nums, i)) == 0){
            cell_nums[i] = rand() % (312);
        }
    }

    for(int i=0;i<10;i++){
        if(i < 5)
            coin[i] = PowerUp(maze.cell[cell_nums[i]][0] + 0.12f, maze.cell[cell_nums[i]][1] - 0.24f);
        else
            bomb[i-5] = Obstacle(maze.cell[cell_nums[i]][0] + 0.12f, maze.cell[cell_nums[i]][1] - 0.24f);
    }
}

void handle_player_and_bomb_collision(GameCharacter &player, Obstacle (&bomb)[5]){
    // check and update health if player reached bomb
    for(int i=0;i<5;i++){
        if(bomb[i].do_display == 1){
            bomb[i].player_reached_bomb(player);
        }
    }
}

void handle_player_and_coin_collision(GameCharacter &player, PowerUp (&coin)[5], int &active_coins_count){
    // Keep track of number of coins collected by player and if he finished collecting all powerups
    int val;
    for(int i=0;i<5;i++){
        if(coin[i].do_display == 1){
            val = coin[i].player_reached_coin(player);
            if(val == 1){
                active_coins_count--;
            }
        }
    }
    if(active_coins_count == 0){
        player.tasks_left--;
    }
}

void handle_player_and_button_collision(GameCharacter &player, GameCharacter &impostor, Button &vaporize_enemy_button, Button &coins_and_bombs_button,
                                        PowerUp (&coin)[5], Obstacle (&bomb)[5], int &active_coins_count, Maze &maze){
    int val;
    // check if player reached any button and handle each case
    if(vaporize_enemy_button.do_display == 1){
        // vaporize impostor
        vaporize_enemy_button.player_reached_button(player, impostor);
    }
    if(coins_and_bombs_button.do_display == 1){
        // Spawn powerups and obstacles
        val = coins_and_bombs_button.player_reached_button(player, impostor);
        if(val == 1){
            spawn_coins_and_bombs(coin, bomb, active_coins_count, maze);
        }
    }
}

int player_exit_check(GameCharacter &player, Maze& maze){
    // Check if player has reached exit point of maze. If yes, allow him to exit only if he finished both tasks
    float check = 3.0f + (-0.48f * 12) - 0.12f;
    if(player.position.x > 2.88f){
        if(player.tasks_left == 0 && player.position.y <= check){
            return 1;
        }
        player.position.x = 2.88f;
    }
    return 0;
}

