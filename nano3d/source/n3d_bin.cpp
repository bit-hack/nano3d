#include "n3d_bin.h"
#include "n3d_frame.h"

namespace {

    void bin_clear(n3d_bin_t & bin, uint32_t argb, float depth) {

        const uint32_t height = bin.height_;
        const uint32_t width  = bin.width_;
        const uint32_t pitch  = bin.pitch_;

        uint32_t * c = bin.color_;
        float    * z = bin.depth_;

        for (uint32_t y = 0; y < height; ++y) {

            for (uint32_t x = 0; x < width; ++x) {
                c[x] = argb;
                z[x] = depth;
            }

            z += pitch;
            c += pitch;
        }
    }

    void bin_triangle(n3d_bin_t & bin, n3d_rasterizer_t::triangle_t & t) {
        //(todo) shift interpolants to bin offset
    }

};

void n3d_bin_process (
    n3d_bin_t * bin) {

    bool active = true;
    n3d_command_t cmd;

    while (bin->pipe_.pop(cmd) || active) {

        switch (cmd.command_) {
        case (n3d_command_t::cmd_triangle):
            bin_triangle (*bin, *cmd.triangle_);
            break;

        case (n3d_command_t::cmd_present):
            active = false;
            break;

        case (n3d_command_t::cmd_clear) :
            bin_clear (*bin, cmd.clear_.color_, cmd.clear_.depth_);
            break;

        case (n3d_command_t::cmd_texture):
            bin->texture_ = cmd.texture_;
            break;

        case (n3d_command_t::cmd_rasterizer):
            bin->rasterizer_ = cmd.rasterizer_;
            break;

        default:
            assert (!"unknown command");
        }
    }
}
