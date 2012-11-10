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

#ifndef CV_INLINE
#if defined __cplusplus
    #define CV_INLINE inline
#elif (defined WIN32 || defined _WIN32 || defined WINCE) && !defined __GNUC__
    #define CV_INLINE __inline
#else
    #define CV_INLINE static
#endif
#endif /* CV_INLINE */

typedef struct cv_point
{
    int x;
    int y;
} cv_point;

CV_INLINE cv_point _cv_point(int x, int y)
{
    cv_point p;
    p.x=x; p.y=y;
    return p;
}

typedef struct cv_scalar
{
    float val[4];
} cv_scalar;

CV_INLINE cv_scalar _cv_scalar(float val0, float val1, float val2, float val3)
{
    cv_scalar s;
    s.val[0]=val0; s.val[1]=val1; s.val[2]=val2; s.val[3]=val3;
    return s;
}

CV_INLINE cv_scalar _cv_scalar_all(float val0)
{
    cv_scalar s;
    s.val[0]=s.val[1]=s.val[2]=s.val[3]=val0;
    return s;
}

#define CV_RGB( r, g, b )  _cv_scalar( (b), (g), (r), 0 )

#endif /* #ifndef TINYCVTYPES_H */
