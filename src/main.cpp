//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <stack>

#include "Cube.h"
#include "Sphere.h"
//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "logger.h"

#define WIDTH     800
#define HEIGHT    600
#define FRAMERATE 60
#define TIME_PER_FRAME_MS  (1.0f/FRAMERATE * 1e3)
#define INDICE_TO_PTR(x) ((void*)(x))
float t = 0;
struct GameObject {
    GLuint VboID = 0;
    Geometry* geometry = nullptr;
    glm::mat4 propagatedMatrix = glm::mat4(1.0f);
    glm::mat4 localMatrix = glm::mat4(1.0f);
    std::vector<GameObject*> children;

};

void draw(GameObject& go, Shader* shader, std::stack<glm::mat4>& matrices) {
    matrices.push(matrices.top() * go.propagatedMatrix);
    glm::mat4 mvp = matrices.top() * go.localMatrix;


    glUseProgram((shader->getProgramID()));
    {
        glBindBuffer(GL_ARRAY_BUFFER, go.VboID);
        GLint uScale = glGetUniformLocation(shader->getProgramID(), "uScale");
        glUniform1f(uScale, 0.5f);
        GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
        glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
        //GLint uTime = glGetUniformLocation(shader->getProgramID(), "uTime");
        //glUniform1f(uTime, 1.0f);
        GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
        glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(vPosition);

        GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");

        glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(go.geometry->getNbVertices() * 3 * sizeof(float))); //Convert an indice to void* : (void*)(x)
        glEnableVertexAttribArray(vColor); //Enable"vColor"
        glDrawArrays(GL_TRIANGLES, 0, go.geometry->getNbVertices());


    }
    glUseProgram(0);

    for (int i = 0; i < go.children.size(); i++)
        draw(*(go.children[i]), shader, matrices);

    matrices.pop();
}

