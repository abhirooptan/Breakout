/**
 * @file 		breakout.cpp
 * @author		aTandon
 * @assignment  breakout
 * @brief		Main code (game loop, global identifiers ..) for breakout (GLFW+PSP).
 *
 * @notes
 *			
 */

// ===============================================
// Headers
// ===============================================

#include "breakout.h"

// ===============================================
// Global identifiers
// ===============================================

// timing information
double dt, startTime, previousTime, currentTime;

int bricks[ROWS][COLS];

// state for main game loop
GameState gameState;
bool done;
int score, lives;

// ball
Ball balls[5];

// paddles
Paddle paddle = {0, 0, 0,0, PADDLE_HEIGHT, false};

// ===============================================
// @function main
// ===============================================

int main() {

 	init();			// initial graphics/sound components
	
	gameState = GAME_INTRO;
	done = false;
        
	while (!done) {
		switch (gameState) {
			
			// Display game intro screen etc
			case(GAME_INTRO): {
				srand(time(NULL));
				gameState = GAME_START;
				printf ("Game state changed from GAME_INTRO to GAME_START\n");
				break;
			}

			// Run setup code needed at start of game 
			case(GAME_START): {
				lives = 3;
				// position bricks
				for(int r=0; r<ROWS; r++){
					for(int c=0; c<COLS; c++){
						bricks[r][c] = 3;
					}
				}
				
				// power up: paddle size increase
				bricks[rand()%ROWS][rand()%COLS] = 4;
				// trap: paddle size decrease
				bricks[rand()%ROWS][rand()%COLS] = 5;
				// trap: ball increase
				bricks[rand()%ROWS][rand()%COLS] = 6;
				
				gameState = LEVEL_START;
				printf ("Game state changed from GAME_START to LEVEL_START\n");
				break;
			}

			// Create level using data in file and start playing level
			case(LEVEL_START): {
				for(int i=0; i<SIZE; ++i){
					balls[i].x 				= fmod(rand(), ASPECT);			// random x position
					balls[i].y 				= (1-MARGIN_SIZE-2*BORDER_SIZE+balls[i].size)/i;
					balls[i].dx 			= 0;
					balls[i].dy 			= 0;
					balls[i].size 			= BALL_SIZE;
					balls[i].speed			= BALL_SPEED;
					balls[i].canDraw 		= false;
					balls[i].isActive 		= false;
					balls[i].toAim			= false;
				}

				// trapped ball
				bricks[2][5] = 0;
				bricks[2][6] = 0;
				bricks[2][7] = 0;
				
                // place trapped ball at center
                balls[1].x = (ASPECT+WALL_WIDTH)/2.0f ;
                balls[1].y = (1-MARGIN_SIZE-2*BORDER_SIZE+balls[1].size)/2.0f;
				balls[1].dx = 1;
				balls[1].dy = 1;
				balls[1].canDraw = true;
				balls[1].isActive = false;

				// game ball
				balls[0].x = (ASPECT)/3.0f ;
                balls[0].y = (1-MARGIN_SIZE-2*BORDER_SIZE+balls[0].size)/2.0f;
				balls[0].dx = 1;
				balls[0].dy = 1;
				balls[0].canDraw = true;
				balls[0].isActive = true;
                
				gameState = LEVEL_PLAY;
				printf ("Game state changed from LEVEL_START to LEVEL_PLAY\n");
				break;
			}

			case(LEVEL_PLAY): {
				render();
				processInput();
				update();
				break;
			}

			// move to next level or game over - TODO
			case(LEVEL_OVER): {
				gameState = GAME_OVER;
				printf ("Game state changed from LEVEL_OVER to GAME_OVER\n");
				break;
			}

			// restart game or quit game - TODO
			case(GAME_OVER): {
				gameState = GAME_QUIT;
				printf ("Game state changed from GAME_OVER to GAME_QUIT\n");
				break;
			}

			// state - GAME_QUIT
			case(GAME_QUIT): {
				printf ("Game state changed to GAME_QUIT\n");
				done = true;
				break;
			}

			default: {
				fprintf (stderr, "Game state %d is not valid.\n", gameState);
				return 1;
			}
		}
	}
	deinit();
	return 0;
}


