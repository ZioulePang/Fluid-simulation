#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Camera.h"
#include "Water.h"
#include "std_image.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec2 bottom = glm::vec2(0.0f, -6.1f);
glm::vec2 left = glm::vec2(3.7,0);
glm::vec2 right = glm::vec2(-1,0);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 9.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float nodesSimTime;
double gravitySimTime;
float collisionTime;
float ForceSimTime;
float densitySimTime;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//control
bool usePoly;
bool useSpiky;
bool useViscosity;
bool useSummary;
bool useSurface;

int main()
{
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    bool show_demo_window = true;
    bool show_another_window = false;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader waterShader("./Shader/test.vs", "./Shader/test.fs");
    
    Water* water = new Water(3, 3, 12);
    water->init();
    //water->caculateSpace(-3,1,-13,1.5,10); // 0-4 ,-1.5 -- -4

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);


    unsigned int waterVAO, waterVBO;
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        double prev = glfwGetTime();
        double now;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, water->vertices.size() * sizeof(Vertex), &water->vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);


        if (usePoly) 
        {
            water->SimulateGravity(0.016);
            now = glfwGetTime();
            gravitySimTime = now - prev;
            prev = now;

            water->SimulateParticles(0.016);
            now = glfwGetTime();
            nodesSimTime = now - prev;
            prev = now;

            water->caculateForce_Poly();
            now = glfwGetTime();
            ForceSimTime = now - prev;
            prev = now;
        }
        else if (useSpiky) 
        {
            water->SimulateGravity(0.016);
            now = glfwGetTime();
            gravitySimTime = now - prev;
            prev = now;

            water->SimulateParticles(0.016);
            now = glfwGetTime();
            nodesSimTime = now - prev;
            prev = now;

            water->caculateForce_Spiky();
            now = glfwGetTime();
            ForceSimTime = now - prev;
            prev = now;
        }
        else if (useViscosity)
        {
            water->SimulateGravity(0.016);
            now = glfwGetTime();
            gravitySimTime = now - prev;
            prev = now;

            water->SimulateParticles(0.016);
            now = glfwGetTime();
            nodesSimTime = now - prev;
            prev = now;

            water->caculateForce_Vis();
            now = glfwGetTime();
            ForceSimTime = now - prev;
            prev = now;
        }
        else if (useSummary)
        {
            water->caculateDensity();
            now = glfwGetTime();
            densitySimTime = now - prev;
            prev = now;

            water->SimulateGravity(0.016);
            now = glfwGetTime();
            gravitySimTime = now - prev;
            prev = now;

            water->caculateForce_Summary();
            now = glfwGetTime();
            ForceSimTime = now - prev;
            prev = now;

            water->SimulateParticles(0.016);
            now = glfwGetTime();
            nodesSimTime = now - prev;
            prev = now;
        }
        else if (useSurface)
        {
            water->caculateDensity();
            now = glfwGetTime();
            densitySimTime = now - prev;
            prev = now;

            water->SimulateGravity(0.016);
            now = glfwGetTime();
            gravitySimTime = now - prev;
            prev = now;

            water->caculateForce_SummaryWithSurface(bottom);
            now = glfwGetTime();
            ForceSimTime = now - prev;
            prev = now;

            water->SimulateParticles(0.016);
            now = glfwGetTime();
            nodesSimTime = now - prev;
            prev = now;

        }

        water->collisionTest(bottom, glm::vec2(0, 1));
        water->BonderyTest(left, glm::vec2(-1, 0));
        water->BonderyTest(right, glm::vec2(1, 0));
        now = glfwGetTime();
        collisionTime = now - prev;
        prev = now;

        for (int i = 0; i < water->vertices.size(); i++)
        {
            water->vertices[i].Position = water->Particles[i]->vertex.Position; 
        }


        waterShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-3, 3, 0));
        waterShader.setMat4("model", model);
        glPointSize(5);
        glBindVertexArray(waterVAO);
        glDrawArrays(GL_POINTS, 0, water->vertices.size());
        glUseProgram(0);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Test"); // Create a window called "Hello, world!" and append into it.
            ImGui::Text("Wind");
            ImGui::Checkbox("Use Poly", &usePoly);
            ImGui::Checkbox("Use Spiky", &useSpiky);
            ImGui::Checkbox("Use Ball Viscosity", &useViscosity);
            ImGui::Checkbox("Use Ball Summary", &useSummary);
            ImGui::Checkbox("Use Ball Surface", &useSurface);
            //ImGui::Checkbox("Use Friction Test", &useFriction);

            if (ImGui::Button("Write new file")) {}
                // Buttons return true when clicked (most widgets return true when edited/activated)

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // input
        processInput(window);
        // render

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}