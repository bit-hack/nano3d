#define _SDL_main_h
#include <SDL.h>
#include <math.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>

extern const float obj_vertex[1506];
extern const uint32_t obj_index[3000];

extern const uint32_t obj_num_vertex;
extern const uint32_t obj_num_index;

struct app_t {

    SDL_Surface* screen_;
    nano3d_t n3d_;
    n3d_rasterizer_t* rast_;

    bool init()
    {
        const uint32_t c_threads = 3;
        const uint32_t c_width = 512;
        const uint32_t c_height = 512;
        const float c_aspect = float(c_width) / float(c_height);

        // create SDL window
        if (SDL_Init(SDL_INIT_VIDEO))
            return false;
        SDL_WM_SetCaption("nano3D demo", nullptr);
        screen_ = SDL_SetVideoMode(c_width, c_height, 32, 0);
        if (!screen_)
            return false;

        // start nano3d instance
        n3d_framebuffer_t framebuffer = {
            c_width,
            c_height,
            (uint32_t*)screen_->pixels
        };
        n3d_.start(&framebuffer, 0, c_threads);

        // bind the vertex buffer
        n3d_vertex_buffer_t vb = {
            obj_num_vertex / 3,
            (vec3f_t*)obj_vertex,
            nullptr,
            nullptr,
        };
        n3d_.bind(&vb);

        // bind a rasterizer
        rast_ = n3d_rasterizer_new(n3d_raster_depth);
        n3d_.bind(rast_);

        // bind a projection matrix
        mat4f_t proj;
        n3d_frustum(proj, -c_aspect, c_aspect, -1.f, 1.f, 5.f, -128.f);
        n3d_.bind(&proj, n3d_projection);

        return true;
    }

    bool stop()
    {
        // shutdown demo
        n3d_.stop();
        n3d_rasterizer_delete(rast_);
        SDL_Quit();
        return true;
    }

    bool tick()
    {
        // SDL message pump
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                return false;
        }
        return true;
    }

    void inc(float& v, float i)
    {
        // increment and wrap to [0, PI*2]
        v = (v + i > n3d_pi2) ? v + i - n3d_pi2 : v + i;
    }

    bool main()
    {
        float r = 0.f, p = 0.f;

        while (tick()) {
            // update rotation and movement
            inc(r, 0.04021f);
            inc(p, 0.01021f);

            // clear the framebuffer
            const float c_clear_depth = 1.f / 1024;
            n3d_.clear(0x101010, c_clear_depth);

            // bind a model view matrix
            mat4f_t mvm;
            n3d_rotate(mvm, n3d_pi / 2, 0.f, r);
            n3d_translate(mvm, vec3(0.f, 40.f, sinf(p) * 64 - 256.f));
            n3d_.bind(&mvm, n3d_model_view);

            // draw the model
            n3d_.draw(obj_num_index, obj_index);
            n3d_.present();

            // flip the sdl surface
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
