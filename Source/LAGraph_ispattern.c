//------------------------------------------------------------------------------
// LAGraph_ispattern: check if a matrix is all 1
//------------------------------------------------------------------------------

// LAGraph, (TODO list all authors here) (c) 2019, All Rights Reserved.
// http://graphblas.org  See LAGraph/Doc/License.txt for license.

//------------------------------------------------------------------------------

// LAGraph_ispattern: check if a matrix values are all equal to 1.

#include "LAGraph_internal.h"

#define LAGRAPH_FREE_ALL    \
{                           \
    GrB_free (&C) ;         \
    GrB_free (&monoid) ;    \
}

GrB_Info LAGraph_ispattern  // return GrB_SUCCESS if successful
(
    bool *result,           // true if A is all one, false otherwise
    GrB_Matrix A,
    GrB_UnaryOp userop      // for A with user-defined type
)
{

    GrB_Monoid monoid = NULL ;
    GrB_Type type ;
    GrB_Index nrows, ncols ;

    // check inputs
    if (result == NULL)
    {
        // error: required parameter, result, is NULL
        return (GrB_NULL_POINTER) ;
    }
    (*result) = false ;

    // get the type and size of A
    LAGRAPH_OK (GxB_Matrix_type  (&type,  A)) ;
    LAGRAPH_OK (GrB_Matrix_nrows (&nrows, A)) ;
    LAGRAPH_OK (GrB_Matrix_ncols (&ncols, A)) ;

    #ifdef GxB_SUITESPARSE_GRAPHBLAS
    // SuiteSparse has a predefined boolean AND monoid, but this is optional
    monoid = GxB_LAND_BOOL_MONOID ;
    #else
    // this works just fine without SuiteSparse
    LAGRAPH_OK (GrB_Monoid_new_BOOL (&monoid, GrB_LAND, true)) ;
    #endif

    if (type == GrB_BOOL)
    {
        // result = and (A)
        LAGRAPH_OK (GrB_reduce (result, NULL, monoid, A, NULL)) ;
    }
    else
    {

        // select the unary operator
        GrB_UnaryOp op ;
        if      (type == GrB_INT8  ) op = LAGraph_ISONE_INT8 ;
        else if (type == GrB_INT16 ) op = LAGraph_ISONE_INT16 ;
        else if (type == GrB_INT32 ) op = LAGraph_ISONE_INT32 ;
        else if (type == GrB_INT64 ) op = LAGraph_ISONE_INT64 ;
        else if (type == GrB_UINT8 ) op = LAGraph_ISONE_UINT8 ;
        else if (type == GrB_UINT16) op = LAGraph_ISONE_UINT16 ;
        else if (type == GrB_UINT32) op = LAGraph_ISONE_UINT32 ;
        else if (type == GrB_UINT64) op = LAGraph_ISONE_UINT64 ;
        else if (type == GrB_FP32  ) op = LAGraph_ISONE_FP32   ;
        else if (type == GrB_FP64  ) op = LAGraph_ISONE_FP64   ;
        else                         op = userop ;

        // C = isone (A)
        LAGRAPH_OK (GrB_Matrix_new (&C, GrB_BOOL, nrows, ncols)) ;
        LAGRAPH_OK (GrB_apply (C, NULL, NULL, op, A, NULL)) ;

        // result = and (C)
        LAGRAPH_OK (GrB_reduce (result, NULL, monoid, C, NULL)) ;
    }

    // free workspace and return result
    GrB_free (&C) ;
    GrB_free (&monoid) ;
    return (GrB_SUCCESS) ;
}
