#define _SDL_main_h
#include <SDL.h>
#include <SDL_opengl.h>

#include <source/n3d_math.h>

struct rand_t {

    rand_t() = delete;

    rand_t(uint64_t seed)
        : x(0xdeadbeef + seed)
    {
    }

    float randf()
    {
        return 1.f - float(randi() & 0xffff) / float(0x7fff);
    }

    uint64_t randi()
    {
        x ^= x >> 12; // a
        x ^= x << 25; // b
        x ^= x >> 27; // c
        return x * 0x2545F4914F6CDD1D;
    }

protected:
    uint64_t x;
};

struct app_t {

    SDL_Surface* screen_;

    bool init()
    {
        // start up SDL
        if (SDL_Init(SDL_INIT_VIDEO))
            return false;
        screen_ = SDL_SetVideoMode(512, 512, 32, SDL_OPENGL);
        return screen_ != nullptr;
    }

    bool stop()
    {
        SDL_Quit();
        return true;
    }

    bool run_test(uint32_t seed)
    {
        rand_t rng{ seed };
        mat4f_t n3dmat;
        // start with identity
        glLoadIdentity();
        n3d_identity(n3dmat);
        // random number of itterations
        const uint32_t itters = 1 + (rng.randi() % 4);
        for (uint32_t i = 0; i < itters; ++i) {
            // generate some random numbers
            std::array<float, 16> nums;
            for (float& v : nums) {
                v = rng.randf() * 4.f;
            }
            // apply random matrix operations
            switch (rng.randi() % 3) {
            case 0:
                glScalef(nums[0], nums[1], nums[2]);
                n3d_scalef(n3dmat, nums[0], nums[1], nums[2]);
                break;
            case 1:
                glTranslatef(nums[0], nums[1], nums[2]);
                n3d_translatef(n3dmat, nums[0], nums[1], nums[2]);
                break;
            case 2:
                glRotatef(nums[0], nums[1], nums[2], nums[3]);
                n3d_rotatef(n3dmat, nums[0], nums[1], nums[2], nums[3]);
                break;
            }
        }
        // get the opengl matrix
        std::array<float, 16> oglmat;
        glGetFloatv(GL_MODELVIEW_MATRIX, oglmat.data());
        // verify matrix results
        for (uint32_t j = 0; j < 16; ++j) {
            const float diff = oglmat[j] - n3dmat(j);
            if (diff > 0.1f) {
                return false;
            }
        }
        return true;
    }

    bool main()
    {
        bool error = false;
        for (uint32_t i = 0; i < 100000; ++i) {
            if (!run_test(i)) {
                printf("Failed test: %d\n", i);
                error = true;
            }
        }
        return error;
    }
};

int main(int argc, char** args)
{
    app_t app;
    if (!app.init())
        return -1;
    const bool res = app.main();
    app.stop();
    return res ? 0 : 1;
}
