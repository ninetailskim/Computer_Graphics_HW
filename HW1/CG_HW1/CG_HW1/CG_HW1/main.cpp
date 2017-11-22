#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <GL/glew.h>
#include <freeglut/glut.h>
#include "textfile.h"
#include "glm.h"



#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freeglut.lib")

#ifndef GLUT_WHEEL_UP
# define GLUT_WHEEL_UP   0x0003
# define GLUT_WHEEL_DOWN 0x0004
#endif

#ifndef GLUT_KEY_ESC
# define GLUT_KEY_ESC 0x001B
#endif

#ifndef max
# define max(a,b) (((a)>(b))?(a):(b))
# define min(a,b) (((a)<(b))?(a):(b))
#endif

// Shader attributes
GLint iLocPosition;
GLint iLocColor;
GLint Colorstate;

//char filename[7][40] = { "ColorModels/bunny5KC.obj","ColorModels/blitzcrank_incognito.obj","ColorModels/cow3KC.obj","ColorModels/frog2KC.obj","ColorModels/laurana2KC.obj","ColorModels/teapot4KC.obj","ColorModels/ziggs.obj"};
//char filename[7][40] = { "ColorModels/bunny5KC.obj", "ColorModels/santa7KC.obj", "ColorModels/Nala6KC.obj", "ColorModels/cessna7KC.obj", "ColorModels/horse5KC.obj", "ColorModels/heptoroid8KC.obj", "ColorModels/duck4KC.obj" };
char filename[7][40] = { "ColorModels/armadillo12KC.obj", "ColorModels/al7KC.obj", "ColorModels/brain18KC.obj", "ColorModels/Dino20KC.obj", "ColorModels/laurana2KC.obj", "ColorModels/igea17KC.obj", "ColorModels/teapot4KC.obj" };
GLMmodel* OBJ;
GLfloat* vertices;
GLfloat* colors;

int colorgate = 0;
int gate = 1;
int solide_gate = 0;
int filenameindex = 0;
void traverseColorModel()
{
	int i;
	GLfloat maxVal[3];
	GLfloat minVal[3];

	int indc[3];
	GLfloat tmp;
	colors = (GLfloat*)malloc(sizeof(GLfloat) * OBJ->numtriangles * 9);
	vertices = (GLfloat*)malloc(sizeof(GLfloat)* OBJ->numtriangles * 9);

	// The center position of the model 
	OBJ->position[0] = 0;
	OBJ->position[1] = 0;
	OBJ->position[2] = 0;

	int first = 1;
	for(i=0; i<(int)OBJ->numtriangles; i++)
	{
		// the index of each vertex
		indc[0] = OBJ->triangles[i].vindices[0];
		indc[1] = OBJ->triangles[i].vindices[1];
		indc[2] = OBJ->triangles[i].vindices[2];
				
		if (first){
			maxVal[0] = OBJ->vertices[indc[0] * 3 + 0];
			maxVal[1] = OBJ->vertices[indc[0] * 3 + 1];
			maxVal[2] = OBJ->vertices[indc[0] * 3 + 2];
			minVal[0] = OBJ->vertices[indc[0] * 3 + 0];
			minVal[1] = OBJ->vertices[indc[0] * 3 + 1];
			minVal[2] = OBJ->vertices[indc[0] * 3 + 2];
			first = 0;
		}
		for (int inindc = 0; inindc < 3; inindc++){
			for (int otn = 0; otn < 3; otn++){
				tmp = (OBJ->vertices[indc[inindc] * 3 + otn]);
				maxVal[otn] = max(maxVal[otn],tmp);
				minVal[otn] = min(minVal[otn],tmp);
				vertices[i * 9 + inindc * 3 + otn] = tmp;
				colors[i * 9 + inindc * 3 + otn] = OBJ->colors[indc[inindc] * 3 + otn];
			}
		}
	}

	OBJ->position[0] = (maxVal[0] + minVal[0]) / 2;
	OBJ->position[1] = (maxVal[1] + minVal[1]) / 2;
	OBJ->position[2] = (maxVal[2] + minVal[2]) / 2;

	GLfloat scale0 = maxVal[0] - minVal[0];
	GLfloat scale1 = maxVal[1] - minVal[1];
	GLfloat scale2 = maxVal[2] - minVal[2];	

	GLfloat scalein = max(max(scale0, scale1), scale2) / 2;

	for (i = 0; i < (int)OBJ->numtriangles * 3; i++){
		vertices[i * 3 + 0] = (vertices[i * 3 + 0] - OBJ->position[0]) / scalein;
		vertices[i * 3 + 1] = (vertices[i * 3 + 1] - OBJ->position[1]) / scalein;
		vertices[i * 3 + 2] = (vertices[i * 3 + 2] - OBJ->position[2]) / scalein;
	}
}

