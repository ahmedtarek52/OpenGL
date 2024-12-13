#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#define PI  (22/7.f)


using namespace std;
using namespace glm;

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VBO_Ball, VBO_Paddle, VBO_Bricks, IBO_Bricks, IBO, BasicProgramId;

GLuint modelMatLoc, viewMatLoc, projMatLoc;



struct Brick
{
    bool isActive;
    vec3 position;
    vec3 color;

    Brick(vec3 Position, vec3 Color) : isActive(true), position(Position), color(Color) {}
};

float brickWidth = 0.06f, brickHeight = 0.03f;
int brickRow = 8, brickColumns = 15;
vector<Brick> bricks;

void CreateBricks()
{
    float xOffset = -1.0f + (brickWidth / 2.0f) + 0.05f;
    float yOffset = 1.0f - (brickHeight / 2.0f) - 0.05f;

    for (int i = 0; i < brickRow; ++i)
    {
        for (int j = 0; j < brickColumns; ++j)
        {
            float x = xOffset + (j * (brickWidth + 0.07f));
            float y = yOffset - (i * (brickHeight + 0.05f));

            vec3 position(x, y, 0);
            vec3 color = (i < 2) ? vec3(1.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 0.0f);

            bricks.push_back(Brick(position, color));
        }
    }

    bricks.shrink_to_fit();

    glGenBuffers(1, &VBO_Bricks);
}

void RenderBricks()
{
    for (auto& brick : bricks)
    {
        if (!brick.isActive)
            continue;

        GLfloat brickVertices[] =
        {
            -brickWidth, -brickHeight, 0.0f,  brick.color.r, brick.color.g, brick.color.b, 
             brickWidth, -brickHeight, 0.0f,  brick.color.r, brick.color.g, brick.color.b, 
             brickWidth,  brickHeight, 0.0f,  brick.color.r, brick.color.g, brick.color.b, 
            -brickWidth,  brickHeight, 0.0f,  brick.color.r, brick.color.g, brick.color.b  
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Bricks);
        glBufferData(GL_ARRAY_BUFFER, sizeof(brickVertices), brickVertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        mat4 modelMat = translate(mat4(1.0f), brick.position);
        glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));

        glDrawArrays(GL_QUADS, 0, 4);
    }
}


vec3 startPaddlePosition(0, -0.9f, 0);
vec3 paddlePosition = startPaddlePosition;

float paddleWidth = 0.15f;
float paddleHeight = 0.03f;
float paddleSpeed = 0.1f;

void CreatePaddle()
{
    GLfloat PaddleVertices[] =
    {
        -paddleWidth, -paddleHeight, 0.0f,         0.5f, 0.5f, 0.5f,
        paddleWidth, -paddleHeight, 0.0f,          0.5f, 0.5f, 0.5f,
        paddleWidth, paddleHeight, 0.0f,           0.5f, 0.5f, 0.5f,
        -paddleWidth, paddleHeight, 0.0f,          0.5f, 0.5f, 0.5f

    };


    glGenBuffers(1, &VBO_Paddle);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Paddle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PaddleVertices), PaddleVertices, GL_STATIC_DRAW);

}

void RenderPaddle()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Paddle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    mat4 modelMat = translate(mat4(1.f), paddlePosition);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));

    glDrawArrays(GL_QUADS, 0, 4);
}


vec3 startBallPosition(0, -0.8f, 0);
vec3 startVelocity(0.001f, 0.001f, 0);
vec3 ballPosition= startBallPosition;
vec3 ballVelocity= startVelocity;
float ballRadius = 0.04f;  


void CreateBall()
{
    GLfloat ballVertices[80 * 6]; 

    ballVertices[0] = 0;
    ballVertices[1] = 0;
    ballVertices[2] = 0;
    ballVertices[3] = 1;
    ballVertices[4] = 1;
    ballVertices[5] = 1;

    float x = sizeof(ballVertices) / sizeof(GLfloat);
    for (int i = 6; i < 80 * 6; i += 6)
    {
        ballVertices[i] = cos((i - 6) * 2 * PI / (x - 12)) * ballRadius;
        ballVertices[i + 1] = sin((i - 6) * 2 * PI / (x - 12)) * ballRadius;
        ballVertices[i + 2] = 0;  

        // Color of the ball (R, G, B)
        ballVertices[i + 3] = 1;
        ballVertices[i + 4] = 1;
        ballVertices[i + 5] = 1;
    }

    glGenBuffers(1, &VBO_Ball);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Ball);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ballVertices), ballVertices, GL_STATIC_DRAW);
}

void BindBall()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Ball);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}





void UpdateBall()
{
    ballPosition += ballVelocity;


    if (ballPosition.x - ballRadius < -1.0f || ballPosition.x + ballRadius > 1.0f)
    {
        ballVelocity.x *= -1;
    }
    if (ballPosition.y + ballRadius > 1.0f)
    {
        ballVelocity.y *= -1;
    }

    if (ballPosition.y - ballRadius < paddlePosition.y + paddleHeight &&
        ballPosition.x > paddlePosition.x - paddleWidth &&
        ballPosition.x < paddlePosition.x + paddleWidth)
    {
        ballVelocity.y *= -1;
    }

    for (auto& brick : bricks)
    {
        if (!brick.isActive) continue;

        if (ballPosition.x + ballRadius > brick.position.x - brickWidth &&
            ballPosition.x - ballRadius < brick.position.x + brickWidth &&
            ballPosition.y + ballRadius > brick.position.y - brickHeight &&
            ballPosition.y - ballRadius < brick.position.y + brickHeight)
        {
            ballVelocity.y *= -1;

            int rowIndex = static_cast<int>((1.0f - brick.position.y) / (brickHeight + 0.1f)); 
            if (rowIndex >= 2) 
            {
                brick.isActive = false;
            }

            break;
        }
    }


    // Check if the ball falls below the paddle (game over)
    if (ballPosition.y - ballRadius < -1.0f)
    {
        ballPosition = startBallPosition;
        ballVelocity = startVelocity;
        paddlePosition = startPaddlePosition;
    }

}


bool CheckCollision()
{
    for (const auto& brick : bricks)
    {
        if (brick.isActive)
        {
            return false;
        }
    }
    return true;
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
        //getchar();
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

    CompileShader("VS.glsl", "FS.glsl", BasicProgramId);

    CreatePaddle();
    CreateBricks();
    CreateBall();

    // Get uniform locations
    modelMatLoc = glGetUniformLocation(BasicProgramId, "modelMat");
    viewMatLoc = glGetUniformLocation(BasicProgramId, "viewMat");
    projMatLoc = glGetUniformLocation(BasicProgramId, "projMat");

    // Set the projection matrix
    mat4 projMat = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    mat4 viewMat = lookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));


   
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(BasicProgramId);

    RenderPaddle();
    RenderBricks();
    BindBall();
    mat4 modelMat = translate(mat4(1.0f), ballPosition);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));

    glDrawArrays(GL_TRIANGLE_FAN, 0, 80);

   
}


void Update()
{
    UpdateBall();

    if (CheckCollision())
    {
        ballPosition = startBallPosition;
        ballVelocity = startVelocity;
        paddlePosition = startPaddlePosition;
    }
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
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

            float normalizedMouseX = (static_cast<float>(mousePosition.x) / WIDTH) * 2.0f - 1.0f;

            if (normalizedMouseX - paddleWidth / 2.f > -1.0f && normalizedMouseX + paddleWidth / 2.f < 1.0f)
            {
                paddlePosition.x = normalizedMouseX;
            }
        }


        Update();

        Render();

        window.display();
    }


    return 0;
}
