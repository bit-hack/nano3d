#include <array>

#define _SDL_main_h
#include <SDL.h>
#include <math.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>

namespace {
static const float vn = -1.f, vp = 1.f;

template <typename type_t, size_t size>
static constexpr size_t array_length(const type_t (&array)[size])
{
    return size;
}

SDL_Surface * image = nullptr;

SDL_Surface * load_bmp(const char * bmp) {
    SDL_Surface * temp = SDL_LoadBMP(bmp);
    if (!temp) {
        return nullptr;
    }
    SDL_PixelFormat fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.BitsPerPixel = 32;
    fmt.BytesPerPixel = 4;
    fmt.Amask = 0xff000000;
    fmt.Rmask = 0xff0000;
    fmt.Gmask = 0xff00;
    fmt.Bmask = 0xff;
    SDL_Surface * out = SDL_ConvertSurface(temp, &fmt, 0);
    SDL_FreeSurface(temp);
    return out;
}

vec3f_t p[] = {
    { vn, vp, vp },
    { vp, vp, vp },
    { vn, vn, vp },
    { vp, vn, vp },

    { vn, vp, vn },
    { vn, vn, vn },
    { vp, vp, vn },
    { vp, vn, vn },

    { vn, vn, vp },
    { vp, vn, vp },
    { vn, vn, vn },
    { vp, vn, vn },

    { vn, vp, vp },
    { vn, vp, vn },
    { vp, vp, vp },
    { vp, vp, vn },
    
    { vn, vn, vp },
    { vn, vn, vn },
    { vn, vp, vp },
    { vn, vp, vn },
    
    { vp, vn, vp },
    { vp, vp, vp },
    { vp, vn, vn },
    { vp, vp, vn },
};

vec2f_t t[] = {
    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },

    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },

    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },

    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },

    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },

    { 0.f, 0.f },
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },
};

uint32_t ix[] = {
#define face(B, v0, v1, v2, v3) (B+v0), (B+v2), (B+v1), (B+v0), (B+v3), (B+v2)
    face( 0, 0, 1, 3, 2),
    face( 4, 0, 1, 3, 2),
    face( 8, 0, 1, 3, 2),
    face(12, 0, 1, 3, 2),
    face(16, 0, 1, 3, 2),
    face(20, 0, 1, 3, 2),
};
} // namespace {}

struct app_t {

    SDL_Surface* screen_;
    nano3d_t n3d_;
    
    n3d_rasterizer_t* raster_;
    n3d_texture_t* texture_;
    n3d_vertex_buffer_t* buffer_;

    bool init()
    {
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

        // bind the texture
        image = load_bmp("C:\\repos\\nano3d\\texture.bmp");
        if (image) {
            texture_ = new n3d_texture_t {
                uint32_t(image->w),
                uint32_t(image->h),
                (uint32_t*)image->pixels
            };
            n3d_.bind(texture_);
        }

        // bind the vertex buffer
        buffer_ = new n3d_vertex_buffer_t {
            array_length(p),
            p,
            t,
            nullptr
        };
        n3d_.bind(buffer_);

        // bind a rasterizer
        raster_ = n3d_rasterizer_new(n3d_raster_texture);
        n3d_.bind(raster_);

        // bind a projection matrix
        mat4f_t proj;
        n3d_frustum(proj, -1.f, 1.f, -1.f, 1.f, 1.f, 15.f);
        n3d_.bind(&proj, n3d_projection);

        return true;
    }

    bool stop()
    {
        n3d_.stop();
        n3d_rasterizer_delete(raster_);
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

    void inc(float& v, float i)
    {
        const float pi2 = n3d_pi * 2.f;
        v += i;
        if (v >= pi2)
            v -= pi2;
    }

    bool main()
    {
        float rx = 0.f, ry = 0.f, rz = 0.f;
        float jx = 0.f, jy = 0.f, jz = 0.f;
        float px = 0.f, py = 0.f, pz = 0.f;

        while (tick()) {

            n3d_.clear(0x101010, 1.f / 100.f);

            // bind a model view matrix
            mat4f_t mvm;
            n3d_rotate(mvm, rx, ry, rz);
            n3d_translate(mvm, vec3(sinf(px), cosf(py), sinf(pz) - 3.8f));
            n3d_.bind(&mvm, n3d_model_view);

            // draw the cube
            n3d_.draw(array_length(ix), ix);
            n3d_.present();

#if 1
            n3d_rotate(mvm, jx, jy, jz);
            n3d_translate(mvm, vec3(0.f, 0.f, -4.0f));
            n3d_.bind(&mvm, n3d_model_view);

            // draw the cube
            n3d_.draw(array_length(ix), ix);
            n3d_.present();
#endif

            SDL_Flip(screen_);

            inc(rx, 0.07357f);
            inc(ry, 0.01123f);
            inc(rz, 0.04021f);

            inc(jx, 0.03357f);
            inc(jy, 0.00523f);
            inc(jz, 0.02121f);

            inc(px, 0.00317f);
            inc(py, 0.00423f);
            inc(pz, 0.00721f);

//            SDL_Delay(50);
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
