//http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
//https://learnopengl.com/Getting-started/OpenGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <font.h>
#include <shader.h>
#include <camera.h>
#include "texture_loader.h"
#include "buttonBoundingBoxes.h"
#include <objectVertecies.h>

#include <iostream>
#include <string>
#include <vector>

//staandardowy setup OpenGL
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

//ustawienia
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, PLANET_RADIUS, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true, leftMouseButton = false;

//czas symulacji moze bedzie potrzebny jak nie to wywalic
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//freeType font renderer
FontRenderer* fontRenderer;

const unsigned int segments = 64;
const unsigned int rings = 32;
bool drawGridLines = true;

int main()
{
    //boiler code
    //inicjalizacja glfw PO INICJALIZACJI OPENGP!
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //instancja okna
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EXOSKY", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader shader("cubemaps.vs", "cubemaps.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    //definicja obiektu wierzcholkow
    /*
    // floor VAO
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), &groundVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    */

    //HUD VAO
    unsigned int hudVAO, hudVBO;
    glGenVertexArrays(1, &hudVAO);
    glGenBuffers(1, &hudVBO);
    glBindVertexArray(hudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hudVertices), hudVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // sphere VAO
    unsigned int sphereVAO, sphereVBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), &sphereVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    //billboard VAO
    unsigned int billboardVAO, billboardVBO;
    glGenVertexArrays(1, &billboardVAO);
    glGenBuffers(1, &billboardVBO);
    glBindVertexArray(billboardVAO);
    glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), billboardVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //wczytywanie tekstor poczatkowych, potem skybox zrobic dynamicznie:
    //textury
    unsigned int floorTexture = loadTexture("textures\\planet.jpg");
    unsigned int billboardTexture = loadTexture("textures\\mis.png");
    //HUD
    unsigned int hudTexture0 = loadTexture("textures\\hud.png");
    unsigned int hudTexture1 = loadTexture("textures\\hud_download.png");
    unsigned int hudTexture2 = loadTexture("textures\\hud_camera.png");
    unsigned int hudTexture3 = loadTexture("textures\\hud_telescope.png");
    unsigned int hudTexture4 = loadTexture("textures\\hud_rocket.png");

    //tymczasowe tlo / tlo wstepne
    std::vector<std::string> faces
    {
        "textures\\right.bmp",
        "textures\\left.bmp",
        "textures\\top.bmp",
        "textures\\bottom.bmp",
        "textures\\front.bmp",
        "textures\\back.bmp"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //t³o siatki
    faces = std::vector<std::string>
    {
        "textures\\gridRight.png",
        "textures\\gridLeft.png",
        "textures\\gridTop.png",
        "textures\\gridBottom.png",
        "textures\\gridBack.png",
        "textures\\gridFront.png"
    };
    unsigned int gridTexture = loadCubemap(faces);

    // Initialize font rendering
    fontRenderer = new FontRenderer("NunitoSans_7pt-Light.ttf", SCR_WIDTH, SCR_HEIGHT);

    Shader textShader("text.vs", "text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // shader configuration
    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Shader hudShader("HUD.vs", "HUD.fs");

    double xCursorPos = 0, yCursorPos = 0;
    short int HUDscene = 0;

    //glowna petla programu
    while (!glfwWindowShouldClose(window))
    {
        //timestep do jakis symulacji, JEST ZALEZNY OD FPS
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input klawiatura/myszka
        processInput(window);

        // sprawdzanie pozycji myszki
        glfwGetCursorPos(window, &xCursorPos, &yCursorPos);

        if (yCursorPos > MIN_Y && yCursorPos < MAX_Y) {
            if (xCursorPos > boundingBoxes[0] && xCursorPos < boundingBoxes[1]) { HUDscene = 1; }
            else if (xCursorPos > boundingBoxes[2] && xCursorPos < boundingBoxes[3]) { HUDscene = 2; }
            else if (xCursorPos > boundingBoxes[4] && xCursorPos < boundingBoxes[5]) { HUDscene = 3; }
            else if (xCursorPos > boundingBoxes[6] && xCursorPos < boundingBoxes[7]) { HUDscene = 4; }
            else HUDscene = 0;
        } else HUDscene = 0;

        //RENDER
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //view and projection matrices
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 1. skybox, doslownie i w przenosni -------------------
        //zmiana depth passu TYLKO DLA SKYBOX ***
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        // usuwanie translacji (niebo statyczne)
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //skybox grid lines
        if (drawGridLines) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);

            glBindTexture(GL_TEXTURE_CUBE_MAP, gridTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        //koniec zmiany depth passu ***

        // 2. render obiektow nie przexroczystych ---------------
        shader.use();
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setMat4("projection", projection);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        glm::mat4 sphereModel = glm::mat4(1.0f);
        shader.setMat4("model", sphereModel);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glBindVertexArray(sphereVAO);
        for (int i = 0; i <= rings; ++i) {
            glDrawArrays(GL_TRIANGLE_STRIP, i * (segments + 1) * 2, (segments + 1) * 2);
        }
        glBindVertexArray(0);

        //podloga
        /*
        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);*/

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //render text
        /*
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        fontRenderer->RenderText(textShader, "POMOCY", 25.0f, 25.0f, 8.0f, glm::vec3(1.0, 1.0, 1.0));
        */

        // 3. render przexroczystych ----------------------------
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_TRUE);

        //render billboardow
        glm::vec3 billboardPos = glm::vec3(0.0f, 52.5f, 0.0f);
        glm::mat4 billboardModel = glm::mat4(1.0f);
        billboardModel = glm::translate(billboardModel, billboardPos);

        glm::vec3 cameraPos = camera.Position;
        glm::vec3 lookAt = glm::normalize(cameraPos - billboardPos);
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), lookAt));
        glm::vec3 up = glm::normalize(glm::cross(lookAt, right));

        billboardModel[0] = glm::vec4(right, 0.0f);
        billboardModel[1] = glm::vec4(up, 0.0f);
        billboardModel[2] = glm::vec4(lookAt, 0.0f);

        shader.setMat4("model", billboardModel);
        shader.setMat4("view", camera.GetViewMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, billboardTexture);
        glBindVertexArray(billboardVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        //reset transparency
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // 4. Render HUD ----------------------------------------
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        hudShader.use();
        glBindVertexArray(hudVAO);
        //choose current HUD button
        switch (HUDscene)
        {
            case 1:
                glBindTexture(GL_TEXTURE_2D, hudTexture1);
                break;
            case 2:
                glBindTexture(GL_TEXTURE_2D, hudTexture2);
                break;
            case 3:
                glBindTexture(GL_TEXTURE_2D, hudTexture3);
                break;
            case 4:
                glBindTexture(GL_TEXTURE_2D, hudTexture4);
                break;
        default:
            glBindTexture(GL_TEXTURE_2D, hudTexture0);
            break;
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //dealokacja pewnie o czyms zapomne
    //glDeleteVertexArrays(1, &floorVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    //glDeleteBuffers(1, &floorVBO);
    glDeleteBuffers(1, &skyboxVBO);
    delete fontRenderer;

    glfwTerminate();
    return 0;
}

//imput WSAD dodac strzalki
void processInput(GLFWwindow *window)
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

// glfw: zmiana wielkosci okna
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//guziki myszy
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)

        /*
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xCursorPos, yCursorPos;
        glfwGetCursorPos(window, &xCursorPos, &yCursorPos);
        std::cout << yCursorPos << "\n";
        }
    */
}

// glfw: ruch myszy
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
        return;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: zoom myszki do teleskopu
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}