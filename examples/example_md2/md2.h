#pragma once

#include <stdint.h>

struct md2_hdr_t
{
    uint8_t  ident_[4];          // magic number. must be equal to "IDP2"
    uint32_t version_;           // md2 version. must be equal to 8

    uint32_t skinwidth_;         // width of the texture
    uint32_t skinheight_;        // height of the texture
    uint32_t framesize_;         // size of one frame in bytes

    uint32_t num_skins_;         // number of textures
    uint32_t num_xyz_;           // number of vertices
    uint32_t num_uv_;            // number of texture coordinates
    uint32_t num_tris_;          // number of triangles
    uint32_t num_glcmds_;        // number of opengl commands
    uint32_t num_frames_;        // total number of frames

    uint32_t ofs_skins_;         // offset to skin names (64 bytes each)
    uint32_t ofs_st_;            // offset to s-t texture coordinates
    uint32_t ofs_tris_;          // offset to triangles
    uint32_t ofs_frames_;        // offset to frame data
    uint32_t ofs_glcmds_;        // offset to opengl commands
    uint32_t ofs_end_;           // offset to end of file

};

struct md2_vertex_t {

    uint8_t vertex_[3];
    uint8_t light_;
};

struct md2_frame_t {

    float scale_[3];
    float translate_[3];
    char name_[16];
    // followed by 'num_xyz' vertices
};

struct md2_triangle_t {

    uint16_t idx_vert_[3];
    uint16_t idx_uv_[3];
};

struct md2_tex_coord_t {

    uint16_t u_, v_;
};

struct md2_gl_command_t {

    float u_, v_;
    int index_;
};
