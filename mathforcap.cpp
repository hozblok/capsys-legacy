#include "mathforcap.h"

MathForCap::MathForCap(QObject *parent) :
    QObject(parent)

{
    QSettings settings;
    accuracy = settings.value("CurrentSettings/accurancy",404.404).toDouble(); //2e-13
    rayGoesUp = settings.value("CurrentSettings/rayGoesUp",1).toBool();
    r_0SPECTRUM = 1e-6;
    RSPECTRUM = 1;
    LSPECTRUM = 4e3;
//    qDebug() << "accuracy:" << accuracy << "rayGoesUp:" << rayGoesUp;
//    qDebug() << "diffCharXi" << diffCharXi[0] << diffCharXi[1] << diffCharXi[2] << diffCharXi[3] << diffCharXi[4] << diffCharXi[5];
}

/*
////////////////////////////////////////////////////////////////////////////////
//  void Unit_Lower_Triangular_Solve(double *L, double *B, double x[], int n) //
//                                                                            //
//  Description:                                                              //
//     This routine solves the linear equation Lx = B, where L is an n x n    //
//     unit lower triangular matrix.  (Only the subdiagonal part of the matrix//
//     is addressed.)  The diagonal is assumed to consist of 1's and is not   //
//     addressed.                                                             //
//     The algorithm follows:                                                 //
//                          x[0] = B[0], and                                  //
//            x[i] = B[i] - (L[i][0] * x[0]  + ... + L[i][i-1] * x[i-1]),     //
//     for i = 1, ..., n-1.                                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *L   Pointer to the first element of the unit lower triangular  //
//                 matrix.                                                    //
//     double *B   Pointer to the column vector, (n x 1) matrix, B.           //
//     double *x   Pointer to the column vector, (n x 1) matrix, x.           //
//     int     n   The number of rows or columns of the matrix L.             //
//                                                                            //
//  Return Values:                                                            //
//     void                                                                   //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N], B[N], x[N];                                            //
//                                                                            //
//     (your code to create matrix A and column vector B)                     //
//     Unit_Lower_Triangular_Solve(&A[0][0], B, x, n);                        //
//     printf(" The solution is \n");                                         //
//           ...                                                              //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
void MathForCap::Unit_Lower_Triangular_Solve(double *L, double B[], double x[], int n)
{
   int i, k;
//         Solve the linear equation Lx = B for x, where L is a unit lower
//         triangular matrix.
   x[0] = B[0];
   for (k = 1, L += n; k < n; L += n, k++)
      for (i = 0, x[k] = B[k]; i < k; i++) x[k] -= x[i] * *(L + i);
}
/*
////////////////////////////////////////////////////////////////////////////////
// File: upper_triangular.c                                                   //
// Routines:                                                                  //
//    Upper_Triangular_Solve                                                  //
//    Upper_Triangular_Inverse                                                //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  int Upper_Triangular_Solve(double *U, double *B, double x[], int n)       //
//                                                                            //
//  Description:                                                              //
//     This routine solves the linear equation Ux = B, where U is an n x n    //
//     upper triangular matrix.  (The subdiagonal part of the matrix is       //
//     not addressed.)                                                        //
//     The algorithm follows:                                                 //
//                  x[n-1] = B[n-1]/U[n-1][n-1], and                          //
//     x[i] = [B[i] - (U[i][i+1] * x[i+1]  + ... + U[i][n-1] * x[n-1])]       //
//                                                                 / U[i][i], //
//     for i = n-2, ..., 0.                                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *U   Pointer to the first element of the upper triangular       //
//                 matrix.                                                    //
//     double *B   Pointer to the column vector, (n x 1) matrix, B.           //
//     double *x   Pointer to the column vector, (n x 1) matrix, x.           //
//     int     n   The number of rows or columns of the matrix U.             //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix U is singular.                                 //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N], B[N], x[N];                                            //
//                                                                            //
//     (your code to create matrix A and column vector B)                     //
//     err = Upper_Triangular_Solve(&A[0][0], B, x, n);                       //
//     if (err < 0) printf(" Matrix A is singular\n");                        //
//     else printf(" The solution is \n");                                    //
//           ...                                                              //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
int MathForCap::Upper_Triangular_Solve(double *U, double B[], double x[], int n)
{
   int i, k;
//         Solve the linear equation Ux = B for x, where U is an upper
//         triangular matrix.
   for (k = n-1, U += n * (n - 1); k >= 0; U -= n, k--) {
      if (*(U + k) == 0.0) return -1;           // The matrix U is singular
      x[k] = B[k];
      for (i = k + 1; i < n; i++) x[k] -= x[i] * *(U + i);
      x[k] /= *(U + k);
   }
   return 0;
}
/*
////////////////////////////////////////////////////////////////////////////////
// File: doolittle_pivot.c                                                    //
// Routines:                                                                  //
//    Doolittle_LU_Decomposition_with_Pivoting                                //
//    Doolittle_LU_with_Pivoting_Solve                                        //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  int Doolittle_LU_Decomposition_with_Pivoting(double *A, int pivot[],      //
//                                                                    int n)  //
//                                                                            //
//  Description:                                                              //
//     This routine uses Doolittle's method to decompose the n x n matrix A   //
//     into a unit lower triangular matrix L and an upper triangular matrix U //
//     such that A = LU.                                                      //
//     The matrices L and U replace the matrix A so that the original matrix  //
//     A is destroyed.  Note!  In Doolittle's method the diagonal elements of //
//     L are 1 and are not stored.                                            //
//     The LU decomposition is convenient when one needs to solve the linear  //
//     equation Ax = B for the vector x while the matrix A is fixed and the   //
//     vector B is varied.  The routine for solving the linear system Ax = B  //
//     after performing the LU decomposition for A is                         //
//                      Doolittle_LU_with_Pivoting_Solve.                     //
//                                                                            //
//     The Doolittle method with partial pivoting is:  Determine the pivot    //
//     row and interchange the current row with the pivot row, then assuming  //
//     that row k is the current row, k = 0, ..., n - 1 evaluate in order the //
//     following pair of expressions                                          //
//       U[k][j] = A[k][j] - (L[k][0]*U[0][j] + ... + L[k][k-1]*U[k-1][j])    //
//                                 for j = k, k+1, ... , n-1                  //
//       L[i][k] = (A[i][k] - (L[i][0]*U[0][k] + . + L[i][k-1]*U[k-1][k]))    //
//                          / U[k][k]                                         //
//                                 for i = k+1, ... , n-1.                    //
//       The matrix U forms the upper triangular matrix, and the matrix L     //
//       forms the lower triangular matrix.                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *A       Pointer to the first element of the matrix A[n][n].    //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     int     n       The number of rows or columns of the matrix A.         //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N];                                                        //
//     int    pivot[N];                                                       //
//                                                                            //
//     (your code to create matrix A)                                         //
//     err = Doolittle_LU_Decomposition_with_Pivoting(&A[0][0], pivot, N);    //
//     if (err < 0) printf(" Matrix A is singular\n");                        //
//     else { printf(" The LU decomposition of A is \n");                     //
//           ...                                                              //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
int MathForCap::Doolittle_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n)
{
   int i, j, k, p;
   double *p_k, *p_row, *p_col;
   double max;
//         For each row and column, k = 0, ..., n-1,
   for (k = 0, p_k = A; k < n; p_k += n, k++) {
//            find the pivot row
      pivot[k] = k;
      max = fabs( *(p_k + k) );
      for (j = k + 1, p_row = p_k + n; j < n; j++, p_row += n) {
         if ( max < fabs(*(p_row + k)) ) {
            max = fabs(*(p_row + k));
            pivot[k] = j;
            p_col = p_row;
         }
      }
//     and if the pivot row differs from the current row, then
//     interchange the two rows.
      if (pivot[k] != k)
         for (j = 0; j < n; j++) {
            max = *(p_k + j);
            *(p_k + j) = *(p_col + j);
            *(p_col + j) = max;
         }
//                and if the matrix is singular, return error
      if ( *(p_k + k) == 0.0 ) return -1;
//      otherwise find the lower triangular matrix elements for column k.
      for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++) {
         *(p_row + k) /= *(p_k + k);
      }
//            update remaining matrix
      for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++)
         for (j = k+1; j < n; j++)
            *(p_row + j) -= *(p_row + k) * *(p_k + j);
   }
   return 0;
}
/*
////////////////////////////////////////////////////////////////////////////////
//  int Doolittle_LU_with_Pivoting_Solve(double *LU, double *B, int pivot[],  //
//                                       double *x,  int n)                   //
//                                                                            //
//  Description:                                                              //
//     This routine uses Doolittle's method to solve the linear equation      //
//     Ax = B.  This routine is called after the matrix A has been decomposed //
//     into a product of a unit lower triangular matrix L and an upper        //
//     triangular matrix U with pivoting.  The argument LU is a pointer to the//
//     matrix the subdiagonal part of which is L and the superdiagonal        //
//     together with the diagonal part is U. (The diagonal part of L is 1 and //
//     is not stored.)   The matrix A = LU.                                   //
//     The solution proceeds by solving the linear equation Ly = B for y and  //
//     subsequently solving the linear equation Ux = y for x.                 //
//                                                                            //
//  Arguments:                                                                //
//     double *LU      Pointer to the first element of the matrix whose       //
//                     elements form the lower and upper triangular matrix    //
//                     factors of A.                                          //
//     double *B       Pointer to the column vector, (n x 1) matrix, B.       //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     double *x       Solution to the equation Ax = B.                       //
//     int     n       The number of rows or columns of the matrix LU.        //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N], B[N], x[N];                                            //
//     int    pivot[N];                                                       //
//                                                                            //
//     (your code to create matrix A and column vector B)                     //
//     err = Doolittle_LU_Decomposition_with_Pivoting(&A[0][0], pivot,  N);   //
//     if (err < 0) printf(" Matrix A is singular\n");                        //
//     else {                                                                 //
//        err = Doolittle_LU_with_Pivoting_Solve(&A[0][0], B, pivot, x, N);   //
//        if (err < 0) printf(" Matrix A is singular\n");                     //
//        else printf(" The solution is \n");                                 //
//           ...                                                              //
//     }                                                                      //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
int MathForCap::Doolittle_LU_with_Pivoting_Solve(double *A, double B[], int pivot[],
                                                              double x[], int n)
{
   int i, k;
   double *p_k;
   double dum;
//         Solve the linear equation Lx = B for x, where L is a lower
//         triangular matrix with an implied 1 along the diagonal.
   for (k = 0, p_k = A; k < n; p_k += n, k++) {
      if (pivot[k] != k) {dum = B[k]; B[k] = B[pivot[k]]; B[pivot[k]] = dum; }
      x[k] = B[k];
      for (i = 0; i < k; i++) x[k] -= x[i] * *(p_k + i);
   }
//         Solve the linear equation Ux = y, where y is the solution
//         obtained above of Lx = B and U is an upper triangular matrix.
   for (k = n-1, p_k = A + n*(n-1); k >= 0; k--, p_k -= n) {
      if (pivot[k] != k) {dum = B[k]; B[k] = B[pivot[k]]; B[pivot[k]] = dum; }
      for (i = k + 1; i < n; i++) x[k] -= x[i] * *(p_k + i);
      if (*(p_k + k) == 0.0) return -1;
      x[k] /= *(p_k + k);
   }
   return 0;
}
/*
////////////////////////////////////////////////////////////////////////////////
//  int Doolittle_LU_Decomposition(double *A, int n)                          //
//                                                                            //
//  Description:                                                              //
//     This routine uses Doolittle's method to decompose the n x n matrix A   //
//     into a unit lower triangular matrix L and an upper triangular matrix U //
//     such that A = LU.                                                      //
//     The matrices L and U replace the matrix A so that the original matrix  //
//     A is destroyed.                                                        //
//     Note!  In Doolittle's method the diagonal elements of L are 1 and are  //
//            not stored.                                                     //
//     Note!  The determinant of A is the product of the diagonal elements    //
//            of U.  (det A = det L * det U = det U).                         //
//     This routine is suitable for those classes of matrices which when      //
//     performing Gaussian elimination do not need to undergo partial         //
//     pivoting, e.g. positive definite symmetric matrices, diagonally        //
//     dominant band matrices, etc.                                           //
//     For the more general case in which partial pivoting is needed use      //
//                  Doolittle_LU_Decomposition_with_Pivoting.                 //
//     The LU decomposition is convenient when one needs to solve the linear  //
//     equation Ax = B for the vector x while the matrix A is fixed and the   //
//     vector B is varied.  The routine for solving the linear system Ax = B  //
//     after performing the LU decomposition for A is Doolittle_LU_Solve      //
//     (see below).                                                           //
//                                                                            //
//     The Doolittle method is given by evaluating, in order, the following   //
//     pair of expressions for k = 0, ... , n-1:                              //
//       U[k][j] = A[k][j] - (L[k][0]*U[0][j] + ... + L[k][k-1]*U[k-1][j])    //
//                                 for j = k, k+1, ... , n-1                  //
//       L[i][k] = (A[i][k] - (L[i][0]*U[0][k] + . + L[i][k-1]*U[k-1][k]))    //
//                          / U[k][k]                                         //
//                                 for i = k+1, ... , n-1.                    //
//       The matrix U forms the upper triangular matrix, and the matrix L     //
//       forms the lower triangular matrix.                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *A   Pointer to the first element of the matrix A[n][n].        //
//     int     n   The number of rows or columns of the matrix A.             //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N];                                                        //
//                                                                            //
//     (your code to intialize the matrix A)                                  //
//                                                                            //
//     err = Doolittle_LU_Decomposition(&A[0][0], N);                         //
//     if (err < 0) printf(" Matrix A is singular\n");                        //
//     else { printf(" The LU decomposition of A is \n");                     //
//           ...                                                              //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
int MathForCap::Doolittle_LU_Decomposition(double *A, int n)
{
   int i, j, k, p;
   double *p_k, *p_row, *p_col;
//         For each row and column, k = 0, ..., n-1,
//            find the upper triangular matrix elements for row k
//            and if the matrix is non-singular (nonzero diagonal element).
//            find the lower triangular matrix elements for column k.
   for (k = 0, p_k = A; k < n; p_k += n, k++) {
      for (j = k; j < n; j++) {
         for (p = 0, p_col = A; p < k; p_col += n,  p++)
            *(p_k + j) -= *(p_k + p) * *(p_col + j);
      }
      if ( *(p_k + k) == 0.0 ) return -1;
      for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++) {
         for (p = 0, p_col = A; p < k; p_col += n, p++)
            *(p_row + k) -= *(p_row + p) * *(p_col + k);
         *(p_row + k) /= *(p_k + k);
      }
   }
   return 0;
}
/*
////////////////////////////////////////////////////////////////////////////////
//  int Doolittle_LU_Solve(double *LU, double *B, double *x,  int n)           //
//                                                                            //
//  Description:                                                              //
//     This routine uses Doolittle's method to solve the linear equation      //
//     Ax = B.  This routine is called after the matrix A has been decomposed //
//     into a product of a unit lower triangular matrix L and an upper        //
//     triangular matrix U without pivoting.  The argument LU is a pointer to //
//     the matrix the subdiagonal part of which is L and the superdiagonal    //
//     together with the diagonal part is U. (The diagonal part of L is 1 and //
//     is not stored.)   The matrix A = LU.                                   //
//     The solution proceeds by solving the linear equation Ly = B for y and  //
//     subsequently solving the linear equation Ux = y for x.                 //
//                                                                            //
//  Arguments:                                                                //
//     double *LU  Pointer to the first element of the matrix whose elements  //
//                 form the lower and upper triangular matrix factors of A.   //
//     double *B   Pointer to the column vector, (n x 1) matrix, B            //
//     double *x   Solution to the equation Ax = B.                           //
//     int     n   The number of rows or columns of the matrix LU.            //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
//  Example:                                                                  //
//     #define N                                                              //
//     double A[N][N], B[N], x[N];                                            //
//                                                                            //
//     (your code to create matrix A and column vector B)                     //
//     err = Doolittle_LU_Decomposition(&A[0][0], N);                         //
//     if (err < 0) printf(" Matrix A is singular\n");                        //
//     else {                                                                 //
//        err = Doolittle_LU_Solve(&A[0][0], B, x, n);                        //
//        if (err < 0) printf(" Matrix A is singular\n");                     //
//        else printf(" The solution is \n");                                 //
//           ...                                                              //
//     }                                                                      //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
*/
int MathForCap::Doolittle_LU_Solve(double *LU, double B[], double x[], int n)
{
//         Solve the linear equation Lx = B for x, where L is a lower
//         triangular matrix with an implied 1 along the diagonal.
   Unit_Lower_Triangular_Solve(LU, B, x, n);
//         Solve the linear equation Ux = y, where y is the solution
//         obtained above of Lx = B and U is an upper triangular matrix.
   return Upper_Triangular_Solve(LU, x, x, n);
}

