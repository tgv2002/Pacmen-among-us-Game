#include "maze.h"
#include "main.h"

GLfloat Maze::vertex_list[4000];
GLfloat Maze::color_list[4000];

Maze::Maze(float x, float y) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speed = 1;
    this->maze_vertices = 0;

    // Initialize for dsu
    for(int i=0;i<312;i++) {
        this->set_par[i] = i;
        this->set_size[i] = 1;
    }

    for(int i=0;i<587;i++) {
        this->wall_is_visible[i] = 1;
        this->shuffled_walls[i] = i;
    }

    // Initialization for shortest path calculations
    for(int i=0;i<312;i++){
        for(int j=0;j<312;j++){
            this->shortest_distance[i][j] = (i != j) ? 1e8 : 0;
        }
    }
    
    generateMaze();
    this->object = create3DObject(GL_LINES, this->maze_vertices, this->vertex_list, this->color_list, GL_FILL);
}

void Maze::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Maze::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Maze::tick() {
    this->rotation += speed;
    this->rotation = 0;
}

void Maze::Union(int node_1, int node_2){
    // Merge two components in which these 2 nodes are present
    if(this->set_size[node_1] < this->set_size[node_2]) {
        this->set_par[node_1] = node_2;
        this->set_size[node_2]+=this->set_size[node_1];
    }
    else{
        this->set_par[node_2]=node_1;
        this->set_size[node_1]+=this->set_size[node_2];
    }
}

int Maze::find(int node){
    // Find parent / leader of component of this node
    while(this->set_par[node] != node) {
        node = this->set_par[node];
        this->set_par[node] = this->set_par[this->set_par[node]];
    }
    return node;
}

void Maze::getPointsAndWallsLists(){
    float init_x = -3.0f;
    float x_diff = 0.25f;
    float init_y = 3.0f;
    float y_diff = -0.48f;
    int ctr = 0;
    int row_leng = (int)((-2*init_x)/(x_diff)) + 1;
    int col_leng = (int)((-2*init_y)/(y_diff)) + 2;

    // Get points involved as vertices of cells of maze
    for(int j=0;j<col_leng;j++){
        for(int i=0;i<row_leng;i++){
            this->complete_points_list[ctr][0] = init_x + (i*x_diff);
            this->complete_points_list[ctr][1] = init_y + (j*y_diff);
            ctr++;
        }
    }

    int ctr2 = 0;
    // Map every cell to its cell number
    for(int j=0;j<col_leng-1;j++){
        for(int i=0;i<row_leng-1;i++){
            this->cell[ctr2][0] = init_x + (i*x_diff);
            this->cell[ctr2][1] = init_y + (j*y_diff);
            this->ideal_positions[ctr2][0] = init_x + (i*x_diff) + 0.13f;
            this->ideal_positions[ctr2][1] = init_y + (j*y_diff) - 0.11f;
            ctr2++;
        }
    }   

    ctr = 0;
    // Obtain all walls that make up the grid, all walls are visible initially
    for(int i=0;i<col_leng-1;i++) {
        for(int j=1;j<row_leng-1;j++) {
            this->complete_wall_list[ctr][0] = (i*(row_leng-1)) + j;
            this->complete_wall_list[ctr][1] = (i*(row_leng-1)) + j - 1;
            ctr++;   
        }
    }

    for(int j=0;j<row_leng-1;j++){
        for(int i=1;i<col_leng-1;i++){
            this->complete_wall_list[ctr][0] = (i*(row_leng-1)) + j;
            this->complete_wall_list[ctr][1] = (i*(row_leng-1)) + j - (row_leng - 1);
            ctr++;
        }
    }
}

void Maze::shuffle_walls(){
    // randomly shuffle the order of traversing walls
    srand (time(NULL));  
    int n = 587, j, temp;
    for (int i = n - 1; i > 0; i--) {  
        j = rand() % (i + 1);  
        temp = this->shuffled_walls[i];
        this->shuffled_walls[i] = this->shuffled_walls[j];
        this->shuffled_walls[j] = temp;
    }  
}

 void Maze::compute_all_shortest_paths(){

    // Floyd - Warshall algo: compute shortest distance and shortest path among every pair of cells
    for(int i=0;i<312;i++){
        for(int j=0;j<312;j++){
            this->shortest_path_successor[i][j] = (this->shortest_distance[i][j] == 1e8) ? -1 : j;
        }
    }

    for(int k=0;k<312;k++){
        for(int i=0;i<312;i++){
            for(int j=0;j<312;j++){
                if(this->shortest_distance[i][k] == 1e8 || this->shortest_distance[k][j] == 1e8){
                    continue;
                }
                if(this->shortest_distance[i][j] > this->shortest_distance[i][k] + this->shortest_distance[k][j]){
                    this->shortest_distance[i][j] = this->shortest_distance[i][k] + this->shortest_distance[k][j];
                    this->shortest_path_successor[i][j] = this->shortest_path_successor[i][k];
                }
            }
        }
    }
 };

