/* 
 * This is a modified version of the High Performance Computing Linpack
 * Benchmark (HPL). All code not contained in the original HPL version
 * 2.0 is property of the Frankfurt Institute for Advanced Studies
 * (FIAS). None of the material may be copied, reproduced, distributed,
 * republished, downloaded, displayed, posted or transmitted in any form
 * or by any means, including, but not limited to, electronic,
 * mechanical, photocopying, recording, or otherwise, without the prior
 * written permission of FIAS. For those parts contained in the
 * unmodified version of the HPL the below copyright notice applies.
 * 
 * Authors:
 * David Rohr (drohr@jwdt.org)
 * Matthias Bach (bach@compeng.uni-frankfurt.de)
 * Matthias Kretz (kretz@compeng.uni-frankfurt.de)
 * 
 * -- High Performance Computing Linpack Benchmark (HPL)                
 *    HPL - 2.0 - September 10, 2008                          
 *    Antoine P. Petitet                                                
 *    University of Tennessee, Knoxville                                
 *    Innovative Computing Laboratory                                 
 *    (C) Copyright 2000-2008 All Rights Reserved                       
 *                                                                      
 * -- Copyright notice and Licensing terms:                             
 *                                                                      
 * Redistribution  and  use in  source and binary forms, with or without
 * modification, are  permitted provided  that the following  conditions
 * are met:                                                             
 *                                                                      
 * 1. Redistributions  of  source  code  must retain the above copyright
 * notice, this list of conditions and the following disclaimer.        
 *                                                                      
 * 2. Redistributions in binary form must reproduce  the above copyright
 * notice, this list of conditions,  and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. 
 *                                                                      
 * 3. All  advertising  materials  mentioning  features  or  use of this
 * software must display the following acknowledgement:                 
 * This  product  includes  software  developed  at  the  University  of
 * Tennessee, Knoxville, Innovative Computing Laboratory.             
 *                                                                      
 * 4. The name of the  University,  the name of the  Laboratory,  or the
 * names  of  its  contributors  may  not  be used to endorse or promote
 * products  derived   from   this  software  without  specific  written
 * permission.                                                          
 *                                                                      
 * -- Disclaimer:                                                       
 *                                                                      
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR  CONTRIBUTORS  BE  LIABLE FOR ANY  DIRECT,  INDIRECT,  INCIDENTAL,
 * SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES  (INCLUDING,  BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 * ---------------------------------------------------------------------
 */ 
/*
 * Include files
 */
#include "hpl.h"

#include "util_timer.h"
#include "util_trace.h"

#ifndef HPL_dger

void HPL_dger
(
   const enum HPL_ORDER             ORDER,
   const int                        M,
   const int                        N,
   const double                     ALPHA,
   const double *                   X,
   const int                        INCX,
   double *                         Y,
   const int                        INCY,
   double *                         A,
   const int                        LDA
)
{
/* 
 * Purpose
 * =======
 *
 * HPL_dger performs the rank 1 operation
 *  
 *     A := alpha * x * y^T + A,
 *  
 * where alpha is a scalar,  x is an m-element vector, y is an n-element
 * vector and A is an m by n matrix.
 *
 * Arguments
 * =========
 *
 * ORDER   (local input)                 const enum HPL_ORDER
 *         On entry, ORDER  specifies the storage format of the operands
 *         as follows:                                                  
 *            ORDER = HplRowMajor,                                      
 *            ORDER = HplColumnMajor.                                   
 *
 * M       (local input)                 const int
 *         On entry,  M  specifies  the number of rows of  the matrix A.
 *         M must be at least zero.
 *
 * N       (local input)                 const int
 *         On entry, N  specifies the number of columns of the matrix A.
 *         N must be at least zero.
 *
 * ALPHA   (local input)                 const double
 *         On entry, ALPHA specifies the scalar alpha.   When  ALPHA  is
 *         supplied as zero then  X and Y  need not be set on input.
 *
 * X       (local input)                 const double *
 *         On entry,  X  is an incremented array of dimension  at  least
 *         ( 1 + ( m - 1 ) * abs( INCX ) )  that  contains the vector x.
 *
 * INCX    (local input)                 const int
 *         On entry, INCX specifies the increment for the elements of X.
 *         INCX must not be zero.
 *
 * Y       (local input)                 double *
 *         On entry,  Y  is an incremented array of dimension  at  least
 *         ( 1 + ( n - 1 ) * abs( INCY ) )  that  contains the vector y.
 *
 * INCY    (local input)                 const int
 *         On entry, INCY specifies the increment for the elements of Y.
 *         INCY must not be zero.
 *
 * A       (local input/output)          double *
 *         On entry,  A  points  to an array of size equal to or greater
 *         than LDA * n.  Before  entry, the leading m by n part  of the
 *         array  A  must contain the matrix coefficients. On exit, A is
 *         overwritten by the updated matrix.
 *
 * LDA     (local input)                 const int
 *         On entry,  LDA  specifies  the  leading  dimension  of  A  as
 *         declared  in  the  calling  (sub) program.  LDA  must  be  at
 *         least MAX(1,m).
 *
 * ---------------------------------------------------------------------
 */ 
START_TRACE( DGER )

#ifdef HPL_CALL_CBLAS
   cblas_dger( ORDER, M, N, ALPHA, X, INCX, Y, INCY, A, LDA );
#endif
#ifdef HPL_CALL_FBLAS
   double                    alpha = ALPHA;
#ifdef HPL_USE_F77_INTEGER_DEF
   const F77_INTEGER         F77M    = M,   F77N    = N,
                             F77lda  = LDA, F77incx = INCX, F77incy = INCY;
#else
#define F77M                 M
#define F77N                 N
#define F77lda               LDA
#define F77incx              INCX
#define F77incy              INCY
#endif

   if( ORDER == HplColumnMajor )
   {  F77dger( &F77M, &F77N, &alpha, X, &F77incx, Y, &F77incy, A, &F77lda ); }
   else
   {  F77dger( &F77N, &F77M, &alpha, Y, &F77incy, X, &F77incx, A, &F77lda ); }
#endif

END_TRACE
/*
 * End of HPL_dger
 */
}

#endif