void loadOBJModel(int filenameindex)
{
	// read an obj model here
	if(OBJ != NULL){
		free(OBJ);
	}
	OBJ = glmReadOBJ(filename[filenameindex]);
	printf("%s\n", filename);

	// traverse the color model
	traverseColorModel();
}

void onIdle()
{
	glutPostRedisplay();
}

void onDisplay(void)
{
	// clear canvas
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocColor);

	// organize the arrays
	GLfloat* triangle_color = colors;

	GLfloat* triangle_vertex = vertices;

	// bind array pointers to shader
	glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangle_vertex);
	glVertexAttribPointer(   iLocColor, 3, GL_FLOAT, GL_FALSE, 0, triangle_color);
	
	// draw the array we just bound
	if (solide_gate){
		glDrawArrays(GL_TRIANGLES, 0, 3 * OBJ->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else{
		glDrawArrays(GL_TRIANGLES, 0, 3 * OBJ->numtriangles);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glutSwapBuffers();
}

void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled)
{
	glGetShaderiv(shader, GL_COMPILE_STATUS, shaderCompiled);
	if(GL_FALSE == (*shaderCompiled))
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character.
		GLchar *errorLog = (GLchar*) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr, "%s", errorLog);

		glDeleteShader(shader);
		free(errorLog);
	}
}

void setShaders()
{
	GLuint v, f, p;
	char *vs = NULL;
	char *fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shader.vert");
	fs = textFileRead("shader.frag");

	glShaderSource(v, 1, (const GLchar**)&vs, NULL);
	glShaderSource(f, 1, (const GLchar**)&fs, NULL);

	free(vs);
	free(fs);

	// compile vertex shader
	glCompileShader(v);
	GLint vShaderCompiled;
	showShaderCompileStatus(v, &vShaderCompiled);
	if(!vShaderCompiled) system("pause"), exit(123);

	// compile fragment shader
	glCompileShader(f);
	GLint fShaderCompiled;
	showShaderCompileStatus(f, &fShaderCompiled);
	if(!fShaderCompiled) system("pause"), exit(456);

	p = glCreateProgram();

	// bind shader
	glAttachShader(p, f);
	glAttachShader(p, v);

	// link program
	glLinkProgram(p);

	iLocPosition = glGetAttribLocation (p, "av4position");
	iLocColor    = glGetAttribLocation (p, "av3color");
	Colorstate = glGetUniformLocation(p, "Cstate");
	glUseProgram(p);

	glUniform1i(Colorstate, colorgate);
}


void onMouse(int who, int state, int x, int y)
{
	if (gate){
		printf("%18s(): (%d, %d) \n", __FUNCTION__, x, y);

		switch (who)
		{
		case GLUT_LEFT_BUTTON:   printf("left button   "); break;
		case GLUT_MIDDLE_BUTTON: printf("middle button "); break;
		case GLUT_RIGHT_BUTTON:  printf("right button  "); break;
		case GLUT_WHEEL_UP:      printf("wheel up      "); break;
		case GLUT_WHEEL_DOWN:    printf("wheel down    "); break;
		default:                 printf("0x%02X          ", who); break;
		}

		switch (state)
		{
		case GLUT_DOWN: printf("start "); break;
		case GLUT_UP:   printf("end   "); break;
		}

		printf("\n");
	}
}

