#include "scene.h"

#include "../asset/mesh.h"
#include "../asset/texture.h"

#define MAX_STATICMESH_COUNT 512

typedef struct {
    nu_vec3_t eye;
    nu_vec3_t center;
    nu_vec3_t up;
    float fov;
    float near;
    float far;
} nusr_camera_t;

typedef struct {
    uint32_t mesh;
    uint32_t texture;
    nu_mat4_t transform;
    bool active;
} nusr_staticmesh_t;

typedef struct {
    nusr_camera_t camera;
    uint32_t staticmesh_count;
    nusr_staticmesh_t *staticmeshes;
} nusr_scene_data_t;

static nusr_scene_data_t _data;

static void vertex_shader(nu_vec3_t pos, nu_mat4_t m, nu_vec4_t dest)
{
    /* multiply MVP */
    nu_vec4(pos, 1.0f, dest);
    nu_mat4_mulv(m, dest, dest);
}

static void clip_edge_near(
    nu_vec4_t v0, nu_vec4_t v1, nu_vec4_t vclip,
    nu_vec2_t uv0, nu_vec2_t uv1, nu_vec2_t uvclip
)
{
    const nu_vec4_t near_plane = {0, 0, 1, 1};
    float d0 = nu_vec4_dot(v0, near_plane);
    float d1 = nu_vec4_dot(v1, near_plane);
    float s = d0 / (d0 - d1);
    nu_vec4_lerp(v0, v1, s, vclip);
    nu_vec2_lerp(uv0, uv1, s, uvclip);
}
static bool clip_triangle(
    nu_vec4_t vertices[4],
    nu_vec2_t uvs[4],
    uint32_t indices[6],
    uint32_t *indice_count
)
{
    /* default triangle output */
    *indice_count = 3;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    /* compute outsides */
    bool outside[3];
    for (uint32_t i = 0; i < 3; i++) {
        outside[i] = (vertices[i][3] <= 0) || (vertices[i][2] < -vertices[i][3]);
    }

    /* early test out */
    if ((outside[0] & outside[1] & outside[2]) != 0) return false;
    /* early test in  */
    if ((outside[0] | outside[1] | outside[2]) == 0) return true;

    /* clip vertices */
    for (uint32_t i = 0; i < 3; i++) {
        float *vec, *vec_prev, *vec_next;
        float *uv, *uv_prev, *uv_next;
        bool out, out_prev, out_next;
        vec = vertices[i];
        uv = uvs[i];
        out = outside[i];

        vec_next = vertices[(i + 1) % 3];
        uv_next = uvs[(i + 1) % 3];
        out_next = outside[(i + 1) % 3];
        
        vec_prev = vertices[(i + 2) % 3];
        uv_prev = uvs[(i + 2) % 3];
        out_prev = outside[(i + 2) % 3];

        if (out) {
            if (out_next) { /* 2 out case 1 */
                clip_edge_near(vec, vec_prev, vec, uv, uv_prev, uv);
            } else if (out_prev) { /* 2 out case 2 */
                clip_edge_near(vec, vec_next, vec, uv, uv_next, uv);
            } else { /* 1 out */
                /* produce new vertex */
                clip_edge_near(vec, vec_next, vertices[3], uv, uv_next, uvs[3]);
                *indice_count = 6;
                indices[3] = i;
                indices[4] = 3; /* new vertex */
                indices[5] = (i + 1) % 3;

                /* clip existing vertex */
                clip_edge_near(vec, vec_prev, vec, uv, uv_prev, uv);

                return true;
            }
        }
    }

    return true;
}
static void vertex_to_viewport(nu_vec2_t v, nu_vec4_t vp)
{
    /* (p + 1) / 2 */
    nu_vec2_adds(v, 1.0f, v);
    nu_vec2_muls(v, 0.5f, v);
    
    /* convert to viewport */
    nu_vec2_mul(v, vp + 2, v);
    nu_vec2_add(v, vp + 0, v);
}

static float pixel_coverage(const nu_vec2_t a, const nu_vec2_t b, const nu_vec2_t c)
{
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}

