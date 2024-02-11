
#ifndef RENDERING_WORLDRENDER
#define RENDERING_WORLDRENDER

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <thread>

namespace render
{

class WorldRenderer
{
public:
    explicit WorldRenderer();

    /// Init the renderer
    /// @return false if the renderer failed to start
    bool init();

    void run();
private:
    void renderFrame();
    void renderGui();

    double m_targetFps = 60;

    /// Window object
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;
};

} // render

#endif  // RENDERING_WORLDRENDER
