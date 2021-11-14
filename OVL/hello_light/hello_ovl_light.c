#include "../common.h"
// Sample vector model
#include "cube.c"

long t, p, OTz, Flag;              
// Lighting
CVECTOR BGc = {150, 50, 75, 0};  
VECTOR  BKc = {128, 128, 128, 0};
SVECTOR lgtang = {0, 0, 0}; 
MATRIX  rotlgt, rotcube, light;
MATRIX lgtmat = {
//      X      Y      Z
        -ONE,  -ONE,   ONE, // Lightsource 1 : here, the light source is at the Bottom-Left of the screen, and points into the screen.
        0,     0,     0, // Lightsource 2
        0,     0,     0, // Lightsource 3
    };
// Local Color Matrix
MATRIX cmat = {
//   L1    L2   L3
    4096,   0,   0, // R
    4096,   0,   0, // G
    4096,   0,   0  // B
    };

static void setLightEnv()
{
    // Set light env
    // Set far color
    SetFarColor( BGc.r, BGc.g, BGc.b );
    // Set Ambient color
    SetBackColor( BKc.vx, BKc.vy, BKc.vz );
    // Set Color matrix
    SetColorMatrix(&cmat);
    // Set Fog settings
    SetFogNearFar( 1200, 2200, SCREENXRES );
}

