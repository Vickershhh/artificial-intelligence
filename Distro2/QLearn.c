/*
	CSC D84 - Unit 3 - Reinforcement Learning
	
	This file contains stubs for implementing the Q-Learning method
	for reinforcement learning as discussed in lecture. You have to
	complete two versions of Q-Learning.
	
	* Standard Q-Learning, based on a full-state representation and
	  a large Q-Table
	* Feature based Q-Learning to handle problems too big to allow
	  for a full-state representation
	    
	Read the assignment handout carefully, then implement the
	required functions below. Sections where you have to add code
	are marked

	**************
	*** TO DO:
	**************

	If you add any helper functions, make sure you document them
	properly and indicate in the report.txt file what you added.
	
	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Jan. 16
*/

#include "QLearn.h"

void QLearn_update(int s, int a, double r, int s_new, double *QTable)
{
 /*
   This function implementes the Q-Learning update as stated in Lecture. It 
   receives as input a <s,a,r,s'> tuple, and updates the Q-table accordingly.
   
   Your work here is to calculate the required update for the Q-table entry
   for state s, and apply it to the Q-table
     
   The update involves two constants, alpha and lambda, which are defined in QLearn.h - you should not 
   have to change their values. Use them as they are.
     
   Details on how states are used for indexing into the QTable are shown
   below, in the comments for QLearn_action. Be sure to read those as well!
 */
 
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  int max = -100000000;
  for (int i = 0; i< 4; i++){
    if (max < *(QTable+(4*s_new)+i)) {
      max = *(QTable+(4*s_new)+i);
    }
  }
  *(QTable+(4*s)+a) += alpha*(r + lambda*max - *(QTable+(4*s)+a));
}

int QLearn_action(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, double *QTable, int size_X, int graph_size)
{
  /*
     This function decides the action the mouse will take. It receives as inputs
     - The graph - so you can check for walls! The mouse must never move through walls
     - The mouse position
     - The cat position
     - The chees position
     - A 'pct' value in [0,1] indicating the amount of time the mouse uses the QTable to decide its action,
       for example, if pct=.25, then 25% of the time the mouse uses the QTable to choose its action,
       the remaining 75% of the time it chooses randomly among the available actions.
       
     Remember that the training process involves random exploration initially, but as training
     proceeds we use the QTable more and more, in order to improve our QTable values around promising
     actions.
     
     The value of pct is controlled by QLearn_core_GL, and increases with each round of training.
     
     This function *must return* an action index in [0,3] where
        0 - move up
        1 - move right
        2 - move down
        3 - move left
        
     QLearn_core_GL will print a warning if your action makes the mouse cross a wall, or if it makes
     the mouse leave the map - this should not happen. If you see a warning, fix the code in this
     function!
     
   The Q-table has been pre-allocated and initialized to 0. The Q-table has
   a size of
   
        graph_size^3 x 4
        
   This is because the table requires one entry for each possible state, and
   the state is comprised of the position of the mouse, cat, and cheese. 
   Since each of these agents can be in one of graph_size positions, all
   possible combinations yield graph_size^3 states.
   
   Now, for each state, the mouse has up to 4 possible moves (up, right,
   down, and left). We ignore here the fact that some moves are not possible
   from some states (due to walls) - it is up to the QLearn_action() function
   to make sure the mouse never crosses a wall. 
   
   So all in all, you have a big table.
        
   For example, on an 8x8 maze, the Q-table will have a size of
   
       64^3 x 4  entries
       
       with 
       
       size_X = 8		<--- size of one side of the maze
       graph_size = 64		<--- Total number of nodes in the graph
       
   Indexing within the Q-table works as follows:
   
     say the mouse is at   i,j
         the cat is at     k,l
         the cheese is at  m,n
         
     state = (i+(j*size_X)) + ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!
     
     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]
     
     (yes, it's a linear array, no shorcuts with brackets!)
     
     NOTE: There is only one cat and once cheese, so you only need to use cats[0][:] and cheeses[0][:]
   */
  
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/
  int next_move = 0;
  int mouse_x = mouse_pos[0][0];
  int mouse_y = mouse_pos[0][1];
  int mouse_index = mouse_x + mouse_y*size_X;
  int cat_x = cats[0][0];
  int cat_y = cats[0][1];
  int cheese_x = cheeses[0][0];
  int cheese_y = cheeses[0][1];
  int current_state = (mouse_x+(mouse_y*size_X)) + ((cat_x+(cat_y*size_X))*graph_size) + ((cheese_x+(cheese_y*size_X))*graph_size*graph_size);
  int random = rand() % 100;
  int max;
  if (random < pct*100) {
    // Q Learning
    max = -10000000;
    for (int i=0;i<4;i++) {
      if (gr[mouse_index][i] == 1) {
        int value = *(QTable+(4*current_state)+i);
        if (max < value) {
          max = value;
          next_move = i;
        }
      }
    }
  } else {
    //random move
    int random_number = rand() % 4;
    while (gr[mouse_index][random_number] == 0) {
      random_number = rand() % 4;
    }
    next_move = random_number;
  }

  return(next_move);		// <--- of course, you will change this!
}

