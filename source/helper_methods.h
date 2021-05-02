#include "main.h"
#include "maze.h"
#include "game_character.h"
#include "button.h"
#include "power_up.h"
#include "obstacle.h"

#ifndef HELPER_METHODS_H
#define HELPER_METHODS_H

void setup_impostor(GameCharacter &impostor, Maze &maze);
void setup_buttons(Button &vaporize_enemy_button, Button &coins_and_bombs_button, Maze &maze);
void spawn_coins_and_bombs(PowerUp (&coin)[5], Obstacle (&bomb)[5], int &active_coins_count, Maze& maze);
void handle_player_and_bomb_collision(GameCharacter &player, Obstacle (&bomb)[5]);
int can_be_accepted(int (&cell_nums)[10], int i);
void handle_player_and_coin_collision(GameCharacter &player, PowerUp (&coin)[5], int &active_coins_count);
void handle_player_and_button_collision(GameCharacter &player, GameCharacter &impostor, Button &vaporize_enemy_button, Button &coins_and_bombs_button, 
                                        PowerUp (&coin)[5], Obstacle (&bomb)[5], int &active_coins_count, Maze &maze);
int player_exit_check(GameCharacter &player, Maze &maze);

#endif // HELPER_METHODS_H
