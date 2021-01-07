#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>

//! Wrapper class for SFML-based OpenGL context creation and management.
class WindowContext {

public:

    WindowContext();
    ~WindowContext();

    // Polls events (I/O,  process signals, etc.) of the window.
    // return false if the window should terminate, true otherwise.
    bool pollEvent();

    // Clears OpenGL buffers. Should be called before a frame is rendered.
    void clear();

    // Displays render.
    void display();

    // Sets this window active.
    void activate();

    // Sets this window inactive.
    void deactivate();

private:

    sf::Context context_; ///< SFML context
    sf::Window window_;   ///< SFML window
    float aspect_ratio_;  ///< Aspect ratio of the window

    GLuint vertexarray_id_; ///< OpenGL Vertex Array Object

};