nu_result_t nusr_scene_initialize(void)
{
    /* camera */
    nu_vec3_zero(_data.camera.eye);
    nu_vec3_copy(_data.camera.center, (nu_vec3_t){0, 0, -1});
    nu_vec3_copy(_data.camera.up, (nu_vec3_t){0, 1, 0});
    _data.camera.fov = 90.0f;
    _data.camera.near = 1.0f;
    _data.camera.far = 1000.0f;

    /* staticmesh */
    _data.staticmesh_count = 0;
    _data.staticmeshes = (nusr_staticmesh_t*)nu_malloc(sizeof(nusr_staticmesh_t) * MAX_STATICMESH_COUNT);
    for (uint32_t i = 0; i < MAX_STATICMESH_COUNT; i++) {
        _data.staticmeshes[i].active = false;
    }

    return NU_SUCCESS;
}
nu_result_t nusr_scene_terminate(void)
{
    nu_free(_data.staticmeshes);

    return NU_SUCCESS;
}
nu_result_t nusr_scene_render(nusr_framebuffer_t *color_buffer, nusr_framebuffer_t *depth_buffer)
{
    /* clear buffers */
    nusr_framebuffer_clear(color_buffer, 0x0);
    nusr_framebuffer_clear(depth_buffer, 0xFFFF7F7F);

    /* compute VP matrix from camera information */
    nu_mat4_t camera_projection, camera_view, vp;
    nu_lookat(_data.camera.eye, _data.camera.center, (nu_vec3_t){0.0f, 1.0f, 0.0f}, camera_view);
    const float aspect = (double)color_buffer->width / (double)color_buffer->height;
    nu_perspective(_data.camera.fov, aspect, _data.camera.near, _data.camera.far, camera_projection);
    nu_mat4_mul(camera_projection, camera_view, vp);

    /* iterate over staticmeshes */
    for (uint32_t i = 0; i < _data.staticmesh_count; i++) {
        if (!_data.staticmeshes[i].active) continue;

        /* compute mvp matrix */
        nu_mat4_t mvp;
        nu_mat4_mul(vp, _data.staticmeshes[i].transform, mvp);

        /* access mesh */
        nusr_mesh_t *mesh;
        nusr_mesh_get(_data.staticmeshes[i].mesh, &mesh);

        /* access texture */
        nusr_texture_t *texture;
        nusr_texture_get(_data.staticmeshes[i].texture, &texture);

        /* iterate over mesh triangles */
        for (uint32_t vi = 0; vi < mesh->vertex_count; vi += 3) {
            nu_vec4_t viewport = {0, 0, color_buffer->width, color_buffer->height};
            nu_vec4_t tv[4]; /* one vertice can be added for the clipping step */
            nu_vec2_t uv[4]; /* one uv can be added for the clipping step */

            /* transform vertices */
            vertex_shader(mesh->positions[vi + 0], mvp, tv[0]);
            vertex_shader(mesh->positions[vi + 1], mvp, tv[1]);
            vertex_shader(mesh->positions[vi + 2], mvp, tv[2]);

            /* copy uv (should be done in vertex shader) */
            nu_vec2_copy(mesh->uvs[vi + 0], uv[0]);
            nu_vec2_copy(mesh->uvs[vi + 1], uv[1]);
            nu_vec2_copy(mesh->uvs[vi + 2], uv[2]);

            /* clip vertices */
            uint32_t indices[6];
            uint32_t indice_count;
            if (!clip_triangle(tv, uv, indices, &indice_count)) continue;

            /* perspective divide (NDC) */
            uint32_t total_vertex = (indice_count > 3) ? 4 : 3;
            for (uint32_t i = 0; i < total_vertex; i++) {
                nu_vec3_muls(tv[i], 1.0f / tv[i][3], tv[i]);
            }
            
            for (uint32_t idx = 0; idx < indice_count; idx += 3) {
                nu_vec4_t v0, v1, v2;
                nu_vec2_t uv0, uv1, uv2;

                /* vertices to viewport */
                nu_vec4_copy(tv[indices[idx + 0]], v0);
                nu_vec4_copy(tv[indices[idx + 1]], v1);
                nu_vec4_copy(tv[indices[idx + 2]], v2);
                vertex_to_viewport(v0, viewport);
                vertex_to_viewport(v1, viewport);
                vertex_to_viewport(v2, viewport);

                /* copy uvs */
                nu_vec2_copy(uv[indices[idx + 0]], uv0);
                nu_vec2_copy(uv[indices[idx + 1]], uv1);
                nu_vec2_copy(uv[indices[idx + 2]], uv2);

                /* backface culling */
                float area = pixel_coverage(v0, v1, v2);
                if (area < 0) continue;

                /* compute triangle viewport */
                nu_vec4_t tvp;
                float xmin = NU_MIN(v0[0], NU_MIN(v1[0], v2[0]));
                float xmax = NU_MAX(v0[0], NU_MAX(v1[0], v2[0]));
                float ymin = NU_MIN(v0[1], NU_MIN(v1[1], v2[1]));
                float ymax = NU_MAX(v0[1], NU_MAX(v1[1], v2[1]));
                tvp[0] = NU_MAX(0, xmin);
                tvp[1] = NU_MAX(0, ymin);
                tvp[2] = NU_MIN(viewport[2], xmax);
                tvp[3] = NU_MIN(viewport[3], ymax);

                /* compute edges */
                nu_vec2_t edge0, edge1, edge2;
                nu_vec2_sub(v2, v1, edge0);
                nu_vec2_sub(v0, v2, edge1);
                nu_vec2_sub(v1, v0, edge2);

                /* top left rule */
                bool t0 = (edge0[0] != 0) ? (edge0[0] > 0) : (edge0[1] > 0);
                bool t1 = (edge1[0] != 0) ? (edge1[0] > 0) : (edge1[1] > 0);
                bool t2 = (edge2[0] != 0) ? (edge2[0] > 0) : (edge2[1] > 0);

                for (uint32_t j = tvp[1]; j < tvp[3]; j++) {
                    for (uint32_t i = tvp[0]; i < tvp[2]; i++) {
                        nu_vec2_t sample = {i + 0.5, j + 0.5};

                        float w0 = pixel_coverage(v1, v2, sample);
                        float w1 = pixel_coverage(v2, v0, sample);
                        float w2 = pixel_coverage(v0, v1, sample);
                        
                        /* check sample with top left rule */
                        bool included = true;
                        included &= (w0 == 0) ? t0 : (w0 > 0);
                        included &= (w1 == 0) ? t1 : (w1 > 0);
                        included &= (w2 == 0) ? t2 : (w2 > 0);

                        if (included) {
                            const float area_inv = 1.0f / area;
                            w0 *= area_inv;
                            w1 *= area_inv;
                            w2 = 1.0f - w0 - w1;

                            /* depth test */
                            float depth = (w0 * v0[3] + w1 * v1[3] + w2 * v2[3]);
                            if (depth < depth_buffer->pixels[j * depth_buffer->width + i].as_float) {
                                depth_buffer->pixels[j * depth_buffer->width + i].as_float = depth;
                                
                                /* correct linear interpolation */

                                /*     a * f_a / w_a   +   b * f_b / w_b   +  c * f_c / w_c  *
                                 * f=-----------------------------------------------------   *
                                 *        a / w_a      +      b / w_b      +     c / w_c     */
                                 
                                float a = w0 / v0[3];
                                float b = w1 / v1[3];
                                float c = w2 / v2[3];
                                float inv_sum_abc = 1.0f / (a + b + c);

                                /* interpolate uv coordinates */
                                float px = (a * uv0[0] + b * uv1[0] + c * uv2[0]) * inv_sum_abc * texture->width;
                                float py = (a * uv0[1] + b * uv1[1] + c * uv2[1]) * inv_sum_abc * texture->height;
                                uint32_t uvx = NU_MAX(0, NU_MIN(texture->width, (uint32_t)px));
                                uint32_t uvy = NU_MAX(0, NU_MIN(texture->height, (uint32_t)py));

                                uint32_t color = texture->data[uvy * texture->width + uvx];
                                // uint32_t value = (uint32_t)(depth * 255.0f);
                                // color = (value << 24) + (value << 16) + (value << 8);
                                color_buffer->pixels[j * color_buffer->width + i].as_uint = color;
                            }
                        }
                    }
                }
            }
        }
    }

    return NU_SUCCESS;
}