int main(int argc, char* argv[])
{
    ////////////////////////////////////////
    //SDL2 / OpenGL Context initialization : 
    ////////////////////////////////////////

    //Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
        return 0;
    }

    //Create a Window
    SDL_Window* window = SDL_CreateWindow("Fenêtre triangle",                           //Titre
        SDL_WINDOWPOS_UNDEFINED,               //X Position
        SDL_WINDOWPOS_UNDEFINED,               //Y Position
        WIDTH, HEIGHT,                         //Resolution
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

//Initialize OpenGL Version (version 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
    SDL_GLContext context = SDL_GL_CreateContext(window);

    //Tells GLEW to initialize the OpenGL function with this version
    glewExperimental = GL_TRUE;
    glewInit();


    //Start using OpenGL to draw something on screen
    glViewport(0, 0, WIDTH, HEIGHT); //Draw on ALL the screen

    //The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
    glClearColor(0.0, 0.0, 0.0, 1.0); //Full Black

    glEnable(GL_DEPTH_TEST); //Active the depth test

    //TODO
    // 

    /*Sphere sphere(32, 32);
    GLuint vboSphereID;
    glGenBuffers(1, &vboSphereID);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereID);
    glBufferData(GL_ARRAY_BUFFER,sphere.getNbVertices()*(3+3)*sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,sphere.getNbVertices()*3*sizeof(float), sphere.getVertices());
    glBufferSubData(GL_ARRAY_BUFFER, sphere.getNbVertices() * 3 * sizeof(float), sphere.getNbVertices() * 3 * sizeof(float), sphere.getNormals());
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
    Sphere sphere(32, 32);
    GLuint vboSphereID;
    glGenBuffers(1, &vboSphereID);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereID);
    glBufferData(GL_ARRAY_BUFFER, sphere.getNbVertices() * (3 + 3) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere.getNbVertices() * 3 * sizeof(float), sphere.getVertices());
    glBufferSubData(GL_ARRAY_BUFFER, sphere.getNbVertices() * 3 * sizeof(float), sphere.getNbVertices() * 3 * sizeof(float), sphere.getNormals());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GameObject sunGO;
    GameObject earthGO;


    sunGO.children.push_back(&earthGO);
    sunGO.geometry = &sphere;
    sunGO.VboID = vboSphereID;

    earthGO.geometry = &sphere;
    earthGO.VboID = vboSphereID;

    /*float position[] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };
    float color[] =
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    //Création VBO
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    //glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, 3 * (3 + 3) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * 3 * sizeof(float), position);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), 3 * 3 * sizeof(float), color);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);*/
    // 
    // 
    //From here you can load your OpenGL objects, like VBO, Shaders, etc.
    // 
    //TODO
    FILE* vertFile = fopen("Shaders/color.vert", "r");
    FILE* fragFile = fopen("Shaders/color.frag", "r");
    if (vertFile == NULL || fragFile == NULL)
    {
        std::cout << "Ouverture des fichiers Shaders impossible\n";
        return 1;
    }
    Shader* shader = Shader::loadFromFiles(vertFile, fragFile);
    fclose(vertFile);
    fclose(fragFile);
    if (shader == NULL)
    {
        return EXIT_FAILURE;
    }

    bool isOpened = true;

    //Main application loop
    while (isOpened)
    {
        //Time in ms telling us when this frame started. Useful for keeping a fix framerate
        uint32_t timeBegin = SDL_GetTicks();

        //Fetch the SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    isOpened = false;
                    break;
                default:
                    break;
                }
                break;
                //We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
            }
        }

        //Clear the screen : the depth buffer and the color buffer
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



        //TODO rendering

        glClearColor(0.10f, 0.0f, 0.0f, 0.0f);

        glm::mat4 projection(1.0f);
        glm::mat4 view(1.0f);

        std::stack<glm::mat4> matrices;
        matrices.push(projection * view);

        sunGO.localMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
        sunGO.propagatedMatrix = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 1.0f, 0.0f));

        t += 0.05f;

        earthGO.propagatedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(+0.75f, 0.0f, 0.0f));
        earthGO.localMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));


        draw(sunGO, shader, matrices);
        /* glm::mat4 model(1.0f);
         int angle = 60;
         model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
         glm::mat4 mvp = projection * camera * model;*/

         /*glUseProgram((shader->getProgramID()));
         {
             glBindBuffer(GL_ARRAY_BUFFER, vboSphereID);
             GLint uScale = glGetUniformLocation(shader->getProgramID(), "uScale");
             glUniform1f(uScale, 0.5f);
             GLint uMVP = glGetUniformLocation(shader->getProgramID(), "uMVP");
             glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
             //GLint uTime = glGetUniformLocation(shader->getProgramID(), "uTime");
             //glUniform1f(uTime, 1.0f);
             GLint vPosition = glGetAttribLocation(shader->getProgramID(), "vPosition");
             glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

             glEnableVertexAttribArray(vPosition);

             GLint vColor = glGetAttribLocation(shader->getProgramID(), "vColor");

             glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, INDICE_TO_PTR(sphere.getNbVertices()*3*sizeof(float))); //Convert an indice to void* : (void*)(x)
             glEnableVertexAttribArray(vColor); //Enable"vColor"
             glDrawArrays(GL_TRIANGLES, 0, sphere.getNbVertices());


         }
         glUseProgram(0);*/













         //Display on screen (swap the buffer on screen and the buffer you are drawing on)
        SDL_GL_SwapWindow(window);

        //Time in ms telling us when this frame ended. Useful for keeping a fix framerate
        uint32_t timeEnd = SDL_GetTicks();

        //We want FRAMERATE FPS
        if (timeEnd - timeBegin < TIME_PER_FRAME_MS)
            SDL_Delay((uint32_t)(TIME_PER_FRAME_MS)-(timeEnd - timeBegin));
    }

    //Free everything
    if (context != NULL)
        SDL_GL_DeleteContext(context);
    if (window != NULL)
        SDL_DestroyWindow(window);

    return 0;
}
