#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 25.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 80.0f;
const float PLANET_RADIUS = 52.0f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, PLANET_RADIUS, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system.
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 movement(0.0f);

        if (direction == FORWARD)
            movement += Front;
        if (direction == BACKWARD)
            movement -= Front;
        if (direction == LEFT)
            movement -= Right;
        if (direction == RIGHT)
            movement += Right;

        // Project movement onto the tangent plane of the sphere
        glm::vec3 normalizedPosition = glm::normalize(Position);
        movement = glm::normalize(movement - glm::dot(movement, normalizedPosition) * normalizedPosition);

        Position += movement * velocity;

        // Ensure the camera stays on the planet surface
        Position = glm::normalize(Position) * PLANET_RADIUS;

        updateCameraVectors();
    }

    // processes input received from a mouse input system.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;
        /*
        if (constrainPitch)
        {
            if (Pitch > 179.0f)
                Pitch = 179.0f;
            if (Pitch < -179.0f)
                Pitch = -179.0f;
        }
        */
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Recalculate the camera's direction vectors
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event.
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= 3 * (float)yoffset;
        if (Zoom < 25.0f)
            Zoom = 25.0f;
        if (Zoom > 90.0f)
            Zoom = 90.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Calculate the new Right vector
        Right = glm::normalize(glm::cross(Front, glm::normalize(Position)));

        // Calculate the new Up vector
        Up = glm::normalize(glm::cross(Right, Front));
    }
};