#include <3d.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <graphics.h>

void main()

/* 3D Transforms Demonstration Program.  Requires Turbo C Version 1.5.
   Project file should contain the line

                       DEMO3D.C GRAPHICS.LIB 3D.LIB

   The library directory must include 3D.LIB and GRAPHICS.LIB, and the
   include directory must include 3D.H.  The default directory must contain
   the driver for your graphics hardware.

   The program displays a cube, a tetrahedron, and an octahedron in
   three dimensions.  Each figure is rotated about a different axis. */

{
    FACE *f,*g,*h,*i,*j,*k;                /* Faces of cube */
    FACE *t1,*t2,*t3,*t4;                  /* Faces of tetrahedron */
    FACE *o1,*o2,*o3,*o4,*o5,*o6,*o7,*o8;  /* Faces of octahedron */
    VECTOR n,s;
    MATRIX xm,ym,xr,yr,om,or,id;           /* Transformation matrices */
    OBJECT *o;                             /* Cube */
    OBJECT *p;                             /* Tetrahedron */
    OBJECT *q;                             /* Octahedron */
    int g_driver,g_mode;
    int apage,vpage,tpage,frame;

    /* Initialize matrices and data structures */

    identity (xm);
    identity (ym);
    identity (xr);
    identity (yr);
    identity (id);
    identity (om);
    identity (or);

    f = (FACE *)malloc(sizeof(FACE));
    g = (FACE *)malloc(sizeof(FACE));
    h = (FACE *)malloc(sizeof(FACE));
    i = (FACE *)malloc(sizeof(FACE));
    j = (FACE *)malloc(sizeof(FACE));
    k = (FACE *)malloc(sizeof(FACE));

    t1 = (FACE *)malloc(sizeof(FACE));
    t2 = (FACE *)malloc(sizeof(FACE));
    t3 = (FACE *)malloc(sizeof(FACE));
    t4 = (FACE *)malloc(sizeof(FACE));

    o1 = (FACE *)malloc(sizeof(FACE));
    o2 = (FACE *)malloc(sizeof(FACE));
    o3 = (FACE *)malloc(sizeof(FACE));
    o4 = (FACE *)malloc(sizeof(FACE));
    o5 = (FACE *)malloc(sizeof(FACE));
    o6 = (FACE *)malloc(sizeof(FACE));
    o7 = (FACE *)malloc(sizeof(FACE));
    o8 = (FACE *)malloc(sizeof(FACE));

    o = (OBJECT *)malloc(sizeof(OBJECT));
    new_obj (o);

    p = (OBJECT *)malloc(sizeof(OBJECT));
    new_obj (p);

    q = (OBJECT *)malloc(sizeof(OBJECT));
    new_obj (q);

    /* Define cube */

    new_face (f);
    add_corner (   0.0,   0.0,   0.0,f);
    add_corner ( 100.0,   0.0,   0.0,f);
    add_corner ( 100.0, 100.0,   0.0,f);
    add_corner (   0.0, 100.0,   0.0,f);
    add_face (o,f);

    new_face (g);
    add_corner (   0.0,   0.0,   0.0,g);
    add_corner (   0.0, 100.0,   0.0,g);
    add_corner (   0.0, 100.0, 100.0,g);
    add_corner (   0.0,   0.0, 100.0,g);
    add_face (o,g);

    new_face (h);
    add_corner (   0.0,   0.0,   0.0,h);
    add_corner (   0.0,   0.0, 100.0,h);
    add_corner ( 100.0,   0.0, 100.0,h);
    add_corner ( 100.0,   0.0,   0.0,h);
    add_face (o,h);

    new_face (i);
    add_corner (   0.0,   0.0, 100.0,i);
    add_corner (   0.0, 100.0, 100.0,i);
    add_corner ( 100.0, 100.0, 100.0,i);
    add_corner ( 100.0,   0.0, 100.0,i);
    add_face (o,i);

    new_face (j);
    add_corner (   0.0, 100.0,   0.0,j);
    add_corner ( 100.0, 100.0,   0.0,j);
    add_corner ( 100.0, 100.0, 100.0,j);
    add_corner (   0.0, 100.0, 100.0,j);
    add_face (o,j);

    new_face (k);
    add_corner ( 100.0,   0.0,   0.0,k);
    add_corner ( 100.0,   0.0, 100.0,k);
    add_corner ( 100.0, 100.0, 100.0,k);
    add_corner ( 100.0, 100.0,   0.0,k);
    add_face (o,k);

    /* Define tetrahedron */

    new_face (t1);
    add_corner (   0.00,   0.00,   0.00,t1);
    add_corner ( 100.00,   0.00,   0.00,t1);
    add_corner (  50.00,  81.65,  28.87,t1);
    add_face (p,t1);

    new_face (t2);
    add_corner (   0.00,   0.00,   0.00,t2);
    add_corner (  50.00,   0.00,  86.60,t2);
    add_corner ( 100.00,   0.00,   0.00,t2);
    add_face (p,t2);

    new_face (t3);
    add_corner (   0.00,   0.00,   0.00,t3);
    add_corner (  50.00,  81.65,  28.87,t3);
    add_corner (  50.00,   0.00,  86.60,t3);
    add_face (p,t3);

    new_face (t4);
    add_corner ( 100.00,   0.00,   0.00,t4);
    add_corner (  50.00,   0.00,  86.60,t4);
    add_corner (  50.00,  81.65,  28.87,t4);
    add_face (p,t4);

    /* Define octahedron */

    new_face (o1);
    add_corner (   0.00,  70.71,   0.00,o1);
    add_corner (  50.00,   0.00,  50.00,o1);
    add_corner ( 100.00,  70.71,   0.00,o1);
    add_face (q,o1);

    new_face (o2);
    add_corner ( 100.00,  70.71,   0.00,o2);
    add_corner (  50.00,   0.00,  50.00,o2);
    add_corner ( 100.00,  70.71, 100.00,o2);
    add_face (q,o2);

    new_face (o3);
    add_corner ( 100.00,  70.71, 100.00,o3);
    add_corner (  50.00,   0.00,  50.00,o3);
    add_corner (   0.00,  70.71, 100.00,o3);
    add_face (q,o3);

    new_face (o4);
    add_corner (   0.00,  70.71, 100.00,o4);
    add_corner (  50.00,   0.00,  50.00,o4);
    add_corner (   0.00,  70.71,   0.00,o4);
    add_face (q,o4);

    new_face (o5);
    add_corner (   0.00,  70.71,   0.00,o5);
    add_corner (  50.00, 141.40,  50.00,o5);
    add_corner (   0.00,  70.71, 100.00,o5);
    add_face (q,o5);

    new_face (o6);
    add_corner (   0.00,  70.71, 100.00,o6);
    add_corner (  50.00, 141.40,  50.00,o6);
    add_corner ( 100.00,  70.71, 100.00,o6);
    add_face (q,o6);

    new_face (o7);
    add_corner ( 100.00,  70.71, 100.00,o7);
    add_corner (  50.00, 141.40,  50.00,o7);
    add_corner ( 100.00,  70.71,   0.00,o7);
    add_face (q,o7);

    new_face (o8);
    add_corner ( 100.00,  70.71,   0.00,o8);
    add_corner (  50.00, 141.40,  50.00,o8);
    add_corner (   0.00,  70.71,   0.00,o8);
    add_face (q,o8);

    /* The light source in the z direction (from the eye) */

    s[0] =  0.25;
    s[1] =  0.0;
    s[2] =  1.0;

    /* Center figures in space and orient them */

    /* The following series of function calls illustrates the concatenation
       of 3D transforms.  Each of three matrices xm, ym, and om is the
       concatenation of four transforms; first, center the object on the
       origin, second, rotate the object about the y axis, third, rotate the
       object about the x axis, and fourth, translate the object to its final
       position.  All four transforms are concatenated in each matrix, then
       the object is transformed.  Thus, each vertex in the object is mul-
       tiplied by the transformation matrix only once.    */

    trans (-50,-50,-50,xm);
    trans (-50,-50,-50,ym);
    trans (-50,-50,-50,om);
    yrot (M_PI/4,xm);
    yrot (M_PI/3,ym);
    yrot (M_PI/4,om);
    xrot (M_PI/4,xm);
    xrot (M_PI/3,ym);
    xrot (M_PI/4,om);

    /* Position figures in space */

    trans (200,200,200,xm);
    trans (400,150,200,ym);
    trans (500,250,200,om);
    xform (*o,xm);

    xform (*p,ym);
    xform (*q,om);

    /* Set up rotation matrices */

    /* These matrices are created to perform an incremental rotation of each
       object.  First, the object is translated to the origin, second, the
       object is rotated, and third, the object is translated back to its
       original postion. */

    trans (-200,-200,-200,xr);
	yrot (M_PI/45,xr);
	trans (200,200,200,xr);

    trans (-400,-150,-200,yr);
    xrot (M_PI/45,yr);
    trans (400,150,200,yr);

    trans (-500,-250,-200,or);
    xrot (M_PI/45,or);
    yrot (M_PI/45,or);
    trans (500,250,200,or);

	apage = 0;
	vpage = 1;

    registerbgidriver(CGA_driver);
    registerbgidriver(EGAVGA_driver);
    detectgraph (&g_driver,&g_mode);
    initgraph (&g_driver,&g_mode,"");

	for (frame = 1; frame <= 45; frame++)
	{
        setvisualpage(vpage);
		setactivepage(apage);
		tpage = apage;
		apage = vpage;
		vpage = tpage;
		cleardevice();
        xform (*o,xr);
        xform (*p,yr);
        xform (*q,or);
        disp_object (s,1,o,id);
        disp_object (s,4,p,id);
        disp_object (s,2,q,id);
    }
}