nu_result_t nusr_scene_camera_set_fov(float fov)
{
    _data.camera.fov = fov;
    return NU_SUCCESS;
}
nu_result_t nusr_scene_camera_set_eye(const nu_vec3_t eye)
{
    nu_vec3_copy(eye, _data.camera.eye);
    return NU_SUCCESS;
}
nu_result_t nusr_scene_camera_set_center(const nu_vec3_t center)
{
    nu_vec3_copy(center, _data.camera.center);
    return NU_SUCCESS;
}
nu_result_t nusr_scene_camera_set_transform(const nu_mat4_t transform)
{
    // vec3 temp;
    // glm_vec3_copy(temp, (vec3){0, 0, -1});
    // glm_mat4_mulv3(transform, temp, 0, temp);
    nu_fatal("not implemented.\n");
    return NU_SUCCESS;
}

nu_result_t nusr_scene_staticmesh_create(uint32_t *id, const nusr_staticmesh_create_info_t *info)
{
    uint32_t found_id = MAX_STATICMESH_COUNT;
    for (uint32_t i = 0; i < _data.staticmesh_count; i++) {
        if (!_data.staticmeshes[i].active) {
            found_id = i;
            break;
        }
    }

    if (found_id == MAX_STATICMESH_COUNT) {
        if (_data.staticmesh_count >= MAX_STATICMESH_COUNT) return NU_FAILURE;
        found_id = _data.staticmesh_count;
        _data.staticmesh_count++;
    }

    _data.staticmeshes[found_id].active = true;
    _data.staticmeshes[found_id].mesh = info->mesh;
    _data.staticmeshes[found_id].texture = info->texture;
    nu_mat4_copy(info->transform, _data.staticmeshes[found_id].transform);

    *id = found_id;

    return NU_SUCCESS;
}
nu_result_t nusr_scene_staticmesh_destroy(uint32_t id)
{
    if (!_data.staticmeshes[id].active) return NU_FAILURE;

    _data.staticmeshes[id].active = false;

    return NU_SUCCESS;
}
nu_result_t nusr_scene_staticmesh_set_transform(uint32_t id, const nu_mat4_t m)
{
    if (!_data.staticmeshes[id].active) return NU_FAILURE;

    nu_mat4_copy(m, _data.staticmeshes[id].transform);

    return NU_SUCCESS;
}