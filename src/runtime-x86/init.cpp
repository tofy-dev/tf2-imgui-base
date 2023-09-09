#include <SDL2/SDL.h>
#include <dlfcn.h>
#include <fstream>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl2.h"

// Pointer to 'SDL_GL_SwapWindow' in the jump table.
uintptr_t* swapwindow_ptr;

// Address of the original 'SDL_GL_SwapWindow'.
uintptr_t swapwindow_original;

// Create our replacement function.
void hkSwapWindow(SDL_Window* window) {
	// Get the original 'SDL_GL_SwapWindow' symbol from 'libSDL2-2.0.so.0'.
	static void (*oSDL_GL_SwapWindow) (SDL_Window*) = reinterpret_cast<void(*)(SDL_Window*)>(swapwindow_original);
    static std::ofstream ofs{"/tmp/imgui.txt"};
	
	// Store OpenGL contexts.
	static SDL_GLContext original_context = SDL_GL_GetCurrentContext();
	static SDL_GLContext user_context = NULL;
	
	// Perform first-time initialization.
	if (!user_context) {
		// Create a new context for our rendering.
		user_context = SDL_GL_CreateContext(window);
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.WantCaptureMouse = true;

        ImGui_ImplSDL2_InitForOpenGL(window, user_context);
        ImGui_ImplOpenGL2_Init();
	}

	// Switch to our context.
	SDL_GL_MakeCurrent(window, user_context);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

	// Perform UI rendering.
    ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

	ImGui::ShowDemoWindow();

    bool my_tool_active = true;
    ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool_active = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    float my_color = 0;
    // Edit a color stored as 4 floats
    ImGui::ColorEdit4("Color", &my_color);

    // Generate samples and plot them
    float samples[100];
    for (int n = 0; n < 100; n++)
        samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
    ImGui::PlotLines("Samples", samples, 100);

    // Display contents in a scrolling region
    ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
        ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();
    ImGui::End();

	ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());


	// Swap back to the game context.
	SDL_GL_MakeCurrent(window, original_context);

	// Call the original function.
	oSDL_GL_SwapWindow(window);
}

void __attribute__((constructor)) attach() {
	// Get the symbol address of 'SDL_GL_SwapWindow'.
	Dl_info sdl_libinfo = {};
	dladdr(dlsym(RTLD_NEXT, "SDL_GL_SwapWindow"), &sdl_libinfo);
    std::ofstream ofs{"/tmp/imgui.txt"};
    ofs << &sdl_libinfo << std::endl;

    ofs << "fbase" << sdl_libinfo.dli_fbase << std::endl;
    ofs << "fname" << sdl_libinfo.dli_fname << std::endl;
    ofs << "saddr" << sdl_libinfo.dli_saddr << std::endl;
    ofs << "sname" << sdl_libinfo.dli_sname << std::endl;

	// Get the address of 'SDL_GL_SwapWindow' in the jump table.
	swapwindow_ptr = reinterpret_cast<uintptr_t*>(uintptr_t(sdl_libinfo.dli_fbase) + 0xFD648);
	
	// Backup the original address.
	swapwindow_original = *swapwindow_ptr;

	// Write the address to our replacement function.
	*swapwindow_ptr = reinterpret_cast<uintptr_t>(&hkSwapWindow);
}

void __attribute__((destructor)) detach() {
	// Restore the original address.
	*swapwindow_ptr = swapwindow_original;
}
