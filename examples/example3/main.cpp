#define _SDL_main_h
#include <SDL.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>
#include <math.h>

namespace {

    const float vn = -1.f;
    const float vp =  1.f;

    vec3f_t p[] = {

        { vp, vp, vp },
        { vn, vp, vp },
        { vp, vn, vp },
        { vn, vn, vp },
        { vp, vp, vn },
        { vn, vp, vn },
        { vp, vn, vn },
        { vn, vn, vn },
    };

    vec3f_t c[] = {

        { 1.f, 0.f, 0.f },
        { 0.f, 1.f, 0.f },
        { 0.f, 0.f, 1.f },
        { 1.f, 1.f, 0.f },
        { 1.f, 0.f, 0.f },
        { 0.f, 1.f, 0.f },
        { 0.f, 0.f, 1.f },
        { 1.f, 1.f, 0.f },
    };

    uint32_t ix[] = {

#define face(v0,v1,v2,v3) v0, v1, v2, v1, v3, v2

/*
            5     4
         1      0

            7     6
         3      2
**/
        face(0, 1, 2, 3),
        face(4, 0, 6, 2),
        face(4, 5, 6, 7),
        face(5, 1, 7, 3),
        face(4, 5, 0, 1),
        face(2, 3, 6, 7),
    };

    float delta = 0.f;

} // namespace {}

struct app_t {

    SDL_Surface * screen_;
    nano3d_t n3d_;
    n3d_rasterizer_t * rast_;

    bool init() {

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
        n3d_.start(&framebuffer, 1);

        // bind the vertex buffer
        n3d_vertex_buffer_t vb = {
            4,
            p,
            nullptr,
            c
        };
        n3d_.bind(&vb);

        // bind a rasterizer
        rast_ = n3d_rasterizer_new(n3d_raster_reference);
        n3d_.bind(rast_);

        // bind model view matrix
        mat4f_t mvm;
        n3d_identity(mvm);
        mvm.e[11] = -3.f;
        mvm.e[14] = -3.f;
        n3d_.bind(&mvm, n3d_model_view);

        // bind a projection matrix
        mat4f_t proj;
        n3d_frustum(proj, -1.f, 1.f, -1.f, 1.f, 1.f, 100.f);
        n3d_.bind(&proj, n3d_projection);

        return true;
    }

    bool stop() {

        n3d_.stop();
        n3d_rasterizer_delete(rast_);
        return n3d_.stop() == n3d_sucess;
    }

    bool tick() {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                return false;
        }
        return true;
    }

    bool main() {

        while (tick()) {

            SDL_FillRect(screen_, nullptr, 0x101010);

            n3d_.draw(36, ix);

            n3d_.present();

            SDL_Flip(screen_);

            delta += 0.003f;
            if (delta >= (n3d_pi * 2.f))
                delta -= (n3d_pi * 2.f);
        }

        return true;
    }
};

int main(int argc, char ** args) {

    app_t app;

    if (!app.init())
        return -1;

    app.main();
    app.stop();

    return 0;
}
