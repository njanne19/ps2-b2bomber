#include "graphics.h" 
#include <stdlib.h> 
#include <time.h> 
#include <tamtypes.h>
#include <math3d.h>
#include "assets/arrow.c"

// Declare some static variables that are used in the long term. 
static GSFONTM* gsFontM = NULL; // Global font object for rendering text. 
static time_t rawtime; 
static struct tm* timeinfo; // Time structure for displaying current time.
static char timeString[256]; // Buffer for formatted time string.


// Configuration of GUI arrangement 
#define BOX_COUNT 3 
#define BOX_MARGIN 10 
typedef struct {
    int x1, y1; // top-left corner 
    int x2, y2; // bottom-right corner
} BoxRect; 

static BoxRect gBoxes[BOX_COUNT]; // Array to hold the bottom box coordinates for GUI. 
static int gBoxesInitialized = 0; 


void flipScreen(GSGLOBAL* gsGlobal) {
    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);
}

GSGLOBAL* init_graphics() {
    /// Initializes the graphics subsystem for the PS2.
    /// Sets up the GS global structure, configures DMA, and prepares the screen.
    /// @return Pointer to the initialized GSGLOBAL structure.
    GSGLOBAL* gsGlobal = gsKit_init_global();

    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
    //gsGlobal->PrimAAEnable = GS_SETTING_ON;

    gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_set_clamp(gsGlobal, GS_CMODE_REPEAT);

    gsKit_vram_clear(gsGlobal);

    gsKit_init_screen(gsGlobal);

    gsKit_mode_switch(gsGlobal, GS_ONESHOT);

    return gsGlobal;
}

GSGLOBAL* GUI_init() {
    GSGLOBAL* gs = init_graphics(); 
    if (gsFontM == NULL) {
        gsFontM = gsKit_init_fontm();
        gsKit_fontm_upload(gs, gsFontM);
    }
    return gs; 
}

void GUI_next_frame(GSGLOBAL* gs) {
    // TODO: Yaw/Pitch/Roll not exactly the right orientaiton with respect to the arrow. 
    static float yaw = 180.0f, pitch = 0.0f, roll = 0.0f;
    gsKit_clear(gs, Black); 
    GUI_draw_title_and_time(gs);
    GUI_draw_bottom_boxes(gs);
    GUI_draw_arrow(gs, 0, yaw, pitch, roll);
    roll += 0.2f; // Increment yaw for demonstration
}

static void GUI_draw_title_and_time(GSGLOBAL* gs) {
    // Draw the title and current time at the top of the screen
    gsKit_fontm_print_scaled(gs, gsFontM, 10, 10, 3, 0.45f, GreenFont,
        "PS2 Bomber Guidance Flight Computer\n"
    );

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
    gsKit_fontm_print_scaled(gs, gsFontM, 10, 25, 3, 0.45f, GreenFont,
        timeString
    );
}

static void GUI_draw_bottom_boxes(GSGLOBAL* gs) {
    // Draw the bottom boxes in the GUI 
    if (!gBoxesInitialized) {
        compute_all_box_positions(gs);
    }

    for (int i = 0; i < BOX_COUNT; i++) {
        BoxRect* box = &gBoxes[i];
        // Draw the box edges
        gsKit_prim_line(gs, box->x1, box->y1, box->x2, box->y1, 1, GreenTrans); // top edge
        gsKit_prim_line(gs, box->x2, box->y1, box->x2, box->y2, 1, GreenTrans); // right edge
        gsKit_prim_line(gs, box->x2, box->y2, box->x1, box->y2, 1, GreenTrans); // bottom edge
        gsKit_prim_line(gs, box->x1, box->y2, box->x1, box->y1, 1, GreenTrans); // left edge
    }
}