double QLearn_reward(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function computes and returns a reward for the state represented by the input mouse, cat, and
    cheese position. 
    
    You can make this function as simple or as complex as you like. But it should return positive values
    for states that are favorable to the mouse, and negative values for states that are bad for the 
    mouse.
    
    I am providing you with the graph, in case you want to do some processing on the maze in order to
    decide the reward. 
        
    This function should return a maximim/minimum reward when the mouse eats/gets eaten respectively.      
   */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/ 
  int mouse_x = mouse_pos[0][0];
  int mouse_y = mouse_pos[0][1];
  int cat_x = cats[0][0];
  int cat_y = cats[0][1];
  int cheese_x = cheeses[0][0];
  int cheese_y = cheeses[0][1];

  if ((mouse_x == cheese_x) && (mouse_y == cheese_y)) {
    return 500;
  }
  if ((mouse_x == cat_x) && (mouse_y == cat_y)) {
    return -500;
  }
  
  // --------------------BFS------------------
  int cheese_dist = 0;
  int cat_dist = 0;
  int cat_on_route = 0;
  int step = 0;
  int b = 0;
  int visited[graph_size][1];
  //int queue[graph_size][3];
  int queue[graph_size][4];
  for (int i = 0; i < graph_size; i++) {
 	  visited[i][0] = 0;    
  }

  int currx = mouse_pos[0][0];
  int curry = mouse_pos[0][1];
  visited[currx + curry*size_X][0] = 1;
  queue[step][0] = currx; // x
  queue[step][1] = curry; // y
  queue[step][2] = 0; // distance
  queue[step][3] = 0; // cat on route
  while (b < graph_size) {
	  int distance = queue[b][2];
    int cat_found = queue[b][3];
    if (cheeses[0][0] == currx && cheeses[0][1] == curry) {
      cheese_dist = distance;
      cat_on_route = cat_found;
		}
    if (cats[0][0] == currx && cats[0][1] == curry) {
      cat_dist = distance;
      cat_found = 1;
		}

	  // up
	  if (gr[currx + curry*size_X][0] == 1) {
		  if (visited[currx + (curry - 1)*size_X][0] == 0) {
		    visited[currx + (curry - 1)*size_X][0] = 1;
		    step++;
		    queue[step][0] = currx;
		    queue[step][1] = curry - 1;
		    queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
		  }
	  }
	  // right
	  if (gr[currx + curry*size_X][1] == 1) {
      if (visited[currx + 1 + curry*size_X][0] == 0) {
        visited[currx + 1 + curry*size_X][0] = 1;
        step++;
        queue[step][0] = currx + 1;
		    queue[step][1] = curry;
		    queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  // down
	  if (gr[currx + curry*size_X][2] == 1) {
      if (visited[currx + (curry + 1)*size_X][0] == 0) {
        visited[currx + (curry + 1)*size_X][0] = 1;
        step++;
        queue[step][0] = currx;
        queue[step][1] = curry + 1;
        queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  // left
	  if (gr[currx + curry*size_X][3] == 1) {
      if (visited[currx - 1 + curry*size_X][0] == 0) {
        visited[currx - 1 + curry*size_X][0] = 1;
        step++;
        queue[step][0] = currx - 1;
        queue[step][1] = curry;
        queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  b++;
	  currx = queue[b][0];
	  curry = queue[b][1];
  }

  if (cat_on_route) {
    return(-50/(cat_dist+1) + 75/(cheese_dist+1));
  }
  
  return (-50/(cat_dist+1) + 600/(cheese_dist+1));		// <--- of course, you will change this as well!
}

void feat_QLearn_update(double gr[max_graph_size][4],double weights[25], double reward, int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights associated with your
    features. Unlike standard Q-learning, you don't receive a <s,a,r,s'> tuple, instead,
    you receive the current state (mouse, cats, and cheese potisions), and the reward 
    associated with this action (this is called immediately after the mouse makes a move,
    so implicit in this is the mouse having selected some action)
    
    Your code must then evaluate the update and apply it to the weights in the weight array.    
   */
  
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/     
  int i;
  double features[numFeatures];
  double qsa;
  double next_qsa;
  int next_action;
  maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &next_qsa, &next_action);
  evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X, graph_size);
  qsa = Qsa(weights, features);

  for (i = 0; i < numFeatures; i++) {
    weights[i] = weights[i] + alpha * (reward + (lambda * next_qsa) - qsa) * features[i];
  }
  return;
}

int feat_QLearn_action(double gr[max_graph_size][4],double weights[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, int size_X, int graph_size)
{
  /*
    Similar to its counterpart for standard Q-learning, this function returns the index of the next
    action to be taken by the mouse.
    
    Once more, the 'pct' value controls the percent of time that the function chooses an optimal
    action given the current policy.
    
    E.g. if 'pct' is .15, then 15% of the time the function uses the current weights and chooses
    the optimal action. The remaining 85% of the time, a random action is chosen.
    
    As before, the mouse must never select an action that causes it to walk through walls or leave
    the maze.    
   */

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/        
  int next_move = 0;
  int random = rand() % 100;
  int mouse_index = mouse_pos[0][0] + mouse_pos[0][1]*size_X;
  double qsa;
  int action;
  if (random < pct*100) {
    maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, &qsa, &action);
    next_move = action;
  } else {
    //random move
    int random_number = rand() % 4;
    while (gr[mouse_index][random_number] == 0) {
      random_number = rand() % 4;
    }
    next_move = random_number;
  }

  return next_move;		// <--- replace this while you're at it!
}

void evaluateFeatures(double gr[max_graph_size][4],double features[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
   This function evaluates all the features you defined for the game configuration given by the input
   mouse, cats, and cheese positions. You are free to define up to 25 features. This function will
   evaluate each, and return all the feature values in the features[] array.
   
   Take some time to think about what features would be useful to have, the better your features, the
   smarter your mouse!
   
   Note that instead of passing down the number of cats and the number of cheese chunks (too many parms!)
   the arrays themselves will tell you what are valid cat/cheese locations.
   
   You can have up to 5 cats and up to 5 cheese chunks, and array entries for the remaining cats/cheese
   will have a value of -1 - check this when evaluating your features!
  */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/      
   //initialize feature list
  for (int i = 0; i < numFeatures; i++){
	  features[i] = 0;
  }
  // int dist_to_cat;
  // int dist_to_cheese;
  // int cat_on_route;
  // BFS_dist(gr,mouse_pos,cats,cheeses,size_X,graph_size, &dist_to_cat, &dist_to_cheese, &cat_on_route);
  // features[0] = closest_cat_distance(mouse_pos,cats);
  // features[1] = closest_cheese_distance(mouse_pos,cheeses);

  // features[2] = eaten_cat(mouse_pos, cats);
  // features[3] = eat_cheese(mouse_pos, cheeses);
  //features[4] = 0;
  //features[4] = cat_on_route ? 0 : 20 / (dist_to_cheese + 1);
  features[0] = 1;closest_cat_distance(cats, mouse_pos);
  features[1] = 1;total_cat_distance(cats, mouse_pos);
  features[2] = 1;closest_cheese_distance(cheeses, mouse_pos);
  features[3] = 1;total_cheese_distance(cheeses, mouse_pos);
  features[4] = 1;num_walls(gr, mouse_pos);
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  /***********************************************************************************************
  * TO DO: Complete this function
  ***********************************************************************************************/
  double result = 0;
  int i;
  for (i = 0; i < numFeatures; i++) {
    result += weights[i] * features[i];
  }
  return result;		// <--- stub! compute and return the Qsa value
}

void maxQsa(double gr[max_graph_size][4],double weights[25],int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size, double *maxU, int *maxA)
{
 /*
   Given the state represented by the input positions for mouse, cats, and cheese, this function evaluates
   the Q-value at all possible neighbour states and returns the max. The maximum value is returned in maxU
   and the index of the action corresponding to this value is returned in maxA.
   
   You should make sure the function does not evaluate moves that would make the mouse walk through a
   wall. 
  */
 
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  
  int currx = mouse_pos[0][0];
  int curry = mouse_pos[0][1];
  int i;
  double max = -99999999;
  int action;
  int new_mouse_pos[1][2];
  double features [numFeatures];
  double qsa;
  // up
  if (gr[currx + curry*size_X][0] == 1) {
    new_mouse_pos[0][0] = currx;
    new_mouse_pos[0][1] = curry - 1;
    evaluateFeatures(gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    qsa = Qsa(weights, features);
    if (qsa > max) {
      max = qsa;
      action = 0;
    }
  }
  // right
  if (gr[currx + curry*size_X][1] == 1) {
    new_mouse_pos[0][0] = currx + 1;
    new_mouse_pos[0][1] = curry;
    evaluateFeatures(gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    qsa = Qsa(weights, features);
    if (qsa > max) {
      max = qsa;
      action = 1;
    }
  }
  // down
  if (gr[currx + curry*size_X][2] == 1) {
    new_mouse_pos[0][0] = currx;
    new_mouse_pos[0][1]= curry + 1;
    evaluateFeatures(gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    qsa = Qsa(weights, features);
    if (qsa > max) {
      max = qsa;
      action = 2;
    }
  }
  // left
  if (gr[currx + curry*size_X][3] == 1) {
    new_mouse_pos[0][0] = currx - 1;
    new_mouse_pos[0][1] = curry;
    evaluateFeatures(gr, features, new_mouse_pos, cats, cheeses, size_X, graph_size);
    qsa = Qsa(weights, features);
    if (qsa > max) {
      max = qsa;
      action = 3;
    }
  }
  *maxU = max;
  *maxA = action;
  // if (action > -1) {
  //   *maxU = max;
  //   *maxA = action;
  // } else {
  //   *maxU=0;	// <--- stubs! your code will compute actual values for these two variables!
  //   *maxA=0;
  // }  
  return;
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/
double closest_cat_distance(int mouse_pos[1][2], int cats[5][2]) {
  int min = 1000000;
  int i = 0;
  int distance;
  while (cats[i][0] != -1 && i < 5){
    distance = abs(cats[i][0] - mouse_pos[0][0]) + abs(cats[i][1] - mouse_pos[0][1]);
    if (distance < min) {
      min = distance;
    }
    i++;
  }
  return -10/ (min+1);
}

double closest_cheese_distance(int mouse_pos[1][2], int cheeses[5][2]) {
  int min = 1000000;
  int i = 0;
  int distance;
  while (cheeses[i][0] != -1 && i < 5){
    distance = abs(cheeses[i][0] - mouse_pos[0][0]) + abs(cheeses[i][1] - mouse_pos[0][1]);
    if (distance < min) {
      min = distance;
    }
    i++;
  }
  return 10/(min+1);
}

double eat_cheese(int mouse_pos[1][2], int cheeses[5][2]) {
  int i = 0;
  while (cheeses[i][0] != -1 && i < 5){
    if (cheeses[i][0] == mouse_pos[0][0] && cheeses[i][1] == mouse_pos[0][1]) {
      return 10;
    }
    i++;
  }
  return 0;
}

double eaten_cat(int mouse_pos[1][2], int cats[5][2]) {
  int i = 0;
  while (cats[i][0] != -1 && i < 5){
    if (cats[i][0] == mouse_pos[0][0] && cats[i][1] == mouse_pos[0][1]) {
      return -5;
    }
    i++;
  }
  return 0;
}

void BFS_dist(double gr[max_graph_size][4],int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size, int *dist_to_cat,int *dist_to_cheese, int *cat_on_route) {
   // --------------------BFS------------------
  *dist_to_cheese = 0;
  *dist_to_cat = 0;
  *cat_on_route = 0;
  int step = 0;
  int b = 0;
  int visited[graph_size][1];
  //int queue[graph_size][3];
  int queue[graph_size][4];
  int eaten = 0;
  int ate = 0;
  for (int i = 0; i < graph_size; i++){
 	  visited[i][0] = 0;    
  }
  int currx = mouse_pos[0][0];
  int curry = mouse_pos[0][1];
  visited[currx + curry*size_X][0] = 1;
  queue[step][0] = currx; // x
  queue[step][1] = curry; // y
  queue[step][2] = 0; // distance
  queue[step][3] = 0; // cat on route
  while (b < graph_size) {
	  int distance = queue[b][2];
    int cat_found = queue[b][3];
    int i = 0;
    while (cheeses[i][0] != -1 && i < 5) {
      if (cheeses[i][0] == currx && cheeses[i][1] == curry) {
        if (ate == 0) {
          *dist_to_cheese = distance;
          *cat_on_route = cat_found;
          ate = 1;
        }
      }
      i++;
    }
    int j = 0;
    while (cats[j][0] != -1 && j < 5) {
      if (cats[j][0] == currx && cats[j][1] == curry) {
        if (eaten == 0){
          *dist_to_cat = distance;
          cat_found = 1;
          eaten = 1;
        }
      }
      j++;
    }

	  // up
	  if (gr[currx + curry*size_X][0] == 1) {
		  if (visited[currx + (curry - 1)*size_X][0] == 0) {
		    visited[currx + (curry - 1)*size_X][0] = 1;
		    step++;
		    queue[step][0] = currx;
		    queue[step][1] = curry - 1;
		    queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
		  }
	  }
	  // right
	  if (gr[currx + curry*size_X][1] == 1) {
      if (visited[currx + 1 + curry*size_X][0] == 0) {
        visited[currx + 1 + curry*size_X][0] = 1;
        step++;
        queue[step][0] = currx + 1;
		    queue[step][1] = curry;
		    queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  // down
	  if (gr[currx + curry*size_X][2] == 1) {
      if (visited[currx + (curry + 1)*size_X][0] == 0) {
        visited[currx + (curry + 1)*size_X][0] = 1;
        step++;
        queue[step][0] = currx;
        queue[step][1] = curry + 1;
        queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  // left
	  if (gr[currx + curry*size_X][3] == 1) {
      if (visited[currx - 1 + curry*size_X][0] == 0) {
        visited[currx - 1 + curry*size_X][0] = 1;
        step++;
        queue[step][0] = currx - 1;
        queue[step][1] = curry;
        queue[step][2] = distance + 1;
		    queue[step][3] = cat_found;
      }
	  }
	  b++;
	  currx = queue[b][0];
	  curry = queue[b][1];
  }
}


/***
 * Feature 1
 */
double closest_cat_distance(int cats[5][2], int mouse_pos[1][2]) {
  int closest_cat = -1,
    curr,
    i;

  for (i=0; i<5;i++) {
    if (cats[i][0] != -1 && cats[i][1] != -1) {
      curr = abs(cats[0][0] - mouse_pos[0][1]) + abs(cats[0][1] - mouse_pos[0][1]);
      if (curr < closest_cat) {
        closest_cat = curr;
      }
    }
  }
  return closest_cat;
}


/***
 * Feature 2
 */
double total_cat_distance(int cats[5][2], int mouse_pos[1][2]) {
  int total_cat_dist = 0,
    i;

  for (i=0; i<5;i++) {
    if (cats[i][0] != -1 && cats[i][1] != -1) {
      total_cat_dist += abs(cats[0][0] - mouse_pos[0][1]) + abs(cats[0][1] - mouse_pos[0][1]);
    }
  }
  return total_cat_dist;
}

/***
 * Feature 3
 */
double closest_cheese_distance(int cheeses[5][2], int mouse_pos[1][2]) {
  int closest_cheeses = -1,
    curr,
    i;

  for (i=0; i<5;i++) {
    if (cheeses[i][0] != -1 && cheeses[i][1] != -1) {
      curr = abs(cheeses[0][0] - mouse_pos[0][1]) + abs(cheeses[0][1] - mouse_pos[0][1]);
      if (curr < closest_cheeses) {
        closest_cheeses = curr;
      }
    }
  }
  return closest_cheeses;
}

/***
 * Feature 4
 */
double total_cheese_distance(int cheeses[5][2], int mouse_pos[1][2]) {
  int total_cheese_dist = 0,
    i;

  for (i=0; i<5;i++) {
    if (cheeses[i][0] != -1 && cheeses[i][1] != -1) {
      total_cheese_dist += abs(cheeses[0][0] - mouse_pos[0][0]) + abs(cheeses[0][1] - mouse_pos[0][1]);
    }
  }
  return total_cheese_dist;
}


/***
 * Feature 5
 */
double num_walls(double gr[max_graph_size][4], int mouse_pos[1][2]) {
  int num_walls = 0,
    i;

  for (i=0;i<4;i++) {
    num_walls += gr[mouse_pos[0][0] + 32 * mouse_pos[0][1]][i];
  }
  return num_walls;
}

