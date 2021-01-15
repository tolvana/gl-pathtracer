#include <iostream>

#include "pathtracer.hpp"
#include "windowcontext.hpp"


int main (int argc, const char ** argv) {

    Scene scene;

    WindowContext window;

    PathTracer pathtracer;
    pathtracer.setScene(scene);

    window.display();

    while (window.pollEvent()) {


        if (pathtracer.shouldSample()) {
            pathtracer.sample();
        }

        window.clear();
        pathtracer.draw();
        window.display();
    }

    return 0;

}
