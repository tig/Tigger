/*  3-D graphics library data structures and prototypes.
    These functions are based on information in
    "Principles of Interactive Computer Graphics", by
    Newman and Sproull, Second Edition, McGraw-Hill,
    Publishers.
*/

#define    CMAX    4    /* maximum column (4 x 4 matrix) */
#define    RMAX    4    /* maximum row    (4 x 4 matrix) */
#define    DIM     3    /* number of dimensions */

typedef double MATRIX [RMAX] [CMAX];

/* A MATRIX is used to describe a transformation from one coordinate
system to another.  Multiple transformations may be concatenated by
multiplying their transformation matrices. */

typedef double VECTOR [DIM];

/* A VECTOR is an array of three coordinates [x,y,z] representing magnitude
and direction.  It is typically used to represent the direction of a
light source, or the normal of a plane. */

typedef struct vertex {
    double coord [DIM];
    struct vertex *next;
}VERTEX;

typedef struct corner {
    VERTEX *this;
    struct corner *next;
}CORNER;

typedef struct face {
    CORNER *first;
    struct face *next;
}FACE;

/* A FACE is a plane figure consisting of a linked list of corners.
The corners in the list are assumed to be in clockwise order as viewed
from the outside of an object. */

typedef struct object {
    FACE *faces;
    VERTEX *vertices;
} OBJECT;

/* An OBJECT is a solid consisting of faces and vertices.  The topology
is completely described by the list of faces; the geometry is described
by the faces and vertices. */

/* Initialization functions */

void    identity (MATRIX this_mat);
int     new_face (FACE *this_face);
int     new_obj (OBJECT *this_obj);

/* Vector and matrix math functions */

double  dot_prod (VECTOR vec1, VECTOR vec2);
void    mat_mul (MATRIX mat1, MATRIX mat2, MATRIX prod);
int     normal (FACE *this_face, VECTOR norm);
void    vec_mul (VERTEX *this_vec, MATRIX this_mat, VERTEX *prod);

/* Transformation functions */

void    scale (double sx, double sy, double sz, MATRIX this_mat);
void    trans (double tx, double ty, double tz, MATRIX this_mat);
void    xrot (double theta, MATRIX this_mat);
void    yrot (double theta, MATRIX this_mat);
void    zrot (double theta, MATRIX this_mat);
void    persp (double s, double d, double f, MATRIX this_mat);

/* Put figures on the screen */

void    disp_face (VECTOR lsource, int color, FACE *this_face, MATRIX xfrm_mat);
void    disp_object (VECTOR lsource, int color, OBJECT *this_obj, MATRIX xfrm_mat);

/* Manipulate data structures */

int     add_corner (double x, double y, double z, FACE *this_face);
int     xform (OBJECT this_obj, MATRIX transform);
int     del_face (OBJECT *this_obj, FACE *this_face);
double  max_z (FACE this_face);
double  min_z (FACE this_face);
int     add_face (OBJECT *this_obj, FACE *this_face);

/* Dump data structures to screen */

void    dump_mat (MATRIX this_mat);
void    dump_vec (VERTEX this_vec);
void    dump_face (FACE this_face);
void    dump_obj (OBJECT this_obj);