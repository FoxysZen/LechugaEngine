#include <Window.h>

Window::Window(std::string _title, int _width, int _height)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
        SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    title = _title;
    width = _width;
    height = _height;
    
    window = SDL_CreateWindow(_title.c_str(), _width, _height, flags);
    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", 
            SDL_GetError());
        Logger::error("Window: Could not create window.");
        return;
    }

    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, 
            "Could not create OpenGL context: %s\n", SDL_GetError());
        Logger::error("Window: Could not create OpenGL context.");
        isOpened = false;
        return;
    }

    SDL_GL_MakeCurrent(window, glContext);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize GLAD\n");
        Logger::error("Window: Failed to initialize GLAD.");
        isOpened = false;
        return;
    }

    isOpened = true;
}

Window::~Window()
{
    if (glContext != nullptr)
    {
        SDL_GL_DestroyContext(glContext);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::closeWindow()
{
    isOpened = false;
}

void Window::swapBuffers()
{
    SDL_GL_SwapWindow(window);
}

SDL_Window *Window::getWindow()
{
    return window;
}

int Window::getHeight()
{
    return height;
}

int Window::getWidth()
{
    return width;
}

bool Window::isOpen()
{
    return isOpened;
}

void Window::setSize(int _width, int _height)
{
    if (!SDL_SetWindowSize(window, _width, _height))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not resize window: %s\n", 
            SDL_GetError());
        Logger::error("Window: Could not resize window.");
    }
    else
    {
        width = _width;
        height = _height;
    }
}

void Window::setTitle(std::string _title)
{
    if (!SDL_SetWindowTitle(window, _title.c_str()))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, 
            "Could not change window title: %s\n", SDL_GetError());
        Logger::error("Window: Could not change window title.");
    }
    else
    {
        title = _title;
    }
}