static void compute_all_box_positions(GSGLOBAL* gs) {
    int W = gs->Width;
    int H = gs->Height;

    // Define the contained region 
    int region_top = H / 2; 
    int region_height = H - region_top; 

    // Compute boxes height and width 
    int box_width = (W - ((BOX_COUNT + 1) * BOX_MARGIN)) / BOX_COUNT; 
    int box_height = region_height - 2 * BOX_MARGIN; 

    for (int i = 0; i < BOX_COUNT; i++) {
        gBoxes[i].x1 = BOX_MARGIN + i * (box_width + BOX_MARGIN);
        gBoxes[i].y1 = region_top + BOX_MARGIN;
        gBoxes[i].x2 = gBoxes[i].x1 + box_width;
        gBoxes[i].y2 = gBoxes[i].y1 + box_height;
    }
    gBoxesInitialized = 1; // Mark boxes as initialized
}

static void get_box_position(int i, int *out_x1, int *out_y1, int *out_x2, int *out_y2) {
    // Get the position of the box at index i
    if (i < 0 || i >= BOX_COUNT || !gBoxesInitialized) {
        return; // Invalid index
    }
    BoxRect* box = &gBoxes[i];
    *out_x1 = box->x1;
    *out_y1 = box->y1;
    *out_x2 = box->x2;
    *out_y2 = box->y2;
}

static void GUI_draw_arrow(GSGLOBAL* gs, int i, float yawDeg, float pitchDeg, float rollDeg) {
    // 1) Fetch box-pixel bounds 
    int x1, y1, x2, y2;
    get_box_position(i, &x1, &y1, &x2, &y2);

    // Check to make sure the box is valid
    if (x1 >= x2 || y1 >= y2) {
        return; // Invalid box dimensions
    }

    int box_width = x2 - x1;
    int box_height = y2 - y1;
    int full_width = gs->Width;
    int full_height = gs->Height;

    // 2) Enable scissor so nothing draws outside this box
    gsKit_set_scissor(gs, GS_SETREG_SCISSOR(x1, y1, x2, y2));

    // 3) Build projection matrix for this box's aspect ratio 
    MATRIX viewProjMatrix; 
    float aspect = (float)box_width / (float)box_height;
    create_view_screen(viewProjMatrix, aspect, -0.5f,0.5f, -0.5f,0.5f, 1.0f, 2000.0f);

    
    // 4) Build View (camera) matrix
    // Camera should sit at (0, 0, 10), looking towards the origin 
    VECTOR camTranslation = {0, 0, 10, 1};
    VECTOR camEuler = {0, 0, 0, 1}; // No rotation
    MATRIX viewMatrix;

    create_world_view(viewMatrix, camTranslation, camEuler);

    // 5) Build Model matrix (rotate arrow by yaw/pitch/roll) in radians. 
    MATRIX modelMatrix; 
    matrix_unit(modelMatrix); // Start with identity matrix
    VECTOR arrowEuler = { 
        pitchDeg * DEG_TO_RAD, 
        rollDeg * DEG_TO_RAD, 
        yawDeg * DEG_TO_RAD, 
        1.0f 
    };

    // Matrix rotate applies Z->Y->X order, so we need to apply roll first, then pitch, then yaw.
    matrix_rotate(modelMatrix, modelMatrix, arrowEuler);

    // 6) Combine into MVP: Model -> View -> Projection
    MATRIX mvpMatrix;
    create_local_screen(mvpMatrix, modelMatrix, viewMatrix, viewProjMatrix);
    // internals: MVP = Projection * View * Model

    // 7) Unroll the arrow mesh into per-vertex arrays in cVerts[] and cColors[]
    extern int arrow_points_count; 
    extern int arrow_points[]; 
    extern VECTOR arrow_vertices[]; 
    extern VECTOR arrow_colors[]; 
    extern VECTOR arrow_colors[]; 
    int pc = arrow_points_count; // Number of points in the arrow mesh


    // Allocate memory for contiguous vertex and color arrays
    VECTOR *c_verts = memalign(128, sizeof(VECTOR) * pc);
    VECTOR *c_colors = memalign(128, sizeof(VECTOR) * pc);
    for (int idx = 0; idx < pc; idx++) {
        int vi = arrow_points[idx];
        memcpy(c_verts[idx], arrow_vertices[vi], sizeof(VECTOR));
        memcpy(c_colors[idx], arrow_colors[vi], sizeof(VECTOR));
    }

    // 8) Transform the vertices from object space to clip space 
    VECTOR *c_verts_clip = memalign(128, sizeof(VECTOR) * pc);
    calculate_vertices(c_verts_clip, pc, c_verts, mvpMatrix);

    // 9) Project the clip-space vertices into full-screen pixel coordinates 
    VECTOR *c_verts_pixel = memalign(128, sizeof(VECTOR) * pc);
    gsKit_convert_xyz((vertex_f_t*)c_verts_pixel, gs, pc, (vertex_f_t*)c_verts_clip);

    // 10) Remap the full-screen -> box local coordinates 
    for (int k = 0; k < pc; k++) {
        float sx = c_verts_pixel[k][0];  // full‐screen X
        float sy = c_verts_pixel[k][1];  // full‐screen Y
        float nx = sx / (float)full_width;  // normalize to [0..1]
        float ny = sy / (float)full_height;

        // Remap into [x1..x2], [y1..y2]:
        c_verts_pixel[k][0] = x1 + nx * box_width;
        c_verts_pixel[k][1] = y1 + (1.0f - ny) * box_height;  // flip Y
        // Leave proj[k][2] (depth) unchanged
    }

    // 11) Convert float RGB/A -> fixed and pack into color_t cols[] 
    color_t *c_colors_rgbq = memalign(128, sizeof(color_t) * pc);
    draw_convert_rgbq(c_colors_rgbq, pc, (vertex_f_t*)c_verts_clip, (color_f_t*)c_colors, 0x80);

    // 12) Build array of GSPRIMPOINTs to draw the triangles. 
    GSPRIMPOINT *prim = memalign(128, sizeof(GSPRIMPOINT) * pc);
    for (int n = 0; n < pc; n++) {
        prim[n].rgbaq = color_to_RGBAQ(
            c_colors_rgbq[n].r, c_colors_rgbq[n].g, c_colors_rgbq[n].b, c_colors_rgbq[n].a, 0.0f);
        prim[n].xyz2 = vertex_to_XYZ2(gs, c_verts_pixel[n][0], c_verts_pixel[n][1], c_verts_pixel[n][2]);
    }

    // 13) Draw the arrow triangles
    gsKit_prim_list_triangle_gouraud_3d(gs, pc, prim);

    // 14) Cleanup: free allocated memory
    free(c_verts);
    free(c_colors);
    free(c_verts_clip);
    free(c_verts_pixel);
    free(c_colors_rgbq);
    free(prim);

    // 15) Reset scissor to full screen
    gsKit_set_scissor(gs, GS_SETREG_SCISSOR(0, 0, full_width, full_height));

    return;
} 

