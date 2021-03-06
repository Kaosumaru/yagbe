#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "renderer/sdl2_renderer.hpp"
#include "serializer/serializer.hpp"
#include "vm/context.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
using namespace yagbe;

context ctx;
sdl2_renderer renderer(gpu::screen_size());

#ifdef __EMSCRIPTEN__
emscripten_serializer saves(ctx);
#else
filesave_serializer saves(ctx);
#endif

bool loaded_rom = false;

void one_iter()
{
    if (!loaded_rom)
        return;
    renderer.next_frame();
    while (renderer.step() && !renderer.frame_drawn())
    {
        for (int i = 0; i < 10; i++)
            ctx.cpu_step();
    }
}

int main(int argc, char* argv[])
{

    std::map<SDL_Keycode, key_handler::key> keys = {
        { SDLK_LEFT, key_handler::key::Left },
        { SDLK_RIGHT, key_handler::key::Right },
        { SDLK_UP, key_handler::key::Up },
        { SDLK_DOWN, key_handler::key::Down },

        { SDLK_a, key_handler::key::Select },
        { SDLK_s, key_handler::key::Start },
        { SDLK_z, key_handler::key::A },
        { SDLK_x, key_handler::key::B },
    };

    std::string path = YAGBE_ROMS;

    //path += "../test_roms/individual/01-special.gb"; //PASSED
    //path += "../test_roms/individual/02-interrupts.gb"; //PASSED
    //path += "../test_roms/individual/03-op sp,hl.gb"; //PASSED

    //path += "../test_roms/individual/04-op r,imm.gb"; //PASSED
    //path += "../test_roms/individual/05-op rp.gb"; //PASSED
    //path += "../test_roms/individual/06-ld r,r.gb"; //PASSED

    //path += "../test_roms/individual/07-jr,jp,call,ret,rst.gb"; //PASSED

    //path += "../test_roms/individual/08-misc instrs.gb"; //PASSED

    //path += "../test_roms/individual/09-op r,r.gb"; //PASSED

    //path += "../test_roms/individual/10-bit ops.gb"; //PASSED

    //path += "../test_roms/individual/11-op a,(hl).gb"; //PASSED
    //path += "../test_roms/instr_timing.gb"; //FAILED, timer doesn't work properly

    //path += "hangman.gb";
    //path += "sml.gb";

    path += "adjtris.gb";

    renderer.initialize(ctx);

    renderer.onKeyChanged = [&](SDL_Keycode c, bool v, const sdl2_renderer::key_info& info) {
        if (c >= SDLK_0 && c <= SDLK_9 && v)
        {
            int index = c - SDLK_0;
            if (info.shift)
                saves.save_state(index);
            else
                saves.load_state(index);
        }

        auto it = keys.find(c);
        if (it != keys.end())
            ctx.key_handler.set_key(it->second, v);
    };

#ifdef __EMSCRIPTEN__
    int raw_url = EM_ASM_INT({
        var getQueryString = function(field, url)
        {
            var href = url ? url : window.location.href;
            var reg = new RegExp('[?&]' + field + '=([^&#]*)', 'i');
            var string = reg.exec(href);
            return string ? string[1] : null;
        };

        var url = getQueryString("url") || "";
        return allocate(intArrayFromString(url), 'i8', ALLOC_NORMAL);
    });

    std::cout << "Starting emulator..." << std::endl;
    std::string url = (const char*)raw_url;
    free((void*)raw_url);
    if (!url.empty())
    {
        std::cout << "Getting rom from: " << url << std::endl;

        auto onLoad = [](void*, void* b, int s) {
            std::cout << "Done." << std::endl;
            auto rom_info = ctx.load_rom((uint8_t*)b, s);
            if (rom_info)
            {
                loaded_rom = true;
                std::cout << "Loaded OK, name: " << rom_info->game_title() << std::endl;
                return;
            }
            std::cout << "Loading failed" << std::endl;
        };

        auto onError = [](void*) {
            std::cout << "Error." << std::endl;
        };

        emscripten_async_wget_data(url.c_str(), nullptr, onLoad, onError);
    }
    else
    {
        std::cout << "Loading built-in rom" << std::endl;
        auto rom_info = ctx.load_rom(path);
        if (!rom_info)
            return -1;
        std::cout << "Loaded built-in OK, name: " << rom_info->game_title() << std::endl;
        loaded_rom = ctx.load_rom(path);
    }

    emscripten_set_main_loop(one_iter, 0, 1);
#else
    auto rom_info = ctx.load_rom(path);
    if (!rom_info)
        return -1;
    loaded_rom = true;

    while (renderer.running())
        one_iter();
#endif

    return 0;
}

/*
Left to do:
- 16h sprites
- RAM & timers in MBC1-3

-sound
*/