#include <gl_deps.h>
#include <stb_image.h>



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <memory>

#include "shader.h"
#include "compute_shader.h"

#include "texture.h"


// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;


bool menu_window_state = false;

std::unique_ptr<ComputeShader> cs_play;
std::unique_ptr<ComputeShader> cs_set;
std::unique_ptr<ComputeShader> cs_reset;
std::unique_ptr<ComputeShader> cs_copy;
std::unique_ptr<Shader> gr_shader;

std::unique_ptr<Texture> texture_front;
std::unique_ptr<Texture> texture_back;


float color_value[3] = {1.0f,1.0f,1.0f};
unsigned int texture_resolution = 20;
//std::vector<float> texture_init_data;

int mouse_x = 0, mouse_y = 0;
bool r_pressed=false;

int s_width = SCR_WIDTH, s_height = SCR_HEIGHT;

bool cell_automata_play_state = false;
int draw_mode = 0;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);


void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}
















int main()
{
    


#pragma region INIT

    // glfw: initialize and configure
    // ------------------------------
    // Initialise GLFW

    if( !glfwInit() )
    {
      std::cout<<"Failed to initialize GLFW\n";
      return -1;
    }
  


    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL shaders", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        const char** errDecr;
        glfwGetError(errDecr);
        std::cout <<errDecr<<"\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );




    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);



    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup ImGui Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);



    cs_play = std::make_unique<ComputeShader>("shaders/base.glsl");

    cs_set = std::make_unique<ComputeShader>("shaders/put.glsl");

    cs_reset = std::make_unique<ComputeShader>("shaders/reset.glsl");

    cs_copy = std::make_unique<ComputeShader>("shaders/copy.glsl");

    gr_shader = std::make_unique<Shader>("shaders/vertex_shader.vs", "shaders/fragment_shader.frag");

    texture_back = std::make_unique<Texture>(texture_resolution);
    texture_front = std::make_unique<Texture>(texture_resolution);



    cs_reset.get()->bind();
    texture_back.get()->compute_bind(0);
    cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);

    cs_reset.get()->bind();
    texture_front.get()->compute_bind(0);
    cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);




    gr_shader.get()->bind();
    gr_shader.get()->setUniformTexure("tex", 0, texture_back.get()->id);









    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    // fill "indices" as needed
    std::vector<unsigned int> indices= {
        0,1,2,2,3,0
    };

    // Generate a buffer for the indices
     GLuint elementbuffer;
     glGenBuffers(1, &elementbuffer);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    float vertices[] = {
        // positions       // texture coords
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f    // top left 
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));

    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));










#pragma endregion
    


    float delay = 0.f;
    float speed = 0.01f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        

        // ImGui render         
        if(menu_window_state){
            static int counter = 0;
            static int resolution = texture_resolution;

            ImGui::Begin("cell automata settings");
            
            ImGui::Text("\tu can play with speed, color, grid resolution");
            
            ImGui::SliderFloat(">speeed<", &speed, 0.0f, 0.5f);

            ImGui::ColorEdit3(">color value<", color_value);

            ImGui::InputInt(">resolution<", &resolution);

            if(resolution != texture_resolution){

                
                texture_resolution = resolution;

                texture_back.release();
                texture_front.release();


                texture_back = std::make_unique<Texture>(texture_resolution);
                texture_front = std::make_unique<Texture>(texture_resolution);

                cs_reset.get()->bind();
                texture_back.get()->compute_bind(0);
                cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);

                cs_reset.get()->bind();
                texture_front.get()->compute_bind(0);
                cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);

                cell_automata_play_state = false;

                std::cout<<"texture new resolution\n";

            }


            ImGui::Text("\tjust a cute buttom :3");

            if(ImGui::Button("cute button :3")){
                counter++;
            }
            ImGui::Text("\tcounter = %d", counter);

            if(ImGui::Button("\treset counter")){
                counter = 0;
            }

            ImGui::Text("\n\nfps = %.3f | ms = %.3f", ImGui::GetIO().Framerate, 1000.f/ImGui::GetIO().Framerate);

            ImGui::End();
        }


        ImGui::Render();    


        //




        if(cell_automata_play_state){
            if(delay >= 1.f){
                //compute shader part

                cs_play.get()->bind();
                texture_back.get()->compute_bind(0);
                texture_front.get()->compute_bind(1);
                cs_play.get()->dispatch(texture_resolution, texture_resolution, 1);

                

                cs_copy.get()->bind();
                texture_front.get()->compute_bind(0);
                texture_back.get()->compute_bind(1);
                cs_copy.get()->dispatch(texture_resolution, texture_resolution, 1);


                delay = 0;

            }

        }





        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader before any calls to glUniform
        gr_shader.get()->bind();
        texture_front.get()->display_bind(0);
        glUniform3f(glGetUniformLocation(gr_shader.get()->id, "color"), color_value[0], color_value[1], color_value[2]);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        
        



        //render ImGui on to screen
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());






        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        sleep(0.01);

        delay += speed;
    }
    

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_Q && action ==GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        cell_automata_play_state = !cell_automata_play_state;
        std::cout<<"play state: "<<cell_automata_play_state<<"\n";
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        menu_window_state = !menu_window_state;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        
        cs_reset.get()->bind();
        texture_back.get()->compute_bind(0);
        cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);

        cs_reset.get()->bind();
        texture_front.get()->compute_bind(0);
        cs_reset.get()->dispatch(texture_resolution, texture_resolution, 1);

        cell_automata_play_state = false;

        std::cout<<"texture_reset\n";
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS){

        draw_mode+=1;
        draw_mode%=2;

    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        
        unsigned int putX =  int(float(mouse_x)/float(s_width)*float(texture_resolution)); 
        unsigned int putY =  (texture_resolution-1) - int(float(mouse_y)/float(s_height)*float(texture_resolution)); 


        cs_set.get()->bind();
        texture_front.get()->compute_bind(0);
        glUniform2i(glGetUniformLocation(cs_set.get()->id, "putPos"), putX, putY); 
        cs_set.get()->dispatch(texture_resolution, texture_resolution, 1);
        
        cs_set.get()->bind();
        texture_back.get()->compute_bind(0);
        glUniform2i(glGetUniformLocation(cs_set.get()->id, "putPos"), putX, putY); 
        cs_set.get()->dispatch(texture_resolution, texture_resolution, 1);
        

        std::cout<<mouse_x<<" "<<mouse_y<<"     "<<putX<<" "<<putY<<"\n";

        r_pressed = true;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){

        r_pressed = false;

    }
    
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x = xpos; mouse_y = ypos;

    if(draw_mode == 1){
        if(r_pressed){

            unsigned int putX =  int(float(mouse_x)/float(s_width)*float(texture_resolution)); 
            unsigned int putY =  (texture_resolution-1) - int(float(mouse_y)/float(s_height)*float(texture_resolution)); 


            cs_set.get()->bind();
            texture_front.get()->compute_bind(0);
            glUniform2i(glGetUniformLocation(cs_set.get()->id, "putPos"), putX, putY); 
            cs_set.get()->dispatch(texture_resolution, texture_resolution, 1);

            cs_set.get()->bind();
            texture_back.get()->compute_bind(0);
            glUniform2i(glGetUniformLocation(cs_set.get()->id, "putPos"), putX, putY); 
            cs_set.get()->dispatch(texture_resolution, texture_resolution, 1);


            std::cout<<mouse_x<<" "<<mouse_y<<"     "<<putX<<" "<<putY<<"\n";
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    s_width = width;
    s_height =  height;
    glViewport(0, 0, width, height);
}