//DELET ME::
double MathForCap::rectangle_integrate(mglExpr & funRe, mglExpr & funIm, double tau,/* double border_, double border__, */long int & n, double & delay)
//                           ,std::complex<double> (*f1)(double)
//                           ,std::complex<double> (*f2)(double)
//                           ,std::complex<double> (*f3)(double))
{
    std::complex<double> result = std::complex<double> (0.0,0.0);
//    double h = (border__ - border_)/n; //Шаг сетки
    double h = (tau)/n; //Шаг сетки
    double border_ = -tau/2.0;
    double argument = 0.0;

    for(int i = 0; i < n; ++i)
    {
        argument = border_ + h*(i + 0.5);
        if (std::fabs(argument+delay) <= tau/2.0)
        {
            result += std::complex<double> (funRe.Eval(argument+delay),funIm.Eval(argument+delay))
                    * std::complex<double> (funRe.Eval(argument),(-1)*funIm.Eval(argument)); //Вычисляем в средней точке и добавляем в сумму
        }
        //       qDebug() << "!" << result.real() << result.imag();
    }
    result *= h;

    return result.real();
}
//:: DELETE ME

double MathForCap::FunTor(double x, double y, double z)
{
    return (
                std::pow((std::pow((x-RSPECTRUM),2)+std::pow(y,2)+std::pow(z,2)+std::pow(RSPECTRUM,2)-std::pow(r_0SPECTRUM,2)),2)
            - 4*std::pow(RSPECTRUM,2)*(std::pow((x-RSPECTRUM),2)+pow(z,2))
            );
}

