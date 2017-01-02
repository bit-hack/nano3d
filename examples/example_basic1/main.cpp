#define _SDL_main_h
#include <SDL.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>

namespace {

template <typename type_t, size_t size>
static constexpr size_t array_length(const type_t (&array)[size])
{
    return size;
}

// vertex position
vec3f_t p[] = {
    {  0.f, -1.f, -.5f },
    {  1.f,  1.f, -.5f },
    { -1.f,  1.f, -.5f },
};

// vertex colour
vec4f_t c[] = {
    { 1.f, 0.f, 0.f, 1.f },
    { 0.f, 1.f, 0.f, 1.f },
    { 0.f, 0.f, 1.f, 1.f },
};

// index buffer
uint32_t ix[] = {
    0, 1, 2,
};

} // namespace {}

struct app_t {

    SDL_Surface* screen_;
    nano3d_t n3d_;

    n3d_rasterizer_t* rast_;
    n3d_vertex_buffer_t * buffer_;

    bool init()
    {
        // start up SDL
        if (SDL_Init(SDL_INIT_VIDEO))
            return false;
        screen_ = SDL_SetVideoMode(512, 512, 32, 0);
        if (!screen_)
            return false;

        // start nano3d instance
        n3d_framebuffer_t framebuffer = {
            512,
            512,
            (uint32_t*)screen_->pixels
        };
        n3d_.start(&framebuffer, 0, 0);

        // bind the vertex buffer
        buffer_ = new n3d_vertex_buffer_t{
            array_length(p),
            p,
            nullptr,
            c
        };
        n3d_.bind(buffer_);

        // create a rasterizer and bind it
        rast_ = n3d_rasterizer_new(n3d_raster_rgb);
        n3d_.bind(rast_);

        // bind a projection matrix
        mat4f_t proj;
        n3d_identity(proj);
        n3d_.bind(&proj, n3d_projection);
        n3d_.bind(&proj, n3d_model_view);

        return true;
    }

    bool stop()
    {
        n3d_.stop();
        n3d_rasterizer_delete(rast_);
        SDL_Quit();
        return true;
    }

    bool tick()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                return false;
        }
        return true;
    }

    bool main()
    {
        while (tick()) {

            n3d_.clear(0x101010, -100.f);

            SDL_FillRect(screen_, nullptr, 0x101010);

            // draw 6 elements from the index buffer
            n3d_.draw(array_length(ix), ix);

            // copy nano3d state to framebuffer
            n3d_.present();

            SDL_Flip(screen_);
        }

        return true;
    }
};

int main(int argc, char** args)
{
    app_t app;
    if (!app.init())
        return -1;
    app.main();
    app.stop();
    return 0;
}
