#include "windowcontext.hpp"

#include <iostream>
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;

WindowContext::WindowContext() {

    glewExperimental = true;

    sf::ContextSettings settings;

    settings.depthBits          = 24;
    settings.stencilBits        = 8;
    settings.antialiasingLevel  = 4;
    settings.majorVersion       = 4;
    settings.minorVersion       = 6;

    aspect_ratio_ = static_cast<float>(window_.getSize().x)
                    / static_cast<float>(window_.getSize().y);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW.";
        std::exit(EXIT_FAILURE);
    }

    window_.create(sf::VideoMode(2560, 1440), "PT",
                      sf::Style::Resize, settings);
    window_.setVerticalSyncEnabled(true);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glGenVertexArrays(1, &vertexarray_id_);
    glBindVertexArray(vertexarray_id_);

}

WindowContext::~WindowContext() { glDeleteVertexArrays(1, &vertexarray_id_); }

void WindowContext::display() { window_.display(); }

void WindowContext::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowContext::activate() { window_.setActive(true); }
void WindowContext::deactivate() { window_.setActive(false); }

bool WindowContext::pollEvent() {

    sf::Event event;

    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed
            || (event.type == sf::Event::KeyPressed
                && event.key.code == sf::Keyboard::Escape)) {

            // Should exit main loop
            return false;

        } else if (event.type == sf::Event::Resized) {

            // Resize window, adjust aspect ratio
            aspect_ratio_ = static_cast<float>(event.size.width)
                            / static_cast<float>(event.size.height);
            glViewport(0, 0, event.size.width, event.size.height);
        }

    }

    return true;

}