int ovl_main_light()
{
    #ifndef STANDALONE
        useOT = 1;
    #endif
    uint16_t i = 0;
    int PadStatus;
    int TPressed=0;
    int AutoRotate=1;
    // Rotating cube
    POLY_G3 * poly;
    SVECTOR Rotate={ ONE/6,ONE/6,ONE/6 };                   // Rotation coordinates
    VECTOR  Trans={ -SCREENXRES/2, 0, CENTERX * 3, 0 }; // Translation coordinates
    VECTOR  Scale={ ONE/2, ONE/2, ONE/2, 0 };     // Scaling coordinates : ONE == 4096
    MATRIX  Matrix={0};                     // Matrix data for the GTE
    // Static cube
    POLY_G3 * poly1;                   // pointer to a POLY_G4 
    SVECTOR Rotate1={ ONE/6, ONE/6, ONE/6, 0 };                   // Rotation coordinates
    VECTOR  Trans1={ SCREENXRES/2, 0, CENTERX * 3, 0 }; // Translation coordinates
    VECTOR  Scale1={ ONE/2, ONE/2, ONE/2, 0 };     // Scaling coordinates : ONE == 4096
    MATRIX  Matrix1={0};                     // Matrix data for the GTE
    init();
    setLightEnv();
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
        }
        if (PadStatus & PADstart) {
            if (TPressed == 0) {
                AutoRotate = (AutoRotate + 1) & 1;
                Rotate.vy = Rotate.vx = Rotate.vz = ONE/6;
                Scale.vx = Scale.vy = Scale.vz = ONE/2;
                Trans.vx = -SCREENXRES/2;
                Trans.vy = 0;
                Trans.vz = CENTERX * 3;
            }
            TPressed = 1;
        } else {
            TPressed = 0;
        }
        if (AutoRotate) {
            Rotate.vy += 8; // Pan
            Rotate.vx += 8; // Tilt
            //~ Rotate.vz += 8; // Roll
        }
        // Clear the current OT
        ClearOTagR(ot[db], OTLEN);
        // Render the sample vector model
        t=0;        
        // modelCube is a TMESH, len member == # vertices, but here it's # of triangle... So, for each tri * 3 vertices ...
        for (int v = 0; v < (modelCube.len*3); v += 3) {               
            poly = (POLY_G3 *)nextpri;
            // Initialize the primitive and set its color values
            SetPolyG3(poly);
            // Rotate, translate, and project the vectors and output the results into a primitive
            // Could be replaced with one call with RotTransPers3()
            OTz  = RotTransPers(&modelCube_mesh[modelCube_index[t]]  , (long*)&poly->x0, &p, &Flag);
            OTz += RotTransPers(&modelCube_mesh[modelCube_index[t+2]], (long*)&poly->x1, &p, &Flag);
            OTz += RotTransPers(&modelCube_mesh[modelCube_index[t+1]], (long*)&poly->x2, &p, &Flag);
            // Find light color
            // Work color vectors
            CVECTOR outCol, outCol1, outCol2  = { 0,0,0,0 };
            // Find local color from three normal vectors and perform depth cueing.
            // Could be replaced with one call with NormalColorDpq3()
            NormalColorDpq(&modelCube.n[ modelCube_index[t+0] ], &modelCube.c[v+0], p, &outCol);
            NormalColorDpq(&modelCube.n[ modelCube_index[t+2] ], &modelCube.c[v+2], p, &outCol1);
            NormalColorDpq(&modelCube.n[ modelCube_index[t+1] ], &modelCube.c[v+1], p, &outCol2);
            // Set vertex colors 
            setRGB0(poly, outCol.r, outCol.g  , outCol.b);
            setRGB1(poly, outCol1.r, outCol1.g, outCol1.b);
            setRGB2(poly, outCol2.r, outCol2.g, outCol2.b);
            // Sort the primitive into the OT
            OTz /= 3;
            if ((OTz > 0) && (OTz < OTLEN))
                AddPrim(&ot[db][OTz-2], poly);
            nextpri += sizeof(POLY_G3);
            t+=3;
        }
        // Find and apply light rotation matrix
        // Find rotmat from light angles
        RotMatrix_gte(&lgtang, &rotlgt);
        // Find rotmat from cube angles
        RotMatrix_gte(&Rotate, &rotcube);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&rotcube, &rotlgt, &rotlgt);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&lgtmat, &rotlgt, &light);
        // Set new light matrix 
        SetLightMatrix(&light);
        // Convert and set the matrices
        // Find Rotation matrix from object's angles
        RotMatrix(&Rotate, &Matrix); 
        // Find Scale matrix from object's angles
        ScaleMatrix(&Matrix, &Scale);
        // Find Translation matrix from object's angles
        TransMatrix(&Matrix, &Trans);
        // Set GTE's rotation matrix
        SetRotMatrix(&Matrix);
        // Set GTE's Translation matrix
        SetTransMatrix(&Matrix);
        // Draw static cube
        t=0;   
        for (int v = 0; v < (modelCube1.len*3); v += 3) {               
            poly1 = (POLY_G3 *)nextpri;
            SetPolyG3(poly1);
            OTz  = RotTransPers(&modelCube1_mesh[modelCube1_index[t]]  , (long*)&poly1->x0, &p, &Flag);
            OTz += RotTransPers(&modelCube1_mesh[modelCube1_index[t+2]], (long*)&poly1->x1, &p, &Flag);
            OTz += RotTransPers(&modelCube1_mesh[modelCube1_index[t+1]], (long*)&poly1->x2, &p, &Flag);
            CVECTOR outCol  = { 0,0,0,0 };
            CVECTOR outCol1 = { 0,0,0,0 };
            CVECTOR outCol2 = { 0,0,0,0 };
            NormalColorDpq(&modelCube1.n[ modelCube1_index[t+0] ], &modelCube1.c[v+0], p, &outCol);
            NormalColorDpq(&modelCube1.n[ modelCube1_index[t+2] ], &modelCube1.c[v+2], p, &outCol1);
            NormalColorDpq(&modelCube1.n[ modelCube1_index[t+1] ], &modelCube1.c[v+1], p, &outCol2);
            setRGB0(poly1, outCol.r, outCol.g  , outCol.b);
            setRGB1(poly1, outCol1.r, outCol1.g, outCol1.b);
            setRGB2(poly1, outCol2.r, outCol2.g, outCol2.b);
            OTz /= 3;
            if ((OTz > 0) && (OTz < OTLEN))
                AddPrim(&ot[db][OTz-2], poly1);
            nextpri += sizeof(POLY_G3);
            t+=3;
        }
        // See l.216
        RotMatrix_gte(&lgtang, &rotlgt);
        RotMatrix_gte(&Rotate1, &rotcube);  
        MulMatrix0(&rotcube, &rotlgt, &rotlgt);
        MulMatrix0(&lgtmat, &rotlgt, &light);
        SetLightMatrix(&light);
        // See l.227
        RotMatrix(&Rotate1, &Matrix1);
        ScaleMatrix(&Matrix1, &Scale1);
        TransMatrix(&Matrix1, &Trans1);
        SetRotMatrix(&Matrix1);
        SetTransMatrix(&Matrix1);
        FntPrint(0, "Hello lightsources !\n %d", i);
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