double MathForCap::FunTorDX(double x, double y, double z)
{
    return (
                4*(x-RSPECTRUM)*(std::pow((x-RSPECTRUM),2)+std::pow(y,2)+std::pow(z,2)+std::pow(RSPECTRUM,2)-std::pow(r_0SPECTRUM,2))
            - 8*std::pow(RSPECTRUM,2)*(x-RSPECTRUM)
            );
}

double MathForCap::FunTorDY(double x, double y, double z)
{
    return (
                4*(y)*(std::pow((x-RSPECTRUM),2)+std::pow(y,2)+std::pow(z,2)+std::pow(RSPECTRUM,2)-std::pow(r_0SPECTRUM,2))
            );
}

double MathForCap::FunTorDZ(double x, double y, double z)
{
    return (
                4*(z)*(std::pow((x-RSPECTRUM),2)+std::pow(y,2)+std::pow(z,2)+std::pow(RSPECTRUM,2)-std::pow(r_0SPECTRUM,2))
            - 8*std::pow(RSPECTRUM,2)*(z)
            );
}

inline int MathForCap::Sign(double Val1,double Val2) const
{
//  if ((Val1!=Val1)||(Val2!=Val2)) return -2;
  if (Val1 == 0.)  return 1;
  if (Val2 == 0.)  return 2;
  if (((Val1>0.)&&(Val2<0.))
      ||((Val1<0.)&&(Val2>0.)))  return 0;
  else return -1;
}