void onMouseMotion(int x, int y)
{
	if (gate){
		printf("%18s(): (%d, %d) mouse move\n", __FUNCTION__, x, y);
	}
}

void onKeyboard(unsigned char key, int x, int y) 
{
	//printf("%18s(): (%d, %d) key: %c(0x%02X) \n", __FUNCTION__, x, y, key, key);
	switch(key) 
	{
		case GLUT_KEY_ESC: /* the Esc key */ 
			exit(0); 
			break;
		case 'h':
		case 'H':
			printf("-.-.-.-.-.-.-.-.-.-Help menu (H/h).-.-.-.-.-.-.-.-.-.-\n");
			printf(".  W/w -- switch between solid:wired rendering mode. .\n");
			printf("|  Z/z -- move to previous model.                    |\n");
			printf(".  X/x -- move to next model	                     .\n");
			printf("|  M/m -- open or close Mouse callback Messages.     |\n");
			printf(".  C/c -- change color.                              .\n");
			printf("|  S/s -- show author information.                   |\n");
			printf(".-.-.-.-.-..-.-.-.-Help menu (H/h)-.-.-.-.-.-.-.-.-.-.\n");
			break;
		case 'w':
		case 'W':
			solide_gate = !solide_gate;
			if (solide_gate){
				printf("Show Wireframes.\n\n");
			}
			else{
				printf("Show Solide.\n\n");
			}
			//switch between solid:wired rendering mode
			break;
		case 'z':
		case 'Z':
			free(colors);
			free(vertices);
			filenameindex = (filenameindex + 6) % 7;
			loadOBJModel(filenameindex);
			//move to previous model
			break;
		case 'x':
		case 'X':
			free(colors);
			free(vertices);
			filenameindex = (filenameindex + 1) % 7;
			loadOBJModel(filenameindex);

			//move to next model
			break;
		case 'm':
		case 'M':
			gate = !gate;
			if (gate){
				printf("The Mouse callback messages have been opeded.\n\n");
			}
			else{
				printf("The Mouse callback messages have been closed.\n\n");
			}
			break;
		case 's':
		case 'S':
			printf("Coded by Han lei,CVLAB 2016\n\n");
			break;
		case 'c':
		case 'C':
			colorgate = (colorgate + 1) % 4;
			setShaders();
			printf("Change color.\n");
			break;
	}
	printf("\n");
}

void onKeyboardSpecial(int key, int x, int y){
	printf("%18s(): (%d, %d) ", __FUNCTION__, x, y);
	switch(key)
	{
		case GLUT_KEY_LEFT:
			printf("key: LEFT ARROW");
			break;
			
		case GLUT_KEY_RIGHT:
			printf("key: RIGHT ARROW");
			break;

		default:
			printf("key: 0x%02X      ", key);
			break;
	}
	printf("\n");
}

void onWindowReshape(int width, int height)
{
	printf("%18s(): %dx%d\n", __FUNCTION__, width, height);
}

int main(int argc, char **argv) 
{
	// glut init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("105062467 2017-CGHW");

	glewInit();
	if(glewIsSupported("GL_VERSION_2_0")){
		printf("Ready for OpenGL 2.0\n");
	}else{
		printf("OpenGL 2.0 not supported\n");
		system("pause");
		exit(1);
	}

	// load obj models through glm
	loadOBJModel(0);

	// register glut callback functions
	glutDisplayFunc (onDisplay);
	glutIdleFunc    (onIdle);
	glutKeyboardFunc(onKeyboard);
	glutSpecialFunc (onKeyboardSpecial);
	glutMouseFunc   (onMouse);
	glutMotionFunc  (onMouseMotion);
	glutReshapeFunc (onWindowReshape);

	// set up shaders here
	setShaders();
	
	glEnable(GL_DEPTH_TEST);

	// main loop
	glutMainLoop();

	// free
	glmDelete(OBJ);

	return 0;
}

