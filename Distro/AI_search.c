/*
	CSC D84 - Unit 1 - Search

	This file contains stubs for implementing the different search
	algorithms covered in the course. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E., Jul. 15
	Updated: F.J.E., Jan. 18
*/

/**********************************************************************
% COMPLETE THIS TEXT BOX:
%
% 1) Student Name:		
% 2) Student Name:		
%
% 1) Student number:
% 2) Student number:
% 
% 1) UtorID
% 2) UtorID
% 
% We hereby certify that the work contained here is our own
%
% ____________________             _____________________
% (sign with your name)            (sign with your name)
***********************************************************************/

#include "AI_search.h"
typedef struct BFSQueue{
		int totalElements;
		int maxNumofEle;
		int *realQueue;
		int front;
		int rear;
}BFSQueue;

BFSQueue *createQueue(){
	BFSQueue *queue;
	queue->totalElements = 0;
	queue->maxNumofEle = 1024;
	queue->realQueue = (int *)malloc(sizeof(int)*1024);
	queue->front = 0;
	queue->rear = 0;
}

int removeFromQueue(BFSQueue *queue){
	if (queue->totalElements == 0){
		return;
	} else{
		int frontEle = queue->realQueue[queue->front];
		queue->front++;
		queue->totalElements--;
		return frontEle;
	}
}

void addToQueue(BFSQueue *queue, int index) {
	if (queue->maxNumofEle == queue->totalElements) {
		return;
	} else {
		queue->realQueue[queue->rear] = index;
		queue->rear++;
		queue->totalElements++;
	}
}

