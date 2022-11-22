// Cellular Automata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <random>
#include <cassert>
#include <vector>

#include <Windows.h>


//Sand Simulation project, simply has to be a 2d renderer which can render a screen sized quad and put a texture on it,
//essentially postprocessor style thats all.


//version 0.01 
//make a renderer that renders 2d points, increase thickness of points store all values in a grid?
//and then the vertices that control the psitions of these points are in a 2d array whcih i control 
//and i can gridizise eveythin g


//version 0.00001 
//lets make a small opengl program in main only like learn open gl style actually figure out how to edijt etxtures and shit

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

struct pixel {
    float r;
    float g;
    float b;
};

struct particle
{
    pixel rgb;
    float position;
};


class Image
{
public:
    pixel* data;
    int m_width;
    int m_height;

    Image(int w, int h) : m_width{ w }, m_height{ h } {
        data = new pixel[w * h];
    }

    void swap(int i, int j, int ifinal, int jfinal)
    {
        if (i < m_height && j < m_width && ifinal < m_height && jfinal < m_width)
        {
            pixel temp = data[ifinal * m_width + jfinal];
            data[ifinal * m_width + jfinal] = data[i * m_width + j];
            data[i * m_width + j] = temp;
        }
    }

    void internal_swap(int initialID, int finalID)
    {
        if (initialID < m_height *  m_width  && initialID > 0 && finalID < m_height* m_width && initialID > 0)
        {
            pixel temp = data[finalID];
            data[finalID] = data[initialID];
            data[initialID] = temp;
        }
    }

    ~Image()
    {
        delete data;
        data = nullptr;
    }
};

int getindex(int width, int height, int i, int j)
{
    return (i * width + j);
}


//the problem with this method is that, i swwp but then when it iterates through next row, it founds the recently swapped 
//pixel and swaps it again, thus swapping the pixel ro final locstion in one run through
//make a array to store swaps and swap all once thats it.

void swapDeltas(Image* frame, std::vector<long> swaparray)
{
    for (int i = 0; i < swaparray.size(); i+=2)
    {
        frame->internal_swap(swaparray.at(i), swaparray.at(i + 1));
    }
}

