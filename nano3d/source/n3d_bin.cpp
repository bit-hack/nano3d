#include "n3d_bin.h"
#include "n3d_frame.h"

namespace {

    void bin_clear(n3d_bin_t &bin, uint32_t argb, float depth) {

        const uint32_t height = bin.height_;
        const uint32_t width = bin.width_;
        const uint32_t pitch = bin.pitch_;

        uint32_t *c = bin.color_;
        float *z = bin.depth_;

        for (uint32_t y = 0; y < height; ++y) {

            for (uint32_t x = 0; x < width; ++x) {
                c[x] = argb;
                z[x] = depth;
            }

            z += pitch;
            c += pitch;
        }
    }
};

void n3d_bin_process (
    n3d_bin_t * bin,
    uint32_t target_frame) {

    n3d_assert(bin);

    bool active = true;
    n3d_command_t cmd;

    n3d_rasterizer_t::state_t state;
    state.color_  = bin->color_;
    state.width_  = bin->width_;
    state.height_ = bin->height_;
    state.depth_  = bin->depth_;
    state.pitch_  = bin->pitch_;
    state.texure_ = bin->texture_;
    state.offset_ = bin->offset_;

    while (active && (bin->frame_ <= target_frame)) {

        // try to pop a command from the queue
        while (! bin->pipe_.pop(cmd)) {
            n3d_yield ();
        }

        switch (cmd.command_) {
        case (n3d_command_t::cmd_triangle):

            if (bin->rasterizer_) {
                n3d_assert (bin->rasterizer_->run_);
                bin->rasterizer_->run_(state,
                                       cmd.triangle_,
                                       bin->rasterizer_->user_);
            }
            break;

        case (n3d_command_t::cmd_present):
            ++bin->frame_;
            active = false;
            if (bin->bins_pending_)
                n3d_atomic_dec(*bin->bins_pending_);
            break;

        case (n3d_command_t::cmd_clear) :
            bin_clear (*bin, cmd.clear_.color_, cmd.clear_.depth_);
            break;

        case (n3d_command_t::cmd_texture):
            bin->texture_ = cmd.texture_;
            state.texure_ = cmd.texture_;
            break;

        case (n3d_command_t::cmd_rasterizer):
            bin->rasterizer_ = cmd.rasterizer_;
            break;

        default:
            n3d_assert(!"unknown command");
        }
    }
}
