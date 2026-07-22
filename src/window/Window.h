#pragma once
#include <glad/glad.h>
#include <Logger.h>
#include <SDL3/SDL.h>
#include <string>

class Window
{
    public:
        Window(std::string _title, int _width, int _height);
        ~Window();

        void closeWindow();
        void swapBuffers();

        SDL_Window *getWindow();
        SDL_GLContext getGLContext();
        int getHeight();
        int getWidth();
        bool isOpen();

        void setSize(int _width, int _height);
        void setTitle(std::string _title);

    private:
        SDL_Window *window = nullptr;
        SDL_GLContext glContext = nullptr;

        std::string title;
        int width, height;
        bool isOpened = false;
};