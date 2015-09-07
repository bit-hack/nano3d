#include "n3d_bin.h"
#include "n3d_frame.h"

namespace {

    void bin_clear(n3d_bin_t &bin, uint32_t argb, float depth) {

        const uint32_t height = bin.state_.height_;
        const uint32_t width = bin.state_.width_;
        const uint32_t pitch = bin.state_.pitch_;

        uint32_t *c = bin.state_.target_[n3d_target_pixel].uint32_;
        float *z = bin.state_.target_[n3d_target_depth].float_;

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
    n3d_bin_t * bin) {

    n3d_assert(bin->lock_.atom_ != 0);

    // bin_man will aquire the lock when its asked to receive work
    // to do.  this lock scope will release the lock after we have
    // finished processing the bin.
    n3d_scope_spinlock_t(bin->lock_, false);

    n3d_assert(bin);

    n3d_command_t cmd;

    n3d_rasterizer_t::state_t & state = bin->state_;

    while (true) {

        // try to pop a command from the queue
        while (! bin->pipe_.pop(cmd)) {
            return;
        }

        switch (cmd.command_) {
        case (n3d_command_t::cmd_triangle):

            if (bin->rasterizer_) {
                
                n3d_assert (bin->rasterizer_->raster_proc_);
                bin->rasterizer_->raster_proc_(
                    state,
                    cmd.triangle_,
                    bin->rasterizer_->user_);
            }
            break;

        case (n3d_command_t::cmd_present):
            if (bin->counter_)
                n3d_atomic_dec(*bin->counter_);
            ++bin->frame_;
            return;

        case (n3d_command_t::cmd_clear) :
            bin_clear (*bin, cmd.clear_.color_, cmd.clear_.depth_);
            break;

        case (n3d_command_t::cmd_texture):
            bin->state_.texure_ = cmd.texture_;
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
