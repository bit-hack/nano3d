#define _SDL_main_h
#include <SDL.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>
#include <math.h>

namespace {

    vec3f_t p[] = {

        { 1.f, 1.f,-3.f },
        { 1.f,-1.f,-3.f },
        {-1.f, 1.f,-3.f },
        {-1.f,-1.f,-3.f },
    };

    vec4f_t c[] = {

        { 1.f, 0.f, 0.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f },
        { 1.f, 1.f, 0.f, 1.f },
    };

    uint32_t ix[] = {

        0, 1, 2,
        2, 1, 3,
    };

} // namespace {}

struct app_t {

    SDL_Surface * screen_;
    nano3d_t n3d_;
    n3d_rasterizer_t * rast_;
    float delta;

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
        n3d_.start(&framebuffer, 0, 1);

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

        // bind a model view matrix
//        mat4f_t mvm;
//        n3d_identity(mvm);
//        n3d_.bind(&mvm, n3d_model_view);

        // bind a projection matrix
        mat4f_t proj;
        n3d_frustum(proj, -1.f, 1.f, -1.f, 1.f, 1.f, 1.f / 100.f);
        n3d_.bind(&proj, n3d_projection);

        delta = 0.f;

        return true;
    }

    bool stop() {

        n3d_.stop();
        n3d_rasterizer_delete(rast_);
        SDL_Quit();
        return true;
    }

    bool tick() {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                return false;
        }
        return true;
    }

    const float pi2 = n3d_pi * 2.f;
    
    bool main() {

        while (tick()) {

            float st = sinf(delta);
            float ct =-cosf(delta);

            float d =-1.5f;

            p[0].x = st;
            p[0].z = d + ct;
            p[1].x = st;
            p[1].z = d + ct;
            p[2].x =-st;
            p[2].z = d - ct;
            p[3].x =-st;
            p[3].z = d - ct;
            
            n3d_.clear(0x101010, -100.f);

            // draw 6 elements from an index buffer
            n3d_.draw(6, ix);

            n3d_.present();
            SDL_Flip(screen_);

            delta = (delta >= pi2*2) ? 0.f : delta+0.01f;
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
