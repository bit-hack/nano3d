#define _SDL_main_h
#include <SDL.h>
#include <SDL_opengl.h>

#include <math.h>
#include <nano3d.h>
#include <nano3d_ex.h>
#include <source/n3d_math.h>

// from bunny.cpp
extern const float obj_vertex[1506];
extern const uint32_t obj_index[3000];
extern const uint32_t obj_num_vertex;
extern const uint32_t obj_num_index;

struct camera_t {
    float xrot, yrot;
    vec3f_t pos;
};

struct app_t {

    static const uint32_t c_width = 512;
    static const uint32_t c_height = 512;
    static const uint32_t c_threads = 3;

    SDL_Surface* screen_;
    nano3d_t n3d_;
    n3d_rasterizer_t* rast_;
    camera_t camera_;

    bool grab_;

    bool init()
    {
        const float c_aspect = float(c_width) / float(c_height);

        // create SDL window
        if (SDL_Init(SDL_INIT_VIDEO))
            return false;
        SDL_WM_SetCaption("nano3D demo", nullptr);
        screen_ = SDL_SetVideoMode(c_width, c_height, 32, SDL_OPENGL);
        if (!screen_)
            return false;

        grab_ = false;
//      SDL_WarpMouse(256, 256);
//      SDL_WM_GrabInput(grab_ ? SDL_GRAB_ON : SDL_GRAB_OFF);

        // start nano3d instance
//      const n3d_target_t framebuffer = {
//          c_width,
//          c_height,
//          (uint32_t*)screen_->pixels
//      };
//      n3d_.start(&framebuffer, 0, c_threads);

        // bind the vertex buffer
//      const n3d_vertex_buffer_t vb = {
//          obj_num_vertex / 3,
//          (vec3f_t*)obj_vertex,
//          nullptr,
//          nullptr,
//      };
//      n3d_.bind(&vb);

        // bind a rasterizer
//      rast_ = n3d_rasterizer_new(n3d_raster_depth);
//      n3d_.bind(rast_);

        // bind a projection matrix
//      mat4f_t proj;
//      n3d_frustum(proj, -c_aspect, c_aspect, -1.f, 1.f, 3.f, -128.f);
//      n3d_.bind(&proj, n3d_projection);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-c_aspect, c_aspect, -1.f, 1.f, 2.f, 256.f);

        return true;
    }

    bool stop()
    {
        // shutdown demo
//      n3d_.stop();
//      n3d_rasterizer_delete(rast_);
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
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                grab_ = !grab_;
                SDL_WM_GrabInput(grab_ ? SDL_GRAB_ON : SDL_GRAB_OFF);
                if (grab_) {
                    SDL_WarpMouse(256, 256);
                }
            }
        }
        return true;
    }

    void update_camera()
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(camera_.xrot, 1.f, 0, 0);
        glRotatef(camera_.yrot, 0, 1.f, 0);
        glTranslatef(-camera_.pos.x, -camera_.pos.y, -camera_.pos.z);

        std::array<float, 16> mat;
        glGetFloatv(GL_MODELVIEW_MATRIX, mat.data());

        // generate and bind the camera matrix
//      mat4f_t mat;
//      n3d_identity(mat);
//      n3d_rotatef(mat, camera_.xrot, 1.f, 0, 0);
//      n3d_rotatef(mat, camera_.yrot, 0, 1.f, 0);
//      n3d_translatef(mat, -camera_.pos.x, -camera_.pos.y, -camera_.pos.z);
//      n3d_transpose(mat); // XXX: <--- why transpose?
//      n3d_.bind(&mat, n3d_model_view);

        // keyboard input
        if (const uint8_t* keys = SDL_GetKeyState(nullptr)) {

            auto IX = [](uint32_t x, uint32_t y) -> uint32_t {
              return x + y * 4;
            };

            if (keys[SDLK_UP] || keys[SDLK_w]) {
                camera_.pos -= vec3(mat[IX(2, 0)], mat[IX(2, 1)], mat[IX(2, 2)]);
            }
            if (keys[SDLK_DOWN] || keys[SDLK_s]) {
                camera_.pos += vec3(mat[IX(2, 0)], mat[IX(2, 1)], mat[IX(2, 2)]);
            }
            if (keys[SDLK_LEFT] || keys[SDLK_a]) {
                camera_.pos -= vec3(mat[IX(0, 0)], mat[IX(0, 1)], mat[IX(0, 2)]);
            }
            if (keys[SDLK_RIGHT] || keys[SDLK_d]) {
                camera_.pos += vec3(mat[IX(0, 0)], mat[IX(0, 1)], mat[IX(0, 2)]);
            }
        }

        // mouse grab
        if (grab_) {
            int mx = 0, my = 0;
            SDL_GetMouseState(&mx, &my);
            SDL_WarpMouse(256, 256);
            (mx = 256 - mx), (my = 256 - my);
            camera_.xrot -= float(my) * 0.1f;
            camera_.yrot -= float(mx) * 0.1f;
        }
    }

    void frame()
    {
        glClearColor(0.2, 0.3, 0.4, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // clear the framebuffer
//      const float c_clear_depth = 1.f / 1024;
//      n3d_.clear(0x101010, c_clear_depth);

        // update our camera
        update_camera();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 1.f, 1.f);
        for (uint32_t i = 0; i < obj_num_index; ++i) {
            const float *a = obj_vertex + 3 * obj_index[i];
            glVertex3f(a[0], a[1], a[2]);
        }
        glEnd();

        // draw the model
//      n3d_.draw(obj_num_index, obj_index);
//      n3d_.present();

        glFlush();

        // flip the sdl surface
        SDL_GL_SwapBuffers();
//        SDL_Flip(screen_);
    }

    void set_fps_title(double diff)
    {
        std::array<char, 128> title;
        sprintf_s(title.data(), title.size(), "fps: %f", diff);
        SDL_WM_SetCaption(title.data(), nullptr);
    }

    bool main()
    {
        camera_.xrot = 0.f;
        camera_.yrot = 0.f;
        camera_.pos = vec3f_t{ 0.f, 0.f, 256.f };

        static const uint32_t c_interval = 32;
        uint32_t t0 = SDL_GetTicks();
        uint32_t t1 = t0;
        for (uint32_t i = 0; tick(); ++i) {
            if (i % c_interval == 0) {
                t0 = t1;
                t1 = SDL_GetTicks();
                set_fps_title((1000.0 * c_interval) / double(t1 - t0));
            }
            frame();
        }
        return true;
    }
};

int main(int argc, char** args)
{
    app_t app;
    if (!app.init()) {
        return -1;
    }
    app.main();
    app.stop();
    return 0;
}