int isEmpty(BFSQueue *queue){ 
    return queue->totalElements == 0; 
}

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
{
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in AI_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:
	
	The game takes place on a grid of size 32x32, the file board_layout.h specifies the size and defines two
	constants 'size_X' and 'size_Y' for the horizontal and vertical size of the board, respectively. For our
	purposes, the grid of locations is represented by a graph with one node per grid location, so given
	the 32x32 cells, the graph has 1024 nodes. 

	To create a maze, we connect cell locations in the grid in such a way that a) there is a path from any
	grid location to any other grid location (i.e. there are no disconnected subsets of nodes in the graph),
	and b) there are loops.

	Since each node represents a grid location, each node can be connected to up to 4 neighbours in the
	top, right, bottom, and left directions respectively:

			node at (i,j-1)
				^
				|
	(node at i-1, j) <- node at (i,j) -> node at (i+1, j)
				|
				v
			node at (i,j+1)

	The graph is theredore stored as an adjacency list with size 1024 x 4, with one row per node in the
	graph, and 4 columns corresponding to the weight of an edge linking the node with each of its 4
	possible neighbours in the order towp, right, bottom, left (clockwise from top).

	Since all we care is whether nodes are connected. Weights will be either 0 or 1, if the weight is
	1, then the neighbouring nodes are connected, if the weight is 0, they are not. For example, if

	graph[i][0] = 0 
	graph[i][1] = 1
	graph[i][2] = 0
	graph[i][3] = 1

	then node i is connected to the right and left neighbours, but not to top or bottom.

	The index in the graph for the node corresponding to grid location (x,y) is

	index = x + (y*size_X) 		or in this case		index = x + (y*32)

	Conversely, if you have the index and want to figure out the grid location,

	x = index % size_X		or in this case		x = index % 32
	y = index / size_Y		or in this case		y = index / 32

	(all of the above are *integer* operations!)

	A path is a sequence of (x,y) grid locations. We store it using an array of size	
	1024 x 2 (since there are 1024 locations, this is the maximum length of any
	path that visits locations only once).

	Agent locations are coordinate pairs (x,y)	

   Arguments:
		gr[graph_size][4]   - This is an adjacency list for the maze
		path[graph_size][2] - An initially empty path for your code to fill.
				      In this case, empty means all entries are initially -1
		visit_order[size_X][size_Y]	- An array in which your code will store the
						  *order* in which grid locations were 
						  visited during search. For example, while
						  doing BFS, the initial location is the
						  start location, it's visit order is 1.
						  Then the search would expand the immediate
						  neighbours of the initial node in some order,
						  these would get a visit order of 2, 3, 4, and
						  5 respectively, and so on.
					
						  This array will be used to display the search
						  pattern generated by each search method.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- BFS
					mode = 1	- DFS
					mode = 2	- A*

		(*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[10][2], int cats, int cheeses)
				- This is a pointer to one of the heuristic functions you will implement, either H_cost()
				  or H_cost_nokitty(). The driver in AI_search_core_GL will pass the appropriate pointer
				  depending on what search the user wants to run.
			
				  If the mode is 0 or 1, this pointer is NULL

				  * How to call the heuristic function from within this function : *
					- Like any other function:
						h = heuristic( x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses);

   Return values:
		Your search code will directly update data passed-in as arguments:

		- path[graph_size][2]	: Your search code will update this array to contain the path from
					  the mouse to one of the cheese chunks. The order matters, so
					  path[0][:] must be the mouse's current location, path[1][:]
					  is the next move for the mouse. Each successive row will contain
					  the next move toward the cheese, and the path ends at a location
					  whose coordinates correspond to one of the cheese chunks.
					  Any entries beyond that must remain set to -1
		- visit_order[size_X][size_Y] 	:  Your search code will update this array to contain the
						   order in which each location in the grid was expanded
						   during search. This means, when that particular location
						   was checked for being a goal, and if not a goal, had its
						   neighbours added as candidates for future expansion (in
						   whatever order is dictated by the search mode).

						   Note that since there are 1024 locations, the values in 
						   this array must always be in [0, 1023]. The driver code
						   will then display search order as a yellow-colored
						   brightness map where nodes expanded earlier will look
						   brighter.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to carry out the different types of search depending on the
 *		mode.
 *
 *		You can do this by writing code within this single function (and being clever about it!)
 *		Or, you can use this function as a wrapper that calls a different search function
 *		 (BFS, DFS, A*) depending on the mode. Note that in the latter case, you will have
 *	    	  to inform your A* function somehow of what heuristic it's supposed to use.
 *
 *		Visiting Order: When adding the neighbours of a node to your list of candidates for
 *				expansion, do so in the order top, right, bottom, left.
 *
 *		NOTE: Your search functions should be smart enough to not choose a path that goes
 *		      through a cat! this is easily done without any heuristics.
 * 
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

 // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!
	int mouse_x = mouse_loc[0][0];
	int mouse_y = mouse_loc[0][1];
	int mouse_index = (mouse_x + (mouse_y*32));
	int pathHistory[1024][2];
	int neighIndex;
	int curr_step = 1;
	visit_order[mouse_x][mouse_y] = curr_step;
	curr_step++;
	for (int i = 0; i < 1024; i++){
		for (int j = 0; j < 2; j++){
			pathHistory[i][j] = -1;    
		}
	}

	if (mode == 0){
		BFSQueue *queue = createQueue();
		int currentNode;
		addToQueue(queue,mouse_index);
		while (!isEmpty(queue)){
			currentNode = removeFromQueue(queue);
			int mouse_x = currentNode % 32;
			int mouse_y = currentNode / 32;
			visit_order[mouse_x][mouse_y] = curr_step;
			curr_step++;
			if (checkCheese(mouse_x, mouse_y, cheese_loc, cheeses)) {
				break;
			}
			for (int neighOrder = 0; neighOrder < 3; neighOrder++){
				if (gr[index][neighOrder] == 1){
					neighIndex = checkNeighborIndex(neighOrder, mouse_x, mouse_y);
					if (!checkCat(neighIndex,cat_loc, cats)) {
						if (pathHistory[neighIndex][0] != 1){
							pathHistory[neighIndex][0] = 1;
							pathHistory[neighIndex][1] = currentNode;
							addToQueue(queue,neighIndex);
						}
					}
				}
			}
		}
		int reversedPath[graph_size][2];
		reversedPath[0][0] = currentNode % 32;
		reversedPath[0][1] = currentNode / 32;
		int i = 1;
		while (pathHistory[currentNode][1] != -1){
			reversedPath[i][0] = pathHistory[currentNode][1] % 32;
			reversedPath[i][1] = pathHistory[currentNode][1] / 32;
			i++;
			currentNode = pathHistory[currentNode][1];
		}
		i--;
		int j = 0;
		while (i >= 0){
			path[j][0] = reversedPath[i][0];
			path[j][1] = reversedPath[i][1];
			i--;
			j++;
		}
		return;
	}
	return;
}

	
	
	
	
	
// 	if (mode == 0) {
// 		path[0][0] = mouse_loc[0][0];
// 		path[0][1] = mouse_loc[0][1];
// 		int curr_step = 1;
// 		int queue[graph_size][3];
// 		int front = 0;
// 		int rear = 0;
// 		queue[front][0] = mouse_loc[0][0];
// 		queue[front][1] = mouse_loc[0][1];
// 		queue[front][2] = -1;
// 		while (front <= rear){
// 			visit_order[queue[front][0]][queue[front][1]] = curr_step;
// 			if (checkCheese(queue[front][0], queue[front][1], cheese_loc)) {
				// int reversedPath[graph_size][2];
				// int i = 0;
				// while (queue[front][2] != -1){
				// 	reversedPath[i][0] = queue[front][0];
				// 	reversedPath[i][0] = queue[front][1];
				// 	i++;
				// 	front = queue[front][2];
				// }
				// i--;
				// int j = 0;
				// while (i > 0){
				// 	path[i][0] = reversedPath[j][0];
				// 	path[i][1] = reversedPath[j][1];
				// 	i--;
				// 	j++;
				// }
				// return;
