#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

    //kwaterion rotacji :(
    glm::quat Rotation;

    /* //ta chcialoby sie
    // euler Angles
    float Yaw;
    float Pitch;
    */

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, PLANET_RADIUS, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        updateCameraVectors();
    }

    // returns the view matrix calculated using Quaternions and the LookAt Matrix
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

        // Calculate new position
        glm::vec3 newPosition = Position + movement * velocity;

        // Adjust the camera position to stay on the planet surface
        newPosition = glm::normalize(newPosition) * PLANET_RADIUS;

        // Calculate the rotation quaternion from old to new position
        glm::quat rotationDelta = glm::rotation(glm::normalize(Position), glm::normalize(newPosition));

        // Apply the rotation to the camera's orientation
        Rotation = rotationDelta * Rotation;

        // Update the position
        Position = newPosition;

        updateCameraVectors();
    }

    // processes input mouse
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // Create quaternions for pitch and yaw rotations
        glm::quat pitchQuat = glm::angleAxis(glm::radians(yoffset), Right);
        glm::quat yawQuat = glm::angleAxis(glm::radians(-xoffset), glm::normalize(Position));

        // Combine rotations
        Rotation = yawQuat * pitchQuat * Rotation;

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
    void updateCameraVectors()
    {
        Front = glm::normalize(glm::rotate(Rotation, glm::vec3(0.0f, 0.0f, -1.0f)));

        Right = glm::normalize(glm::cross(Front, glm::normalize(Position)));

        Up = glm::normalize(glm::cross(Right, Front));
    }
};