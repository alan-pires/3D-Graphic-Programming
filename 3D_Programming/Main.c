#include <stdio.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

triangle_t* triangles_to_render = NULL;

bool is_running = false;
float glb_fov = 640;
int previous_frame_time = 0;
float speedRotation = 0.01;

vec3_t  glb_cameraPos = { 0, 0, 0 };

bool setup(void)
{
    colorBuffer = (uint32_t*)malloc(window_width * window_height * sizeof(uint32_t));
    if (!colorBuffer)
    {
        fprintf(stderr, "Error allocating colorBuffer memory\n");
        return false;
    }

    //Creating a SDL texture that is used to display the color buffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    //load_cube_mash_data(); 
    load_obj_file_data("C:/Users/Alan_/Desktop/all/3D_Programming/3D_Programming/cube.obj");
    //load_obj_file_data("C:/Users/Alan_/Desktop/all/3D_Programming/3D_Programming/f22.obj");


    return true;
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        if (event.key.keysym.sym == SDLK_UP)
            if (speedRotation <= 0.09) speedRotation+= 0.01;
        if (event.key.keysym.sym == SDLK_DOWN)
            if (speedRotation > 0) speedRotation-= 0.01;
        break;
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        (point.x * glb_fov) / point.z,
        (point.y * glb_fov) / point.z
    };
    return projected_point;
}

void update(void)
{
    Uint32 time_ellapsed = SDL_GetTicks() - previous_frame_time;
    int time_to_wait = FRAME_TARGET_TIME - time_ellapsed;


    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // a partir daqui até o proximo getTicks será o tempo que levou para executar as instruçoes do update
    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += speedRotation;
    mesh.rotation.y += speedRotation;
    mesh.rotation.z += speedRotation;

    int num_faces = array_length(mesh.faces);

    for (int i = 0; i < num_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // ******* Loop all three vertices of this current face to apply transformations *******
        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // translate the vertices away from the camera
            transformed_vertex.z += 5;

            transformed_vertices[j] = transformed_vertex;
        }
        // ******* Loop all three vertices of this current face to apply transformations ******


        // ******* Check the backface culling *******
        vec3_t vec_a = transformed_vertices[0]; /*   A   */
        vec3_t vec_b = transformed_vertices[1]; /*  / \  */
        vec3_t vec_c = transformed_vertices[2]; /* C---B */

        vec3_t vec_ab = vec3_sub(vec_b, vec_a);
        vec3_t vec_ac = vec3_sub(vec_c, vec_a);
        vec3_normalize(&vec_ab);
        vec3_normalize(&vec_ac);
         
        // Compute the face normal using cross product to find the perpendicular
        vec3_t normal = vec3_crossP(vec_ab, vec_ac);

        vec3_normalize(&normal);

        // Find the vector between a point in the triangle and the camera origin
        vec3_t camera_ray = vec3_sub(glb_cameraPos, vec_a);

        // calculate how aligned the camera ray is with the face normal
        float dot_normal_camera = vec3_dotP(normal, camera_ray);

        // bypass the triangles that are looking away from the camera
        if (dot_normal_camera < 0) continue;
        // ******* Check the backface culling *******


        // ******* Loop all threee vertices to perform projection *******
        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++ ) {
            vec2_t projected_point = project(transformed_vertices[j]);

            projected_point.x += ((float)window_width / 2);
            projected_point.y += ((float)window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        array_push(triangles_to_render, projected_triangle);
        // ****** Loop all threee vertices to perform projection ******
    }
}

void render(void)
{
    int num_triangles = array_length(triangles_to_render);


    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];
        draw_rectangle(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);

        draw_triangle(
            triangle.points[0].x, triangle.points[0].y,
            triangle.points[1].x, triangle.points[1].y,
            triangle.points[2].x, triangle.points[2].y,
            0xFF00FF00
        );
    }
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0x00000000);
    SDL_RenderPresent(renderer);
}

void free_resources()
{
    free(colorBuffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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