#define _SDL_main_h
#include <SDL.h>
#include <nano3d.h>
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

        0, 1, 2, /**/ 2, 1, 3,
        0, 1, 2, /**/ 2, 1, 3,
        0, 1, 2, /**/ 2, 1, 3,
        0, 1, 2, /**/ 2, 1, 3,
        0, 1, 2, /**/ 2, 1, 3,
        0, 1, 2, /**/ 2, 1, 3,
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
        rast_ = n3d_.rasterizer_new(n3d_raster_reference);
        n3d_.bind(rast_);

        // bind a projection matrix
        mat4f_t proj;
        n3d_frustum(proj, -1.f, 1.f, -1.f, 1.f, 1.f, 100.f);
        n3d_.bind(&proj, n3d_projection);

        return true;
    }

    bool stop() {

        n3d_.stop();
        n3d_.rasterizer_delete(rast_);
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

#if 1
            float st = sinf(delta);
            float ct = cosf(delta);

            p[0].x = st;
            p[0].z = ct - 2.f;
            p[1].x = st;
            p[1].z = ct - 2.f;
            p[2].x =-st;
            p[2].z =-ct - 2.f;
            p[3].x =-st;
            p[3].z =-ct - 2.f;
#endif

            SDL_FillRect(screen_, nullptr, 0x101010);

            // draw 6 elements from an index buffer
            n3d_.draw(6, ix);

            // copy nano3d state to frame buffer
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
