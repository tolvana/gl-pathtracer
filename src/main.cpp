#include "pathtracer.hpp"
#include "windowcontext.hpp"


int main (int argc, const char ** argv) {

    Scene scene;

    WindowContext window;

    PathTracer pathtracer;
    pathtracer.setScene(scene);
    pathtracer.render();

    window.display();

    while (window.pollEvent()) {
        window.clear();
        pathtracer.display();
        window.display();
    }

    return 0;

}