// 			}
			
// 			//visit_order[queue[front][0]][queue[front][1]] = curr_step;
// 			int index = queue[front][0] + queue[front][1]*32;
			// if (gr[index][0] == 1){
			// 	if (notInQueue(queue, queue[front][0], queue[front][1] - 1, rear) && notVisited(visit_order, queue[front][0], queue[front][1] - 1)) {
			// 		rear++;
			// 		queue[rear][0] = queue[front][0];
			// 		queue[rear][1] = queue[front][1] - 1;
			// 		queue[rear][2] = front;
			// 	}
			// }
			// if (gr[index][1] == 1){
			// 	if (notInQueue(queue, queue[front][0] + 1, queue[front][1], rear) && notVisited(visit_order, queue[front][0] + 1, queue[front][1])) {
			// 		rear++;
			// 		queue[rear][0] = queue[front][0] + 1;
			// 		queue[rear][1] = queue[front][1];
			// 		queue[rear][2] = front;
			// 	}
			// }
			// if (gr[index][2] == 1){
			// 	if (notInQueue(queue, queue[front][0], queue[front][1] + 1, rear) && notVisited(visit_order, queue[front][0], queue[front][1]) + 1) {
			// 		rear++;
			// 		queue[rear][0] = queue[front][0];
			// 		queue[rear][1] = queue[front][1] + 1;
			// 		queue[rear][2] = front;
			// 	}
			// }
			// if (gr[index][3] == 1){
			// 	if (notInQueue(queue, queue[front][0] - 1,queue[front][1], rear) && notVisited(visit_order,queue[front][0] -1, queue[front][1])) {
			// 		rear++;
			// 		queue[rear][0] = queue[front][0] - 1;
			// 		queue[rear][1] = queue[front][1];
			// 		queue[rear][2] = front;
			// 	}
			// }
