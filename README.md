Mathter
===

What is this?
---
Yet another math library, as if there wasn't enough of them already. [Eigen](http://eigen.tuxfamily.org/), [CML](http://cmldev.net/) and [mathfu](https://github.com/google/mathfu) are all great ones (check them out!), but still, I found annoyances in all of them from a game developer's point of view. This library is focused on 3D and 2D game development, and tries to provide an API that is as little pain in the ass as possible.

Features
---
- Header-only: just include Mathter/*
- Configureable
..- Float, double, int, or roll your own (int, std::complex, and custom not yet verified)
..- Dimensions as template parameter
..- Post- and pre-multiplication style for geometrical transforms
..- Row-major and column-major layout
..- Tightly pack elements (i.e. no SIMD alignment and padding)
- Fast: SIMD accelerated, beats the above-mentioned in small (<=4) matrix multiplication
- Vectors, Matrices, associated arithmetic and basic matrix functions
- Affine transforms: scale, rotation, translation
- Planes and lines, intersection

**To come (in that order, presumably):**
- LU decomposition, SVD, determinant, inverse
- Projective transforms
- Quaternions
- Std::complex and user types

Examples
---
Include stuff:
```c++
#include <Mathter/Vector.hpp> // vectors
#include <Mathter/Matrix.hpp> // matrices
#include <Mathter/Geometry.hpp> // lines, planes, intersection
using namespace mathter;
```

Vectors have three parameters:
- scalar type (T)
- dimension (Dim)
- whether to pack tightly (true) or use SIMD (false, default)
```c++
Vector<float, 3, false> u; // SIMD accelerated 3-dimensional vector
Vector<float, 3, false> v(1,2,3);
```

You can take advantage of automatic concatenation:
```c++
Vector<float, 6> u6(7,8,9, u); // 7,8,9,4,5,6
Vector<float, 6> v6 = u | v; // 4,5,6,1,2,3
Vector<double, 6> w6 = { u,v }; // 4,5,6,1,2,3
```
Be careful, different scalar types are implicitly cast. You might want to disable compiler warnings for this.

Matrices have many parameters:
- scalar type (T)
- number of columns, or **width** (Columns)
- number of rows, or **height** (Rows)
- geometrical transformation matrices written after (row-vector)(default) or before (column-vector) the vector
- row-major (default) or column-major storage
- enable tight packing (for uploading to GPU or serialization)
```c++
Matrix<float, 3, 4, eMatrixOrder::FOLLOW_VECTOR, eMatrixLayout::ROW_MAJOR, false> M =
{
	1,	2,	3,
	4,	5,	6,
	7,	8,	9,
	10,	11,	12,
};
```
Elements layed out as on paper. This is not affected by memory layout or order of multiplication.

Let's transform a vector:
```c++
M.SetTranslation(10, 10, 10);
Vector<float, 3> v_transformed1 = (v | 1)*M;
```
Notice we had to append a one to the vector to apply the translation. Since this is not necesserily efficient and nice, I might add implicit extension to vector-matrix multiplication.
The other way around:
```c++
Matrix<float, 4, 3, eMatrixOrder::PRECEDE_VECTOR> M_T = M.Transposed();
Vector<float, 3> v_transformed2 = M_T*(v | 1);
```



