int update() {

    float sdt = (float) (SPEED*dt);

	int indexToAim = 0;
		
	for(int i=0;i<SIZE;i++){
		for(int j=i;j<SIZE;j++){
			if(!balls[indexToAim].toAim){
				if((balls[indexToAim].x - paddle.x)>(balls[j].x - paddle.x)) indexToAim = j;
			}
		}
		balls[indexToAim].toAim = true;
	}

	// update left paddle
	for(int i=0; i<SIZE; i++){
		if (!paddle.human && balls[i].toAim) {
        		paddle.y = paddle.y - balls[i].y;
        		paddle.y = balls[i].y;
    		}
	}
	paddle.y += sdt*PADDLE_SPEED*paddle.dy;
    paddle.y = clamp(paddle.y, COURT_BOTTOM-BORDER_SIZE, COURT_TOP-paddle.height);

	for(int i=0; i<SIZE; i++){

		if(balls[i].dx >= 0 || (balls[i].x - paddle.x) < 1) balls[i].toAim = false;
		if(!balls[i].isActive || !balls[i].canDraw) balls[i].toAim = false;

    	// update ball
    	balls[i].x += sdt*balls[i].speed*balls[i].dx;
    	balls[i].y += sdt*balls[i].speed*balls[i].dy;
    
    	// collisions: ball-border 
    	if (balls[i].y <= COURT_BOTTOM) {
        	balls[i].dy = -balls[i].dy;
    		balls[i].y += sdt*BALL_SPEED*balls[i].dy;
    	} else if (balls[i].y>=COURT_TOP-balls[i].size) {
        	balls[i].dy = -balls[i].dy;
    		balls[i].y += sdt*BALL_SPEED*balls[i].dy;
    	} else if (balls[i].x+balls[i].size>=ASPECT-BORDER_SIZE){
			balls[i].dx = -balls[i].dx;
    		balls[i].x += sdt*BALL_SPEED*balls[i].dx;
    	}
    
    	// collision: balls[i]-paddle
		if (balls[i].x<PADDLE_WIDTH) {
        	if (balls[i].y+balls[i].size>paddle.y && balls[i].y<paddle.y+paddle.height) {
	    		balls[i].dy = (balls[i].y - (paddle.y+paddle.height/2))/(paddle.height/2);
				balls[i].dx = sqrt(2-(balls[i].y*balls[i].y));
        	} else { //miss
				lives--;
				if(lives > 0) gameState = LEVEL_START;
        		else gameState = LEVEL_OVER;
				break;     
        	}
    	}

		// ball - wall
		if (! checkPointBoxCollision(balls[i].x, balls[i].y, WALL_X-balls[i].size, WALL_Y-balls[i].size, 
			COLS*BRICK_WIDTH+balls[i].size, ROWS*BRICK_HEIGHT+balls[i].size)){
			if(!balls[i].isActive) balls[i].isActive = true;
			continue;
		}
	
		// calculate ball position with respect to blocks row and column
		int ballCol = (int) ((balls[i].x - WALL_X ) / (BRICK_WIDTH));
		int ballRow = (int) ((balls[i].y - WALL_Y ) / (BRICK_HEIGHT));
	
		bool top=false, bottom=false, left=false, right=false;
		float const D=balls[i].size/2.0f;		// temporary values to save on typing 

		// only check the set of 9 possible bricks around the ball current position
		for (int col=ballCol-1; col<=ballCol+1; col++) {
			for (int row=ballRow-1; row<=ballRow+1; row++) {
				// top
				top = checkPointBrickCollision(balls[i].x+D, balls[i].y+2*D, row, col, balls[i]) || top;
				// bottom
				bottom = checkPointBrickCollision(balls[i].x+D, balls[i].y, row, col, balls[i]) || bottom;
				// left
				left = checkPointBrickCollision(balls[i].x, balls[i].y+D, row, col, balls[i]) || left;
				// right
				right = checkPointBrickCollision(balls[i].x+2*D, balls[i].y+D, row, col, balls[i]) || right;
			}
		}

		// Now update ball if found any collisions 
		if (top) {
			balls[i].dy = -balls[i].dy;
			balls[i].y -= D;
		}
		if (bottom) {
			balls[i].dy = -balls[i].dy;
			balls[i].y += D;
		}
		if (left) {
			balls[i].dx = -balls[i].dx;
			balls[i].x += D;
		}
		if (right) {
			balls[i].dx = -balls[i].dx;
			balls[i].x -= D;
		}

		balls[i].y = clamp(balls[i].y,BORDER_SIZE,COURT_TOP-balls[i].size);
	}
    return 0;   // success
}

bool checkPointBoxCollision(float x, float y, float xStart, float yStart, float width, float height) {

	return (x>=xStart && y>=yStart && x<=xStart+width && y<=yStart+height);
}
	
bool checkPointBrickCollision(float x, float y, int row, int col, Ball ball) {
	
	if (row>=0 && row<ROWS && col>=0 && col<COLS && bricks[row][col]>0) {
		float xStart = WALL_X + (float)col*BRICK_WIDTH;
		float yStart = WALL_Y + (float)row*BRICK_HEIGHT;
		if (checkPointBoxCollision(x, y, xStart, yStart, BRICK_WIDTH, BRICK_HEIGHT)) {
		    if(ball.isActive){
				// paddle height increase: power up
				if(bricks[row][col] == 4){
					bricks[row][col] = 0;
					paddle.height += 0.04;
					score += 50;
				}
				// paddle height decrease: trap
				else if(bricks[row][col] == 5){
					bricks[row][col] = 0;
					paddle.height -= 0.06;
				}
				// another ball: trap
				else if(bricks[row][col] == 6){
					bricks[row][col] = 0;
					for(int i=0;i<5;i++){
						if(!balls[i].canDraw){
							// game ball
							balls[i].dx = 1;
							balls[i].dy = 1;
							balls[i].canDraw = true;
							balls[i].isActive = true;
							break;
						}
					}
				}
				else{
					bricks[row][col]--;
					score += 10;
				}
			}
			return true;
		}
	}
	return false;
}