// 			curr_step++;
// 			front++;
// 		}
// 		return;
// 	}
// 	// ------------------ DFS -----------------
// 	if (mode == 1) {
// 		// path[0][0] = mouse_loc[0][0];
// 		// path[0][1] = mouse_loc[0][1];
// 		int curr_step = 1;
// 		int stack[graph_size][2];
// 		stack[0][0] = mouse_loc[0][0];
// 		stack[0][1] = mouse_loc[0][1];
// 		int depth = 1;
// 		while (depth > 0) {
// 			int curr_x = stack[depth - 1][0];
// 			int curr_y = stack[depth - 1][1];
// 			if (checkCheese(curr_x, curr_y, cheese_loc)) {
// 				int i = 0;
// 				for (i; i < depth; i++) {
// 					path[i][0] = stack[i][0];
// 					path[i][1] = stack[i][1];
// 				}
// 				return;
// 			}
// 			visit_order[curr_x][curr_y] = curr_step;
// 			int index = curr_x + curr_y*32;
// 			if ((gr[index][0] == 1) && notInStack(stack, curr_x, curr_y - 1, depth) && notVisited(visit_order, curr_x, curr_y - 1)) {
// 				stack[depth][0] = curr_x;
// 				stack[depth][1] = curr_y - 1;
// 				depth++;
// 			} else if ((gr[index][1] == 1) && notInStack(stack, curr_x + 1, curr_y, depth) && notVisited(visit_order, curr_x + 1, curr_y)) {
// 				stack[depth][0] = curr_x + 1;
// 				stack[depth][1] = curr_y;
// 				depth++;
// 			} else if ((gr[index][2] == 1) && notInStack(stack, curr_x, curr_y + 1, depth) && notVisited(visit_order, curr_x, curr_y + 1)) {
// 				stack[depth][0] = curr_x;
// 				stack[depth][1] = curr_y + 1;
// 				depth++;
// 			} else if ((gr[index][3] == 1) && notInStack(stack, curr_x - 1, curr_y, depth) && notVisited(visit_order, curr_x - 1, curr_y)) {
// 				stack[depth][0] = curr_x - 1;
// 				stack[depth][1] = curr_y;
// 				depth++;
// 			} else {
// 				depth--;
// 			}
// 			curr_step++;
// 		}
// 		return;
// 	}
//  return;
// }

int checkNeighborIndex(int neighOrder,int x, int y){
	//top Neighbor
    if (neighOrder == 0){
        return (x + ((y-1)*32));
    }
	//right Neighbor
    if (neighOrder == 1){
        return ((x+1) + (y*32));
    }
	//bottom Neighbor
    if (neighOrder == 2){
        return (x + ((y+1)*32));
    }
	//left Neighbor
    if (neighOrder == 3){
        return ((x-1) + (y*32));
    }
    return -1;
}

int checkCheese(int x, int y, int cheese_loc[10][2], int cheeses) {
	int i = 0;
	while (i < cheeses){
		if ((cheese_loc[i][0] == x) && (cheese_loc[i][1] == y)) {
			return 1;
		}
		i++;
	}
	return 0;
}

// ------------new----------
int checkCat(int neighIndex, int cat_loc[10][2], int cats) {
	int x = neighIndex % 32;
	int y = neighIndex / 32;
	int i = 0;
	while (i < cats){
		if ((cat_loc[i][0] == x) && (cat_loc[i][1] == y)) {
			return 1;
		}
		i++;
	}
	return 0;
}

int notInQueue(int queue[graph_size][3], int x, int y, int rear){
	int i = 0;
	while (i <= rear){
		if ((queue[i][0] == x) && (queue[i][1] == y)){
			return 0;
		}
	}
	return 1;
}

// -------------- new ---------
int notInStack (int stack[graph_size][2], int x, int y, int depth) {
	int i = 0;
	while (i < depth) {
		if ((stack[i][0] == x) && (stack[i][1] == y)) {
			return 0;
		}
	}
	return 1;
}

int notVisited(int visit_order[size_X][size_Y], int x, int y){
	if (visit_order[x][y] > 0){
		return 0;
	} else{
		return 1;
	}
}



int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
 /*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. Which cheese is up to you.
	Whatever you code here, your heuristic must be admissible.

	Input arguments:

		x,y - Location for which this function will compute a heuristic search cost
		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		gr - The graph's adjacency list for the maze

		These arguments are as described in the search() function above
 */

 return(1);		// <-- Evidently you will need to update this.
}

int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
 /*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. 

	However - this time you want your heuristic function to help the mouse avoid being eaten.
	Therefore: You have to somehow incorporate knowledge of the cats' locations into your
	heuristic cost estimate. How well you do this will determine how well your mouse behaves
	and how good it is at escaping kitties.

	This heuristic *does not have to* be admissible.

	Input arguments have the same meaning as in the H_cost() function above.
 */

 return(1);		// <-- Evidently you will need to update this.
}