int MathForCap::NewtonMetodPath(QByteArray &FUN,
                                 const double *prevPoint, double *rPoint,
                                double * directingVectorLine)
{
    long int i,j;
    long int k = 0;
    mglExpr mglf0(FUN.data());
    int temp = 0;
    double tempNorm = 0;

    QVector<double> p, pointR, middle;
    QVector<double>::iterator itp, itm;
    QVector<double>::const_iterator itpr;
    p.resize(numOfVar);
    pointR.resize(numOfVar);
    middle.resize(numOfVar);

//    double * itrPoint = (double *)rPoint;

    double * itdvl = (double *)directingVectorLine;
    double * itdvlend = itdvl + numOfVar;

    for( ; itdvl!=itdvlend; ++itdvl)
    {
        tempNorm += std::pow(*itdvl,2);
    }
    tempNorm = std::sqrt(tempNorm);
//    qDebug() << "NMP: tempNorm" << tempNorm << "dvl:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2] <<"mglf0.Eval(rPoint[0],rPoint[1],rPoint[2])" << mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]);
    if(!flagDEBUGf0ONNSPECTRUM)
        tempNorm *= mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]);
    else
        tempNorm *= FunTor(rPoint[0],rPoint[1],rPoint[2]);

    for(itp = p.begin(), itdvl = (double *)directingVectorLine
        ; itp != p.end()
        ; ++itp, ++itdvl
        )
    {
        *itp= (*itdvl)/tempNorm;
    }
