#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>

using namespace std;
using namespace glm;

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

struct Vertex
{
	vec3 Position;
	vec3 COlor;
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::Lines;


void sierpinski2d(float vertex1[], float vertex2[], float vertex3[], int current_depth)
{
	float next_vertex1[3], next_vertex2[3], next_vertex3[3];


	if (current_depth >= 12)
		return;

	glBegin(GL_LINES);
	glVertex3fv(vertex1);
	glVertex3fv(vertex2);
	glVertex3fv(vertex3);
	glEnd();


	next_vertex1[0] = (vertex1[0] + vertex2[0]) / 2;
	next_vertex1[1] = (vertex1[1] + vertex2[1]) / 2;
	next_vertex1[2] = (vertex1[2] + vertex2[2]) / 2;

	next_vertex3[0] = (vertex2[0] + vertex3[0]) / 2;
	next_vertex3[1] = (vertex2[1] + vertex3[1]) / 2;
	next_vertex3[2] = (vertex2[2] + vertex3[2]) / 2;

	next_vertex2[0] = (vertex1[0] + vertex3[0]) / 2;
	next_vertex2[1] = (vertex1[1] + vertex3[1]) / 2;
	next_vertex2[2] = (vertex1[2] + vertex3[2]) / 2;



	sierpinski2d(next_vertex1, vertex1, next_vertex2, current_depth + 1);
	sierpinski2d(next_vertex1, vertex2, next_vertex3, current_depth + 1);
	sierpinski2d(next_vertex3, vertex3, next_vertex2, current_depth + 1);


}


void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_namering, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_namering);
	glUseProgram(programId);
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);


	glClearColor(0, 0.5, 0.5, 1);

	return 0;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	switch (Current_DrawingMode)
	{
	case Points:
		glPointSize(10);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case Lines:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case FilledTriangle:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
	float x[] = {-1,-1,0};
	float y[] = { 0,1,0 };
	float z[] = { 1,-1,0 };

	sierpinski2d(x, y, z, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Num1)
				{
					Current_DrawingMode = DrawingMode::Points;
				}
				if (event.key.code == sf::Keyboard::Num2)
				{
					Current_DrawingMode = DrawingMode::Lines;
				}
				if (event.key.code == sf::Keyboard::Num3)
				{
					Current_DrawingMode = DrawingMode::FilledTriangle;
				}
				break;
			}
			}
		}

		Render();

		window.display();
	}
	return 0;
}