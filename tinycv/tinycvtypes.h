/*
 * TinyCV
 */

#ifndef TINYCVTYPES_H
#define TINYCVTYPES_H

#ifndef CV_PI
# define CV_PI 3.1415926535897932384626433832795
#endif

#ifndef DEG_TO_RAD
# define DEG_TO_RAD(x) ( (x) * (float)CV_PI/180.0f)
#endif

#ifndef RAD_TO_DEG
# define RAD_TO_DEG(x) ( (x) * 180.0f/(float)CV_PI)
#endif

typedef struct cv_point
{
    int x;
    int y;
} cv_point;

typedef struct cv_scalar
{
    float val[4];
} cv_scalar;

#endif /* #ifndef TINYCVTYPES_H */