//    qDebug() << "NMP: new tempNorm:" << tempNorm << "new p:" << p;

    while(true)
    {
        ++k;
//        qDebug() << "Test 2:" << k <<  p <<tempNorm << "dvl:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2]
//        << "rPoint:" << rPoint[0] << rPoint[1] << rPoint[2]
//                 << "pointR:" << pointR;
        for(i = 0; i < numOfVar; ++i)
        {
            pointR[i] = rPoint[i] + (k)*p[i];
        }

        if(!flagDEBUGf0ONNSPECTRUM)
            temp = Sign(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]),mglf0.Eval(pointR[0],pointR[1],pointR[2]));
        else
            temp = Sign(FunTor(rPoint[0],rPoint[1],rPoint[2]),FunTor(pointR[0],pointR[1],pointR[2]));

        if(temp == 2)
        {
            for( j = 0; j < numOfVar; ++j)
            {
                rPoint[j] = pointR[j];
            }
            return 0;
        }
        if(temp == 0)
        {
            break;
        }
        for( i = 0; i < numOfVar; ++i)
        {
            pointR[i] = rPoint[i] - (k)*p[i];
        }
        if(!flagDEBUGf0ONNSPECTRUM)
            temp = Sign(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]),mglf0.Eval(pointR[0],pointR[1],pointR[2]));
        else
            temp = Sign(FunTor(rPoint[0],rPoint[1],rPoint[2]),FunTor(pointR[0],pointR[1],pointR[2]));
        if(temp == 2)
        {
            for( j = 0; j < numOfVar; ++j)
            {
                rPoint[j] = pointR[j];
            }
            return 0;
        }
        if(temp == 0)
        {
            break;
        }
        if(k>1000)
        {
            qDebug() << "### Error NMP: Stage 2 crashed!";
            return -2;
        }
    }
    //создали промежуток интервала

    middle.fill(0.,3);
    for(itp = p.begin()
        ; itp != p.end()
        ; ++itp )
    {
        (*itp) *= k;
    }

    for( j=0;
         (temp !=1)&&(temp !=2)
         && (std::fabs(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]))>accuracy)
         && (std::fabs(mglf0.Eval(pointR[0],pointR[1],pointR[2]))>accuracy)
          ;++j )
    {
        for(i = 0; i < numOfVar; ++i)
        {
            p[i] /= 2.;
            middle[i] = rPoint[i] + p[i];
        }
        qDebug() << "NMP: iter:"<< j << "rPoint:" << rPoint[0] << rPoint[1] << rPoint[2]
                 << "middle" << middle
                 << "pointR:" << pointR;
        if(!flagDEBUGf0ONNSPECTRUM)
            temp = Sign(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]),mglf0.Eval(middle[0],middle[1],middle[2]));
        else
            temp = Sign(FunTor(rPoint[0],rPoint[1],rPoint[2]),FunTor(middle[0],middle[1],middle[2]));

        if(temp == 1)
        {
            break;
        }
        if(temp == 2)
        {
            for(i = 0; i < numOfVar; ++i)
                rPoint[i] = middle[i];
        }
        if(temp == 0)
        {
            for(i = 0; i < numOfVar; ++i)
               pointR[i] = middle[i];
        }
        if(temp == -1)
        {
            for(i = 0; i < numOfVar; ++i)
                rPoint[i] = middle[i];
        }
        if(!flagDEBUGf0ONNSPECTRUM)
        {
            qDebug() << "NMP: iter:"<< j << "rPoint:" << rPoint[0] << rPoint[1] << rPoint[2] << "middle" << middle << "pointR:" << pointR
                     << "f0(pointR):" << mglf0.Eval(pointR[0],pointR[1],pointR[2]) << "f0(rPoint):"<< mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]);
        }
        else
        {
            qDebug() << "NMP: iter:"<< j << "rPoint:" << rPoint[0] << rPoint[1] << rPoint[2] << "middle" << middle << "pointR:" << pointR
                     << "f0(pointR):" << FunTor(pointR[0],pointR[1],pointR[2]) << "f0(rPoint):" << FunTor(rPoint[0],rPoint[1],rPoint[2]);
        }
        if(j > 3000)
        {
            qDebug() << "### Error NMP: Stage 2-end crashed!";
            return -2;
        }
    }
    double * itrPoint = (double *)rPoint;
    if( (std::fabs(mglf0.Eval(pointR[0],pointR[1],pointR[2]))<accuracy) )
    {
        for(itpr= pointR.constBegin()
            ; itpr!= pointR.constEnd()
            ; ++itrPoint, ++itpr)
        {
            (*itrPoint)= *itpr;
        }
    }
    qDebug() << "NMP: total Point: "<<rPoint[0] << rPoint[1] << rPoint[2];//DELETEME
    return 0;
}