std::vector<long> updatepixels(Image* frame)
{
    std::vector<long> swaps = {};
    //i is rows and j is columns
    for (int i = 0; i < frame->m_height; i++)
    {
        for (int j = 0; j < frame->m_width; j++)
        {
           long id = getindex(frame->m_width, frame->m_height, i, j);
           long id_down = getindex(frame->m_width, frame->m_height, i+1, j);
           long id_diag_r = getindex(frame->m_width, frame->m_height, i + 1, j + 1);
           long id_diag_l = getindex(frame->m_width, frame->m_height, i + 1, j - 1);

           if (frame->data[id].r == 1.0f && frame->data[id_down].r == 0.0f)
           {
               swaps.push_back(id);
               swaps.push_back(id_down);
           }
           
           else if (frame->data[id].r == 1.0f && frame->data[id_diag_l].r == 0.0f)
           {
               swaps.push_back(id);
               swaps.push_back(id_diag_l);
           }

           else if (frame->data[id].r == 1.0f && frame->data[id_diag_r].r == 0.0f)
           {
               swaps.push_back(id);
               swaps.push_back(id_diag_r);
           }

        }
    }

    return swaps;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Sand Simulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices1[] = {-1.0f, -1.0f, 0.0f ,  0.0f, 0.0f,
                         1.0f, 1.0f, 0.0f  ,  1.0f, 1.0f,  
                        -1.0f,  1.0f, 0.0f ,  0.0f, 1.0f
                        -1.0f, -1.0f, 0.0f ,   0.0f, 0.0f,
                        1.0f, 1.0f, 0.0f   ,  1.0f, 1.0f, 
                        1.0f, -1.0f , 0.0f ,  1.0f, 0.0f
                        };

    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };


    const char* vertexShaderSource = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 texcoord;"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   texcoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 450 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texcoord;"
        "uniform sampler2D _texture;"
        "void main()\n"
        "{\n"
        "   FragColor = texture(_texture, texcoord);\n"
        "}\n\0";

    unsigned int ebo;
    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
    
    glEnableVertexArrayAttrib(vao, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(0)); //stride is 0, so opengl calcs auto

    glEnableVertexArrayAttrib(vao, 1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));

    glBindVertexArray(0);
    
    unsigned int vshader, fshader, program;
    vshader = glCreateShader(GL_VERTEX_SHADER);
    fshader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vshader, 1, &vertexShaderSource, NULL);
    glCompileShader(vshader);
    glShaderSource(fshader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fshader);

    program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);


    //Texture preparation
    int width, height, nrChannels;
    unsigned char* data = stbi_load("..\\Resources\\roofing_lowres.jpg", &width, &height, &nrChannels, 0);

    Image custom{ 256  , 256 };
    for (int i = 0; i < custom.m_width * custom.m_height; i++)
    {
        custom.data[i].r = 0.0f;
        custom.data[i].g = 0.0f;
        custom.data[i].b = 0.0f;
    }
    

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    for (long i = 0; i < sizeof(data); i++)
    {
        data[i] = (int) data[i] * 0;
    }

    if (data)
    {
       // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, custom.m_width, custom.m_height, 0, GL_RGB, GL_FLOAT, custom.data);
        //If target is GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE or one of the GL_TEXTURE_CUBE_MAP targets, 
        //data is read from data as a sequence of signed or unsigned bytes, shorts, or longs, or single-precision floating-point values, 
        //depending on type.
        //These values are grouped into sets of one, two, three, or four values, depending on format, to form elements. 
        //Each data byte is treated as eight 1-bit elements, with bit ordering determined by GL_UNPACK_LSB_FIRST (see glPixelStore).
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    int i = 0;

    float frame = 0.0;
    float previousframe = 0.0;


    float delta = 0.0;

    double xpos, ypos;

    int randompixelx = 0;
    int randompixely = 0;
    int moveto = 1;

    custom.data[randompixelx * custom.m_height +  randompixely].r = 1.0f;
    custom.data[randompixelx * custom.m_height + randompixely].g = 1.0f;
    custom.data[randompixelx * custom.m_height + randompixely].b = 1.0f;

    //fill all pixls code
    int x = 0, y = 0;


    
    for (int i = 0; i < custom.m_width * custom.m_height; i++)
    {
        if (rand() > 32000)
        {
            custom.data[i].r = 1.0f;
            custom.data[i].g = 1.0f;
            custom.data[i].b = 1.0f;
        }
    }
    
    pixel* flipdatabuffer = new pixel[256*256];
 
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwGetCursorPos(window, &xpos, &ypos);

        frame = glfwGetTime();
        delta = frame - previousframe;
        
        /*
        for (int i = 0; i < custom.m_width * custom.m_height; i++)
        {
            custom.data[i].r = rand() / pow(10, 4);
            custom.data[i].g = rand() / pow(10, 4);
            custom.data[i].b = rand() / pow(10, 4);
        }
        */
      
        /*
        int tempx = randompixelx;
        int tempy = randompixely;
        if (randompixelx > custom.m_height || randompixely > custom.m_width)
        {
            std::cout << "line restart" << std::endl;
            randompixelx = 0;
            randompixely = 0;
        }
        else {
            std::cout << "next x: " << randompixelx << "next y: " << randompixely << std::endl;
            randompixely += moveto;
        }
   
        custom.swap(tempx, tempy, randompixelx, randompixely);
        
        */
       

        //WHY IS EVERY PIXEL IN THE 2D ARRAY DEFINED AS J, I NOT I, J WHAT IS THIS, WHAT EVEN IS THIS HOLY SHIT 
        //WHY IS EVERYTHING FLIPPED?!!!?!??!!?!

        //so the sense is that these aint cooridnaited these are i and j 2d array values , so i is always rows and j is always 
        //columns so i != x because of obvious reasons as i is rows! 


        /*
        
        if (x <(custom.m_width) && y <(custom.m_height))
        {
            if (x < custom.m_width-1)
            {
                x++;
            }
            else
            {
                y++;
                x = 0;
            }
        }
        else {
            x = 0;
            y = 0;
        }

        custom.data[x * custom.m_height + y].r = rand()/pow(10, 4) ;
        custom.data[x * custom.m_height + y].g = rand()/pow(10, 4) ;
        custom.data[x * custom.m_height + y].b = rand()/pow(10, 4) ;

         */


        //particle emitter at a random index
        if (rand() > 30000)
        {
            custom.data[1234].r = 1.0f;
            custom.data[1234].g = 1.0f;
            custom.data[1234].b = 1.0f;
        }


        swapDeltas(&custom, updatepixels(&custom));
      
       

        memcpy(flipdatabuffer, custom.data, static_cast<size_t>(256 * 256));

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, custom.m_width, custom.m_height, 0, GL_RGB, GL_FLOAT, custom.data);
        glBindTexture(GL_TEXTURE_2D, 0);
        //teximage2d will spit out an execeptio if the size of the data does not matach the given dimensions i think

        
        glUseProgram(program);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        previousframe = frame;
    }

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



