/**
 * @file 		breakout_glfw.cpp
 * @author		aTandon
 * @assignment  breakout
 * @brief		GLFW specific code for breakout (GLFW+PSP).
 *
 * @notes
 *			
 */

// ===============================================
// Headers
// ===============================================
#include "breakout.h"

#ifndef PSP

// helper function taken from GLUI library
// used to render a string 
void _glutBitmapString(void *font, char *s) {
    char *p = s;

    while(*p != '\0') {
        glutBitmapCharacter(font, *p);
        p++;
    }
}

int init() {

	static int argc=1;
	static char *argv[1];
	argv[0] = strdup("breakout");
	glutInit(&argc, argv);

	glfwInit(); 
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	if( !glfwOpenWindow(WINDOW_WIDTH,WINDOW_HEIGHT, 0,0,0,0,0,0, GLFW_WINDOW) ) { 
		glfwTerminate(); 
		return 1; 
	} 
	
	glfwSetWindowPos(300,100);
	glfwSetWindowTitle(WINDOW_NAME);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ASPECT, 0, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
	
	// frame information
    startTime = currentTime = glfwGetTime();

    return 0;   //success
}


int deinit() {
	glfwTerminate(); 

    return 0;   //success
}




int render() {
	glClear(GL_COLOR_BUFFER_BIT); 
	
    // boundaries
    setColor(255, 0, 0);
	drawRectangle(0, MARGIN_SIZE-BORDER_SIZE, ASPECT, BORDER_SIZE); 
	drawRectangle(0, 1-BORDER_SIZE, ASPECT, BORDER_SIZE);
	drawRectangle(ASPECT-BORDER_SIZE, MARGIN_SIZE, BORDER_SIZE, 1);

	// wall
	for (int c=0; c<COLS; c++) {
		for (int r=0; r<ROWS; r++) {
			switch(bricks[r][c]){
				case 1: setColor(0,0,055); 	break;
				case 2: setColor(0,0,100); 	break;
				case 3: setColor(0,0,255); 	break;
				case 4: setColor(0,255,0); 	break;	
				case 5: setColor(255,0,0); 	break;
				case 6: setColor(255,255,255); 	break;
			}
			float x = WALL_X + (float)c*(BRICK_WIDTH);
			float y = WALL_Y + (float)r*(BRICK_HEIGHT);
			if (bricks[r][c]) drawRectangle(x, y, BRICK_WIDTH-BRICK_GAP, BRICK_HEIGHT-BRICK_GAP);
		}
	}

	// paddle
    if (paddle.human)
       	setColor(0, 255, 0);
    else 
       	setColor(255, 255, 0);
	drawRectangle(paddle.x, paddle.y, PADDLE_WIDTH, paddle.height); 	

	// ball	
	for(int i=0; i<5; i++){
		if(balls[i].canDraw){
			if(balls[i].isActive) setColor(255, 255, 255);
			else setColor(255, 255, 0);
			drawCircle(balls[i].x, balls[i].y, balls[i].size/2);
		}
	}
	statusMessage();	

	glfwSwapBuffers(); 

	// update frame timer
	previousTime = currentTime;
    currentTime = glfwGetTime();
	dt = clamp (currentTime - previousTime, 0.0f, 0.05f);
	
	return 0;   // success
}

void setColor(int red, int green, int blue, int alpha) {
	glColor4ub(red, green, blue, alpha);
}

void drawRectangle(float x, float y, float width, float height)  {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(width, height, 1);
    glBegin(GL_QUADS);
    	glVertex3f(0, 0, 0);
    	glVertex3f(1, 0, 0);
    	glVertex3f(1, 1, 0);
    	glVertex3f(0, 1, 0);
    glEnd();	
    glPopMatrix();
}
    
void drawCircle(float x, float y, float radius)	{
	int triangleAmount = 20; //# of triangles used to draw circle
	float twicePi = 2.0f * 3.14;
	glPushMatrix();
	glBegin(GL_TRIANGLE_FAN);
	for(int i = 0; i <= triangleAmount;i++) { 
		glVertex2f(
	            x + (radius * cos(i * twicePi / triangleAmount)), 
		    y + (radius * sin(i * twicePi / triangleAmount))
		);
	}
	glEnd();
	glPopMatrix();
}
void statusMessage(void)  {
	// buffer to hold output message
	static char mess[300];

	sprintf(mess, 
		"FPS = %4.0f        Score = %3d    Lives = %1d", 1.0/dt, score, lives);

	setColor(255,255,255);
	glRasterPos2f(0.1,0.01);
    _glutBitmapString(GLUT_BITMAP_9_BY_15, mess);
}


int processInput() {

	if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) {
	    gameState = GAME_QUIT;
	    return 0;
    }

    if (glfwGetKey('A')) {
        paddle.dy = 1.0f;
    } else if (glfwGetKey('Z')) {
        paddle.dy = -1.0f;
    } else {
        paddle.dy = 0.0f;
    }	
    	
    if (glfwGetKey('L')) paddle.human = !paddle.human;
    
    return 0;   //success
}

#endif