static int gsKit_convert_xyz(vertex_f_t *output, GSGLOBAL *gsGlobal, int count, vertex_f_t *vertices) {
    int zBits;
    unsigned int max_z;

    // 1) Figure out how many bits of depth the GS is using:
    switch (gsGlobal->PSMZ) {
        case GS_PSMZ_32:
            zBits = 32;
            break;
        case GS_PSMZ_24:
            zBits = 24;
            break;
        case GS_PSMZ_16:
        case GS_PSMZ_16S:
            zBits = 16;
            break;
        default:
            // Unsupported format
            return -1;
    }

    // 2) Compute half‐width and half‐height (screen center):
    float halfW = gsGlobal->Width  / 2.0f;
    float halfH = gsGlobal->Height / 2.0f;

    // 3) For fixed‐point depth, we map z ∈ [−1..+1] into [0..(1 << (zBits−1))*2].
    //    max_z = 1 << (zBits−1) gives the positive half. (PS2 uses signed fixed‐point.)
    max_z = 1U << (zBits - 1);

    // 4) Loop over each vertex:
    for (int i = 0; i < count; i++) {
        // vertices[i].x, y, z ∈ [−1..+1] after MVP/perspective‐divide

        // Map x from [−1..+1] to [0..Width]:
        output[i].x = (vertices[i].x + 1.0f) * halfW;

        // Map y from [−1..+1] to [0..Height]:
        output[i].y = (vertices[i].y + 1.0f) * halfH;

        // Map z from [−1..+1] to [0..(max_z*2)] then cast to unsigned:
        output[i].z = (unsigned int) ((vertices[i].z + 1.0f) * max_z);
    }

    return 0;
}