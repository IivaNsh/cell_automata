#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>




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




// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

bool show_demo_window = false;
bool second_window = false;


unsigned int computeSetShader;
unsigned int computeShaderReset;

GLuint texture_id;
unsigned int texture_resolution = 10;

float color_value[3] = {0.1f,0.5f,0.7f};
//std::vector<float> texture_init_data;

int mouse_x = 0, mouse_y = 0;

int s_width = SCR_WIDTH, s_height = SCR_HEIGHT;

bool cell_automata_play_state = false;




void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);



std::string readfile(const char* name){
    std::stringstream s;
    
    std::cout<<"opening file -> "<<name;

    std::ifstream file(name);

    if(!file.is_open()){
        std::cout<<" -> could not open file..\n";
        return "";
    }
    std::cout<<" -> opened -> reading file.. \n";
    while(file){
        char c = file.get();
        s<<c;
        //std::cout<<c;
    }
    return s.str();
}


void initTexture(){
    
    

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture_resolution, texture_resolution, 0, GL_RGBA, GL_FLOAT, NULL);
    //glGenerateMipmap(texture);

}














int main()
{
    


#pragma region INIT

    // glfw: initialize and configure
    // ------------------------------
    
    const char* glsl_version = "#version 430";
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL shaders", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );




    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup ImGui Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);




    









    // build and compile our shader program
    // ------------------------------------


    int success;
    char infoLog[512];
    std::string shader_string;

    //compute shader

    unsigned int compute_shader_code;

    shader_string = readfile("shaders/cell_automata_shader.glsl"); 
    const char* compute_shader_content = shader_string.c_str();

    compute_shader_code = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader_code, 1, &compute_shader_content, NULL);
    glCompileShader(compute_shader_code);
    // check for shader compile errors
    glGetShaderiv(compute_shader_code, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute_shader_code, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // shader Program
    unsigned int computeShader = glCreateProgram();
    glAttachShader(computeShader, compute_shader_code);
    glLinkProgram(computeShader);

    glGetProgramiv(computeShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(computeShader, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }


    unsigned int compute_set_shader_code;

    shader_string = readfile("shaders/cell_automata_set_shader.glsl"); 
    const char* compute_set_shader_content = shader_string.c_str();

    compute_set_shader_code = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_set_shader_code, 1, &compute_set_shader_content, NULL);
    glCompileShader(compute_set_shader_code);
    // check for shader compile errors
    glGetShaderiv(compute_set_shader_code, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute_set_shader_code, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // shader Program
    computeSetShader = glCreateProgram();
    glAttachShader(computeSetShader, compute_set_shader_code);
    glLinkProgram(computeSetShader);

    glGetProgramiv(computeSetShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(computeSetShader, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }





    unsigned int compute_shader_reset_code;

    shader_string = readfile("shaders/cell_automata_shader_reset.glsl"); 
    const char* compute_shader_reset_content = shader_string.c_str();

    compute_shader_reset_code = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader_reset_code, 1, &compute_shader_reset_content, NULL);
    glCompileShader(compute_shader_reset_code);
    // check for shader compile errors
    glGetShaderiv(compute_shader_reset_code, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute_shader_reset_code, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // shader Program
    computeShaderReset = glCreateProgram();
    glAttachShader(computeShaderReset, compute_shader_reset_code);
    glLinkProgram(computeShaderReset);

    glGetProgramiv(computeShaderReset, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(computeShaderReset, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }











    // vertex shader
    //
    //
    //open shader files
    shader_string = readfile("shaders/vertex_shader.vs"); 
    const char* vertex_shader_content = shader_string.c_str();
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_content, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // fragment shader

    //open shader files
    shader_string = readfile("shaders/fragment_shader.frag"); 
    const char* fragment_shader_content = shader_string.c_str();
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_content, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    unsigned int tex_loc = glGetUniformLocation(shaderProgram, "tex");
    glUniform1i(tex_loc, 0); 









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




    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);


    // bind the VAO (it was already bound, but just to demonstrate): seeing as we only have a single VAO we can 
    // just bind it beforehand before rendering the respective triangle; this is another approach.
    //glBindVertexArray(VAO);
   
        
    
    // texture generation
    // ------------------
    //
    //std::cout<<"loading image texture/1.jpg -> ";
    
    initTexture();



    glUseProgram(computeShader);
    glBindImageTexture(0, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
     
    glDispatchCompute(texture_resolution, texture_resolution, 1);
    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);






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

        if(show_demo_window){
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        
        if(second_window){
            static int counter = 0;

            ImGui::Begin("cell automata settings");
            
            ImGui::Text("\tu can play with speed and color");
            
            ImGui::SliderFloat(">speeed<", &speed, 0.0f, 0.5f);

            ImGui::ColorEdit3(">color value<", color_value);

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








        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);





        if(cell_automata_play_state){
            if(delay >= 1.f){
                //compute shader part

                glUseProgram(computeShader);
                glBindImageTexture(0, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
         
                glDispatchCompute(texture_resolution, texture_resolution, 1);

                // make sure writing to image has finished before read
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

                delay = 0;
            }

        }
        



        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0+0);       
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform3f(glGetUniformLocation(shaderProgram, "color"), color_value[0], color_value[1], color_value[2]);


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
    

    glDeleteTextures(1, &texture_id);
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteProgram(computeShader);
    glDeleteProgram(computeSetShader);
    glDeleteProgram(shaderProgram);

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
        second_window = !second_window;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        initTexture();
        glUseProgram(computeShaderReset);
        glBindImageTexture(0, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glDispatchCompute(texture_resolution, texture_resolution, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        cell_automata_play_state = false;

        std::cout<<"texture_reset\n";
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        
        unsigned int putX =  int(float(mouse_x)/float(s_width)*float(texture_resolution)); 
        unsigned int putY =  (texture_resolution-1) - int(float(mouse_y)/float(s_height)*float(texture_resolution)); 

        glUseProgram(computeSetShader);
        glBindImageTexture(0, texture_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glUniform2i(glGetUniformLocation(computeSetShader, "putPos"), putX, putY); 
        glDispatchCompute(texture_resolution, texture_resolution, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        std::cout<<mouse_x<<" "<<mouse_y<<"     "<<putX<<" "<<putY<<"\n";
    
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x = xpos; mouse_y = ypos;
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