void Maze::generateMaze(){
    getPointsAndWallsLists();
    shuffle_walls();
    int par_node_1, par_node_2;
    float maze_color[3] = { 0.3f, 0.3f, 1.0f };

    // Randomized kruskal to generate perfect maze
    for(int i=0;i<587;i++){
        par_node_1 = find(this->complete_wall_list[this->shuffled_walls[i]][0]);
        par_node_2 = find(this->complete_wall_list[this->shuffled_walls[i]][1]);
        if(par_node_2 != par_node_1) {
            // Remove walls if both nodes belong to different components, and merge the two components
            this->wall_is_visible[this->shuffled_walls[i]] = 0;
            Union(par_node_1, par_node_2);
        }
    }


    int cell1, cell2, ctr = 0, ctr2 = 0, edges = 0, maxc, minc;
    float p1_x, p1_y, p2_x, p2_y;
    for(int i=0;i<587;i++){
        cell1 = this->complete_wall_list[i][0];
        cell2 = this->complete_wall_list[i][1];
        if(this->wall_is_visible[i] == 1){
            // Draw only visible walls
            maxc = (cell1 > cell2) ? cell1 : cell2;
            minc = (cell1 < cell2) ? cell1 : cell2; 
            this->maze_vertices += 2;
            
            if((maxc - minc) == 1){
                p1_x = this->cell[minc][0] + 0.25f;
                p1_y = this->cell[minc][1];
                p2_x = this->cell[minc][0] + 0.25f;
                p2_y = this->cell[minc][1] - 0.48f;
            }
            else {
                p1_x = this->cell[minc][0];
                p1_y = this->cell[minc][1] - 0.48f;
                p2_x = this->cell[minc][0] + 0.25f;
                p2_y = this->cell[minc][1] - 0.48f;
            }

            this->vertex_list[ctr++] = p1_x;
            this->vertex_list[ctr++] = p1_y;
            this->vertex_list[ctr++] = 0.0f;
            this->vertex_list[ctr++] = p2_x;
            this->vertex_list[ctr++] = p2_y;
            this->vertex_list[ctr++] = 0.0f;

            for(int j=0;j<6;j++) {
                this->color_list[ctr2++] = maze_color[j%3];
            }
        } 
        else {
            // Consider walls as edges in graph for shortest path
            this->shortest_distance[cell1][cell2] = 1;
            this->shortest_distance[cell2][cell1] = 1;
        }
    }

    compute_all_shortest_paths();

    // Construct top end of maze
    this->vertex_list[ctr++] = -3.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 0.0f;

    for(int j=0;j<6;j++) {
        this->color_list[ctr2++] = maze_color[j%3];
    }

    // Construct bottom end of maze
    this->vertex_list[ctr++] = -3.0f;
    this->vertex_list[ctr++] = 3.0f + (-0.48f * 13);
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 3.0f + (-0.48f * 13);
    this->vertex_list[ctr++] = 0.0f;

    for(int j=0;j<6;j++) {
        this->color_list[ctr2++] = maze_color[j%3];
    }

    // Construct left end of maze
    this->vertex_list[ctr++] = -3.0f;
    this->vertex_list[ctr++] = 2.52f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = -3.0f;
    this->vertex_list[ctr++] = 3.0f + (-0.48f * 13);
    this->vertex_list[ctr++] = 0.0f;

    for(int j=0;j<6;j++) {
        this->color_list[ctr2++] = maze_color[j%3];
    }

    // Construct right end of maze
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 0.0f;
    this->vertex_list[ctr++] = 3.0f;
    this->vertex_list[ctr++] = 3.0f + (-0.48f * 12);
    this->vertex_list[ctr++] = 0.0f;

    for(int j=0;j<6;j++) {
        this->color_list[ctr2++] = maze_color[j%3];
    }
    this->maze_vertices += 8;
}