#include <SDL2/SDL.h>
#include <dlfcn.h>
#include <fstream>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	SDL_GLContext original_context = SDL_GL_GetCurrentContext();
	SDL_GLContext user_context = NULL;

    user_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync
                               
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, user_context);

    bool done = false;
    while (!done) {
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
          ImGui_ImplSDL2_ProcessEvent(&event);
          if (event.type == SDL_QUIT)
              done = true;
      }
      
      ImGui_ImplOpenGL2_NewFrame();
      ImGui_ImplSDL2_NewFrame(window);
      ImGui::NewFrame();

      ImGui::ShowDemoWindow();
      // ImGui::Text("Hello, world!");
      ImGui::Render();
      SDL_GL_SwapWindow(window);
    }
}
