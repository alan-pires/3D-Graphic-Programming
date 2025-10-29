#include <stdio.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "triangle.h"
#include "texture.h"
#include "matrix.h"
#include "light.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"
#include <direct.h>

triangle_t* triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;
float speedRotation = 0.01;
float delta_time = 0;

mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;


bool setup(void)
{
    set_render_method(RENDER_WIRE);
    set_cull_method(CULL_BACKFACE);
    
    init_light(vec3_new(0, 0, 1));

    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float fov_y = 3.141592 / 3.0; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov_y, aspect_y, znear, zfar);

    init_frustum_planes(fov_x, fov_y, znear, zfar);

    load_mesh("f22.obj", "f22.png", vec3_new(1, 1, 1), vec3_new(-3, 0, +8), vec3_new(0, 0, 0));
    load_mesh("drone.obj", "drone.png", vec3_new(1, 1, 1), vec3_new(+3, 0, +8), vec3_new(0, 0, 0));
    return true;
}

void process_input(void)
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type) 
        {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
                break;
            }
            if (event.key.keysym.sym == SDLK_1) {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (event.key.keysym.sym == SDLK_2) {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_3) {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (event.key.keysym.sym == SDLK_4) {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_5) {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (event.key.keysym.sym == SDLK_6) {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_c) {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (event.key.keysym.sym == SDLK_x) {
                set_cull_method(CULL_NONE);
                break;
            }
            if (event.key.keysym.sym == SDLK_w) {
                rotate_camera_pitch(-3.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_s) {
                rotate_camera_pitch(+3.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                rotate_camera_yaw(-1.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                rotate_camera_yaw(+1.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_UP) {
                update_camera_forward_velocity(vec3_mult(get_camera_direction(), 5.0 * delta_time));
                update_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                update_camera_forward_velocity(vec3_mult(get_camera_direction(), 5.0 * delta_time));
                update_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
                break;
            }
            break;
        }
    };
}

// Process the graphics pipeline stages for all the mesh triangles
// 
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
//
void process_graphics_pipeline_stages(mesh_t* mesh)
{
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.y);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

    // Update camera look at target to create view matrix
    vec3_t target = get_camera_lookat_target();
    vec3_t up_direction = vec3_new(0, 1, 0);
    view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

    // Loop all triangle faces of our mesh
    int num_faces = array_length(mesh->faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh->faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a];
        face_vertices[1] = mesh->vertices[mesh_face.b];
        face_vertices[2] = mesh->vertices[mesh_face.c];

        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face to apply transformations *******
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            world_matrix = mat4_identity();
            world_matrix = mat4_mult_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mult_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mult_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mult_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mult_mat4(translation_matrix, world_matrix);

            // Multiply the world matrix by the original vector
            transformed_vertex = mat4_mult_vec4(world_matrix, transformed_vertex);

            // Multiply the view matrix by the vector to transform the scene to camera space
            transformed_vertex = mat4_mult_vec4(view_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t face_normal = get_triangle_normal(transformed_vertices);

        if (is_cull_backface())
        {
            // Find the vector between a point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(vec3_new(0, 0, 0), vec3_from_vec4(transformed_vertices[0]));

            // calculate how aligned the camera ray is with the face normal
            float dot_normal_camera = vec3_dotP(face_normal, camera_ray);

            // bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0)
                continue;
        }

        // CLIPPING
        polygon_t polygon = polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );

        clip_polygon(&polygon);

        triangle_t triagles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triagles_after_clipping, &num_triangles_after_clipping);

        for (int t = 0; t < num_triangles_after_clipping; t++)
        {
            triangle_t triangle_after_clip = triagles_after_clipping[t];
            // Loop all threee vertices to perform projection
            vec4_t projected_points[3];

            for (int j = 0; j < 3; j++) {
                projected_points[j] = mat4_mult_vec4_project(proj_matrix, triangle_after_clip.points[j]);

                // Scale into the view
                projected_points[j].x *= ((float)get_window_width() / 2);
                projected_points[j].y *= ((float)get_window_height() / 2);

                // Invert the y values to account for flipped screen y coordinate
                projected_points[j].y *= -1;

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (get_window_width() / 2.0);
                projected_points[j].y += (get_window_height() / 2.0);
            }

            float light_intensity_factor = -vec3_dotP(face_normal, get_light_direction());

            // Calculate the triangle color based on the light angle
            uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t triangleToRender = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
                },
                .texcoords = {
                    { triangle_after_clip.texcoords[0].u, triangle_after_clip.texcoords[0].v },
                    { triangle_after_clip.texcoords[1].u, triangle_after_clip.texcoords[1].v },
                    { triangle_after_clip.texcoords[2].u, triangle_after_clip.texcoords[2].v }
                },
                .color = triangle_color,
                .texture = mesh->texture
            };

            array_push(triangles_to_render, triangleToRender);
        }
    }
}

void update(void)
{
    Uint32 time_ellapsed = SDL_GetTicks() - previous_frame_time;
    int time_to_wait = FRAME_TARGET_TIME - time_ellapsed;


    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    // a partir daqui até o proximo getTicks será o tempo que levou para executar as instruçoes do update
    previous_frame_time = SDL_GetTicks();

        // Loop all the meshes of our scene
    for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++)
    {
        mesh_t* mesh = get_mesh(mesh_index);
        // rotate_mesh_x(mesh_index, mesh->rotation_velocity.x * delta_time);
        // rotate_mesh_y(mesh_index, mesh->rotation_velocity.y * delta_time);
        // rotate_mesh_z(mesh_index, mesh->rotation_velocity.z * delta_time);
        process_graphics_pipeline_stages(mesh);
    }
}

void render(void)
{
    //SDL_RenderClear(renderer);
    //draw_grid();
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (should_render_filled_triangles()) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // vertex C
                triangle.color
            );
        }

        if (should_render_textured_triangles())
        {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v, // vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, // vertex C
                triangle.texture
            );
        }

        if (should_render_wireframe()) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, // vertex C
                0xFFFFFFFF
            );
        }

        if (should_render_wire_vertex()) {
            draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
            draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
            draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C
        }
    }
    
    array_reset(triangles_to_render);
    render_color_buffer();
}

void free_resources()
{
    free_meshes();
    array_free(triangles_to_render);
}

int main(void)
{
    is_running = initialize_window();

    setup();

    vec3_t myvec = { 1.0, 3.0, -4.0 };

    while (is_running)
    {
        process_input();
        update();
        render();
    }
    destroy_window();
    free_resources();

    return 0;
}