#include "n3d_decl.h"
#include "n3d_forward.h"

// transform vertex
void n3d_transform(
    n3d_vertex_t * v, 
    const uint32_t num_verts, 
    const mat4f_t & mat);

// clip triangle to near plane
void n3d_clip(
    n3d_vertex_t vert[4], 
    uint32_t & num_verts);

// apply perspective division
void n3d_w_divide(
    n3d_vertex_t vert[4],
    const uint32_t num_verts);

// normalised device coordinated to device coordinates
void n3d_ndc_to_dc(
    n3d_vertex_t vert[4], 
    const uint32_t num_verts, 
    const vec2f_t sf);