int MathForCap::NewtonMetodForEquationAndLine(const QByteArray &f0,const QByteArray &FUN,
                                   const double * prevPoint, double  * rPoint,
                                   double * directingVectorLine
                                              )
{
    long int i = 0,j = 0;

//    qDebug() << "Begin NMFEAL:" << rPoint[0] << rPoint[1] << rPoint[2];

    double tempNormDVL = std::sqrt( std::pow(directingVectorLine[0],2)
                                   +std::pow(directingVectorLine[1],2)
                                   +std::pow(directingVectorLine[2],2) );
    for (i = 0; i < numOfVar; ++i)
        directingVectorLine[i] /= tempNormDVL;


    double jacobi[numOfEq][numOfVar]; //матрица a: a*x=b
    double bVector[numOfEq]; //вектор правых частей
    double solVector[numOfVar]; //вектор решения
//    qDebug() << "Test!";
    mglExpr mglf0Modified(FUN.data());
//    mglExpr mglf0(f0.data());
    /// начало метода ньютона

    for (i = 0; ((std::fabs(mglf0Modified.Eval(rPoint[0],rPoint[1],rPoint[2]))+
                 std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
                 std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2])) ) > accuracy)
                || (i==0)
         ; ++i)
    {
        for (j = 0; j < numOfVar; ++j)
        {
            jacobi[0][j]= mglf0Modified.Diff(diffCharXi[j],rPoint[0],rPoint[1],rPoint[2]);
            //перейти к normalVector? Никак нельзя, rPoint меняется, normalVector - пред. точки не меняется
        }
        jacobi[1][0]= directingVectorLine[1];
        jacobi[1][1]= -directingVectorLine[0];
        jacobi[1][2]= 0.0;
        jacobi[2][0]= 0.0;
        jacobi[2][1]= directingVectorLine[2];
        jacobi[2][2]= -directingVectorLine[1];

        bVector[0]= - mglf0Modified.Eval(rPoint[0],rPoint[1],rPoint[2]);
        bVector[1]= - directingVectorLine[1]*(rPoint[0]-prevPoint[0]) + directingVectorLine[0]*(rPoint[1]-prevPoint[1]);
        bVector[2]= - directingVectorLine[2]*(rPoint[1]-prevPoint[1]) + directingVectorLine[1]*(rPoint[2]-prevPoint[2]);
//        bVector[1]= 0 - mglf1.Calc(rVector[0],rVector[1],rVector[2]); //как вариант
//        bVector[2]= 0 - mglf2.Calc(rVector[0],rVector[1],rVector[2]);

//        qDebug() << "Matrica:";
//        qDebug() << jacobi[0][0] << jacobi[0][1] << jacobi[0][2];
//        qDebug() << jacobi[1][0] << jacobi[1][1] << jacobi[1][2];
//        qDebug() << jacobi[2][0] << jacobi[2][1] << jacobi[2][2];

        if (Doolittle_LU_Decomposition(&jacobi[0][0],numOfVar) < 0)
        {
            double at = mglf0Modified.Diff(diffCharXi[0],1.0,1.0,1.0);
            double bt = mglf0Modified.Diff(diffCharXi[1],1.0,1.0,1.0);
            double ct = mglf0Modified.Diff(diffCharXi[2],1.0,1.0,1.0);
            double dt = mglf0Modified.Eval(0,0,0);
            if( (at == mglf0Modified.Diff(diffCharXi[0],-2.0,-2.0,-2.0)) &&
                 (bt == mglf0Modified.Diff(diffCharXi[1],-2.0,-2.0,-2.0)) &&
                  (ct == mglf0Modified.Diff(diffCharXi[2],-2.0,-2.0,-2.0))
                 )
            {
                rPoint[1] = (
                            at*prevPoint[1] - (directingVectorLine[1]/directingVectorLine[0])
                            * ( at*prevPoint[0] + (dt + ct*(prevPoint[2] - (directingVectorLine[2]/directingVectorLine[1])*prevPoint[1])) )
                            ) /
                        ( at +
                          1/(directingVectorLine[0]) *
                          (bt*directingVectorLine[1] + ct*directingVectorLine[2])
                          );
                rPoint[0] = prevPoint[0] + (directingVectorLine[0]/directingVectorLine[1]) * (rPoint[1] - prevPoint[1]);
                rPoint[2] = prevPoint[2] + (directingVectorLine[2]/directingVectorLine[0]) * (rPoint[0] - prevPoint[0]);
//                qDebug() << "rPoint:" << rPoint[0] << rPoint[1] << rPoint[2];
                if( (rPoint[0]==rPoint[0]) &&  (rPoint[1]==rPoint[1]) &&  (rPoint[2]==rPoint[2]) )
                    return 0;
            }
            qDebug() << "### ERROR LU_Decomposition and DirectMetod";
//          Метод Гаусса- Зейделя, впрочем, тоже можно здесь использовать, но...
//            всё одно
//            tempStage2 = -2;
//            break;
            return -1; //ОСТОРОЖНО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        }
        else
        {
//            qDebug() << "Begin 2" << rPoint[0] << rPoint[1] << rPoint[2];

            if (Doolittle_LU_Solve(&jacobi[0][0],bVector,solVector,numOfVar) < 0)
            {
                qDebug() << "### ERROR LU_Solve";
                return -1;
            }
            if((solVector[0]==solVector[0]) &&  (solVector[1]==solVector[1]) &&  (solVector[2]==solVector[2]))
            {
//                qDebug() << solVector[0] << solVector[1] << solVector[2]
//                            << (solVector[0]==solVector[0]) << (solVector[1]==solVector[1]) << (solVector[2]==solVector[2]);
                for (j = 0; j < numOfVar; ++j)
                    rPoint[j] += solVector[j];
            }
            else
            {
//                qDebug() << "isNan -> New approximation";
                rPoint[0] = RandomForCap::GetRandomExcExc(-100.,100.);
                rPoint[1] = RandomForCap::GetRandomExcExc(-100.,100.);
                rPoint[2] = RandomForCap::GetRandomExcExc(-10000.,10000.);
            }
//            qDebug() << "Begin 3" << rPoint[0] << rPoint[1] << rPoint[2];
        }
//        qDebug() << "#N iteracii:" << i << ":" << rPoint[0] << rPoint[1] << rPoint[2] << "accurancy" <<
//                        (fabs(mglf0Modified.Eval(rPoint[0],rPoint[1],rPoint[2]))+
//                         fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//                         fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2])) );
        if ( i > 2000 )
        {
//            tempStage2 = -2;
//            break;
            return -2; //ОСТОРОЖНО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            /*
//            if(std::fabs(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]))+
//               std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//               std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2])) >2.)
//            qDebug() << "Stage 2: rPoint: " << rPoint[0] << rPoint[1] << rPoint[2]
//                     << "dvl:" << directingVectorLine[0] << directingVectorLine[1] << directingVectorLine[2];
            //            if(flagf0ONNSPECTRUM)
//            try {
////                    qDebug() << "test:" << mglf0Modified.Eval(rPoint[0],rPoint[1],rPoint[2]);
////                    qDebug() << "rPoint" << rPoint[0] << rPoint[1] << rPoint[2];
////                    qDebug() << "!!!!!!!!!!!!" <<fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
////                                fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
//                     = NewtonMetodPath(f0, prevPoint, rPoint, directingVectorLine);
////                    qDebug() << "!!Yes," << tempp;
//                    if (tempp != -2)
//                        return 0;
//                    else
//                        return -2;
//                    } catch (...)
//                {
//                    return -2;
//                }*/
        }
//        if(tempStage2 = -2)
//        {
//            break;
//        }
    }
    /// конец метода ньютона
//    achievedAccuracy= std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//            std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
//            + std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))
//            + std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
//    if(achievedAccuracy > 0.) //ИТАК ТОЧНОСТЬ НА НАПРАВЛЯЮЩИЙ ВЕКТОР ОГРОМАДНАЯ, УЛУЧШИТЬ ЕЁ ТАКИМ СПОСОБОМ НЕ ПОЛУЧИТСЯ
//    {
//        qDebug() << "@@@ Stage 1:"
//        << "achieved accuracy:" << achievedAccuracy;
//        double tempNormShift;
//        for(int iii; iii< numOfVar; ++iii)
//            tempNormShift += std::pow(rPoint[iii]-prevPoint[iii],2);
//        tempNormDVL /= tempNormShift;
//        double rrPoint[numOfVar];
//        for(int iii; iii< numOfVar; ++iii)
//            rrPoint[iii] = rPoint[iii] + directingVectorLine[iii]/tempNormDVL;
//        double temps = std::fabs(mglf0.Eval(rrPoint[0],rrPoint[1],rrPoint[2]));
//        qDebug() << "tempsQQQQQQQQQQQQQQQQQQQ" << temps << std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//                    std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
//        if(temps <= 1e-4)
//        {
//            rPoint[0] = rrPoint[0];
//            rPoint[1] = rrPoint[1];
//            rPoint[2] = rrPoint[2];
//        }

//        qDebug() << "Stage 1 DONE!";
//    }



//    int tempStage2 = 0; //проверочный флаг для второй стадии
    /// вот эта вот часть может быть полезна для изогнутого капилляра:
//    double achievedAccuracy = std::fabs(mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]));
//    if((achievedAccuracy > accuracy)&&(achievedAccuracy <= 1e-4))
//    {
////        qDebug() << "@@@ Stage 2:";
////        qDebug() << "achieved accuracy:" << achievedAccuracy;
////        tempStage2 = NewtonMetodPath(f0, prevPoint, rPoint, directingVectorLine);
////        if(tempStage2 < 0)
////        {
////            qDebug() << "##### ERROR 2: Please, Decrease the accuracy!";
////            return -2;
////        }
//        if(achievedAccuracy >= 1e-6)
//        {
//            qDebug() << "##### ERROR 2!";
//            return -2;
//        }
//    }
    /// <-- вот эта вот часть может быть полезна для изогнутого капилляра



//    achievedAccuracy = std::fabs(directingVectorLine[1]*(rPoint[0]-prevPoint[0]) - directingVectorLine[0]*(rPoint[1]-prevPoint[1]))+
//                std::fabs(directingVectorLine[2]*(rPoint[1]-prevPoint[1]) - directingVectorLine[1]*(rPoint[2]-prevPoint[2]));
//    if(achievedAccuracy > 0.)
//    {
//        qDebug() << "@@@ Stage 3:"
//        << "ERROR.......... achieved accuracy:" << achievedAccuracy;
////        tempStage2 = NewtonMetodPath(f0, prevPoint, rPoint, directingVectorLine);
////        if(tempStage2 < 0)
////        {
////            qDebug() << "##### ERROR 3: Please, Decrease the accuracy!";
////            return -2;
////        }

//    } // не получится, и так точность нормальная!!!

//    qDebug() << "END NM:" << rPoint[0] << rPoint[1] << rPoint[2] << "f0(rPoint):" <<mglf0.Eval(rPoint[0],rPoint[1],rPoint[2]);
    return 0;
}
