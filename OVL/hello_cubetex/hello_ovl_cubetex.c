#include "../common.h"
#include "cubetex.c"

#ifdef STANDALONE
    extern unsigned long _binary_TIM_cubetex_tim_start[];
#else
    extern unsigned long _binary_OVL_hello_cubetex_TIM_cubetex_tim_start[];
#endif

int ovl_main_cubetex() 
{    
    #ifndef STANDALONE
        useOT = 1;
    #endif
    uint16_t i = 0;
    int PadStatus;
    int TPressed=0;
    int AutoRotate=1;
    long t, p, OTz, Flag;                // t == vertex count, p == depth cueing interpolation value, OTz ==  value to create Z-ordered OT, Flag == see LibOver47.pdf, p.143
    POLY_GT3 *poly = {0};                           // pointer to a POLY_G4 
    SVECTOR Rotate={ 256, 256, 0, 0 };                   // Rotation coordinates
    VECTOR  Trans={ 0, 0, CENTERX, 0 };     // Translation coordinates
    VECTOR  Scale={ ONE, ONE, ONE, 0 };     // ONE == 4096
    MATRIX  Matrix={0};                     // Matrix data for the GTE
    DR_MODE * dr_mode;                        // Pointer to dr_mode prim
    RECT tws = {0, 0, 32, 32};            // Texture window coordinates : x, y, w, h
    init();
    #ifdef STANDALONE
        LoadTexture(_binary_TIM_cubetex_tim_start, &tim_cube);
    #else
        LoadTexture(_binary_OVL_hello_cubetex_TIM_cubetex_tim_start, &tim_cube);
    #endif

    int primSet = 0;

    // Main loop
    while (1) {
        i++;
        // Read pad status
        PadStatus = PadRead(0);
        if (AutoRotate == 0) {
            if (PadStatus & PADL1) Trans.vz -= 4;
            if (PadStatus & PADR1) Trans.vz += 4;
            if (PadStatus & PADL2) Rotate.vz -= 8;
            if (PadStatus & PADR2) Rotate.vz += 8;
            if (PadStatus & PADLup)     Rotate.vx -= 8;
            if (PadStatus & PADLdown)   Rotate.vx += 8;
            if (PadStatus & PADLleft)   Rotate.vy -= 8;
            if (PadStatus & PADLright)  Rotate.vy += 8;
            if (PadStatus & PADRup)     Trans.vy -= 2;
            if (PadStatus & PADRdown)   Trans.vy += 2;
            if (PadStatus & PADRleft)   Trans.vx -= 2;
            if (PadStatus & PADRright)  Trans.vx += 2;
            if (PadStatus & PADselect) {
                Rotate.vx = Rotate.vy = Rotate.vz = 0;
                Scale.vx = Scale.vy = Scale.vz = ONE;
                Trans.vx = Trans.vy = 0;
                Trans.vz = CENTERX;
            }
        }
        if (PadStatus & PADstart) {
            if (TPressed == 0) {
                AutoRotate = (AutoRotate + 1) & 1;
                Rotate.vx = Rotate.vy = Rotate.vz = 0;
                Scale.vx = Scale.vy = Scale.vz = ONE;
                Trans.vx = Trans.vy = 0;
                Trans.vz = CENTERX;
            }
            TPressed = 1;
        } else {
            TPressed = 0;
        }
        if (AutoRotate) {
            Rotate.vy += 8; // Pan
            Rotate.vx += 8; // Tilt
        }
        // Clear the current OT
        ClearOTagR(ot[db], OTLEN);
        // Convert and set the matrices
        RotMatrix(&Rotate, &Matrix);
        TransMatrix(&Matrix, &Trans);
        ScaleMatrix(&Matrix, &Scale);
        SetRotMatrix(&Matrix);
        SetTransMatrix(&Matrix);
        // Render the sample vector model
        // modelCubeTex is a TMESH, len member == # vertices, but here it's # of triangle... So, for each tri * 3 vertices ...
        //~ if (primSet == 0){
        for (int v = 0; v < (modelCubeTex.len*3); v += 3) {               
            poly = (POLY_GT3 *)nextpri;
            // Initialize the primitive and set its color values
            SetPolyGT3(poly);
            ((POLY_GT3 *)poly)->tpage = getTPage(tim_cube.mode&0x3, 0,
                                                 tim_cube.prect->x,
                                                 tim_cube.prect->y
                                                );
            setRGB0(poly, modelCubeTex.c[v].r , modelCubeTex.c[v].g   , modelCubeTex.c[v].b);
            setRGB1(poly, modelCubeTex.c[v+2].r, modelCubeTex.c[v+2].g, modelCubeTex.c[v+2].b);
            setRGB2(poly, modelCubeTex.c[v+1].r, modelCubeTex.c[v+1].g, modelCubeTex.c[v+1].b);
            setUV3(poly, modelCubeTex.u[v].vx, modelCubeTex.u[v].vy,
                         modelCubeTex.u[v+2].vx, modelCubeTex.u[v+2].vy,
                         modelCubeTex.u[v+1].vx, modelCubeTex.u[v+1].vy);
            // Rotate, translate, and project the vectors and output the results into a primitive
            OTz  = RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v]]  , (long*)&poly->x0, &p, &Flag);
            OTz += RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v+2]], (long*)&poly->x1, &p, &Flag);
            OTz += RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v+1]], (long*)&poly->x2, &p, &Flag);
            // Sort the primitive into the OT
            OTz /= 3;
            if ((OTz > 0) && (OTz < OTLEN))
                AddPrim(&ot[db][OTz-2], poly);
            nextpri += sizeof(POLY_GT3);
        }
            //~ dr_mode = (DR_MODE *)nextpri;
            //~ setDrawMode(dr_mode,1,0, getTPage(tim_cube.mode&0x3, 0,
                                              //~ tim_cube.prect->x,
                                              //~ tim_cube.prect->y), &tws);  //set texture window
            //~ AddPrim(&ot[db], dr_mode);
            //~ nextpri += sizeof(DR_MODE);
            //~ primSet = 1;
        //~ } else {
            //~ nextpri = primbuff[db];
            //~ for (int v = 0; v < (modelCubeTex.len*3); v += 3) {  
                //~ // Rotate, translate, and project the vectors and output the results into a primitive
                //~ OTz  = RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v]]  , (long*) &((POLY_GT3 * ) (&(primbuff[0][ sizeof(POLY_GT3) * (v/3) ]) ) )->x0, &p, &Flag);
                //~ OTz += RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v+2]], (long*) &((POLY_GT3 * ) (&(primbuff[0][ sizeof(POLY_GT3) * (v/3) ]) ) )->x1, &p, &Flag);
                //~ OTz += RotTransPers(&modelCubeTex_mesh[modelCubeTex_index[v+1]], (long*) &((POLY_GT3 * ) (&(primbuff[0][ sizeof(POLY_GT3) * (v/3) ]) ) )->x2, &p, &Flag);
                //~ // Sort the primitive into the OT
                //~ OTz /= 3;
                //~ if ((OTz > 0) && (OTz < OTLEN))
                    //~ AddPrim(&ot[db][OTz-2], (POLY_GT3 * ) (&(primbuff[0][ sizeof(POLY_GT3) * (v/3) ])) );
            //~ }
        //~ }
        FntPrint(0, "Hello textured cube! %d\n", i);
        FntFlush(0);
        #ifndef STANDALONE
            if (i == timeout){
                useOT = 0;
                next_overlay = MOTHERSHIP;
                break;
            }
        #endif
        display();
        
    } 
    return next_overlay;
}
