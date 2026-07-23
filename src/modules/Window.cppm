module;

#include <glfw/glfw3.h>

export module Window;

import std.compat;
using string = std::string;

export class Window{
  private:
    GLFWwindow* m_window;
    size_t m_width , m_height;
  public:
    GLFWwindow* getWindow() { return m_window; }
    Window(string title , size_t width = 800 , size_t height = 800)
    : m_window(nullptr) , m_width(width) , m_height(height)
    {}
    void init(bool fscreen = false){
    }
};