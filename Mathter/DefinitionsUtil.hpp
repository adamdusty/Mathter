#pragma once

// This file contains
// - class pre-declarations
// - class property helpers
// - general helpers

#include <cstdint>
#include <type_traits>


namespace mathter {


// Vector
template <class T, int Dim, bool Packed>
class Vector;


// Swizzle
template <class T, int... Indices>
class Swizzle;


// Matrix
/// <summary> Determines if you want to left- or right-multiply your matrices with vectors. </summary>
/// <remarks>
/// <para> This flag affects the generated transformation matrices. If you want to write M2*M1*v in your code,
/// then choose PRECEDE_VECTOR, if you want v*M1*M2, choose FOLLOW_VECTOR. Matrices generated by Transform, Scale,
/// Rotation and similar functions will match your order of multiplication. (I.e. bottom row is translation if you
/// choose FOLLOW_VECTOR). </para>
/// <para> You can still use M*v and v*M in your code. </para>
/// </remarks>
enum class eMatrixOrder {
	PRECEDE_VECTOR,
	FOLLOW_VECTOR,
};

/// <summary> Determines the memory layout of matrices. </summary>
/// <remarks>
/// <para> For ROW_MAJOR layout, the matrix's first row comes first in memory, followed immediately by
/// the second row's elements. For COLUMN_MAJOR matrices, the memory region begins with the first column. </para>
/// <para> This does not affect arithmetic or matrix generator function in any way. Your arithmetic will work
/// the same way if you change this. </para>
/// <para> Please note that changing this flag may affect performance of arithmetic operations. Profile your
/// code to determine optimal settings. Performance may depend on multiple factors. </para>
/// </remarks>
enum class eMatrixLayout {
	ROW_MAJOR,
	COLUMN_MAJOR,
};

template <class T, int Rows, int Columns, eMatrixOrder Order, eMatrixLayout Layout, bool Packed>
class Matrix;

template <class MatrixT, int SRows, int SColumns>
class Submatrix;


// Quaternion
template <class T, bool Packed>
class Quaternion;


// Dynamically sized things
/// <summary> Specify this as Vector or Matrix dimension template parameter to set size at runtime.
/// PLEASE NOTE THAT DYNAMICALLY SIZED VECTORS AND MATRICES ARE NOT SUPPORTED YET. </summary>
constexpr int DYNAMIC = -1;





namespace impl {

	// Vector properties
	template <class VectorT>
	class VectorPropertiesHelper {};

	template <class T_, int Dim_, bool Packed_>
	class VectorPropertiesHelper<Vector<T_, Dim_, Packed_>> {
	public:
		using Type = T_;
		static constexpr int Dim = Dim_;
		static constexpr bool Packed = Packed_;
	};

	template <class VectorT>
	class VectorProperties : public VectorPropertiesHelper<typename std::decay<VectorT>::type> {};


	// Matrix properties
	template <class MatrixT>
	class MatrixPropertiesHelper {};

	template <class T_, int Rows_, int Columns_, eMatrixOrder Order_, eMatrixLayout Layout_, bool Packed_>
	class MatrixPropertiesHelper<Matrix<T_, Rows_, Columns_, Order_, Layout_, Packed_>> {
	public:
		using Type = T_;
		static constexpr int Rows = Rows_;
		static constexpr int Columns = Columns_;
		static constexpr eMatrixOrder Order = Order_;
		static constexpr eMatrixLayout Layout = Layout_;
		static constexpr bool Packed = Packed_;
	};

	template <class MatrixT>
	class MatrixProperties : public MatrixPropertiesHelper<typename std::decay<MatrixT>::type> {};


	template <eMatrixLayout Layout>
	class OppositeLayout {
	public:
		static constexpr eMatrixLayout value = (Layout == eMatrixLayout::ROW_MAJOR ? eMatrixLayout::COLUMN_MAJOR : eMatrixLayout::ROW_MAJOR);
	};


	// Common utility
	template <class T, class U>
	using MatMulElemT = decltype(T() * U() + T() + U());

	template <class T>
	T sign(T arg) {
		return T(arg > T(0)) - (arg < T(0));
	}

	template <class T>
	T sign_nonzero(T arg) {
		return T(arg >= T(0)) - (arg < T(0));
	}
	


	// Template metaprogramming utilities
	template <template <class> class Cond, class... T>
	struct All;

	template <template <class> class Cond, class Head, class... Rest>
	struct All<Cond, Head, Rest...> {
		static constexpr bool value = Cond<Head>::value && All<Cond, Rest...>::value;
	};

	template <template <class> class Cond>
	struct All<Cond> {
		static constexpr bool value = true;
	};


	template <template <class> class Cond, class... T>
	struct Any;

	template <template <class> class Cond, class Head, class... Rest>
	struct Any<Cond, Head, Rest...> {
		static constexpr bool value = Cond<Head>::value || Any<Cond, Rest...>::value;
	};

	template <template <class> class Cond>
	struct Any<Cond> {
		static constexpr bool value = false;
	};



	template <class... T>
	struct TypeList {};

	template <class Tl1, class Tl2>
	struct ConcatTypeList;

	template <class... T, class... U>
	struct ConcatTypeList<TypeList<T...>, TypeList<U...>> {
		using type = TypeList<T..., U...>;
	};

	template <class T, int N>
	struct RepeatType {
		using type = typename std::conditional<N <= 0, TypeList<>, typename ConcatTypeList<TypeList<T>, typename RepeatType<T, N - 1>::type>::type>::type;
	};


	// Decide if type is Scalar, Vector or Matrix.
	template <class Arg>
	struct IsVector {
		static constexpr bool value = false;
	};
	template <class T, int Dim, bool Packed>
	struct IsVector<Vector<T, Dim, Packed>> {
		static constexpr bool value = true;
	};
	template <class Arg>
	struct NotVector {
		static constexpr bool value = !IsVector<Arg>::value;
	};

	template <class Arg>
	struct IsSwizzle {
		static constexpr bool value = false;
	};
	template <class T, int... Indices>
	struct IsSwizzle<Swizzle<T, Indices...>> {
		static constexpr bool value = true;
	};
	template <class Arg>
	struct NotSwizzle {
		static constexpr bool value = !IsSwizzle<Arg>::value;
	};

	template <class Arg>
	struct IsVectorOrSwizzle {
		static constexpr bool value = IsVector<Arg>::value || IsSwizzle<Arg>::value;
	};

	template <class T>
	struct IsMatrix {
		static constexpr bool value = false;
	};
	template <class T, int Rows, int Columns, eMatrixOrder Order, eMatrixLayout Layout, bool Packed>
	struct IsMatrix<Matrix<T, Rows, Columns, Order, Layout, Packed>> {
		static constexpr bool value = true;
	};
	template <class T>
	struct NotMatrix {
		static constexpr bool value = !IsMatrix<T>::value;
	};

	template <class T>
	struct IsSubmatrix {
		static constexpr bool value = false;
	};
	template <class M, int Rows, int Columns>
	struct IsSubmatrix<Submatrix<M, Rows, Columns>> {
		static constexpr bool value = true;
	};
	template <class T>
	struct NotSubmatrix {
		static constexpr bool value = !IsSubmatrix<T>::value;
	};

	template <class Arg>
	struct IsQuaternion {
		static constexpr bool value = false;
	};
	template <class T, bool Packed>
	struct IsQuaternion<Quaternion<T, Packed>> {
		static constexpr bool value = true;
	};
	template <class Arg>
	struct NotQuaternion {
		static constexpr bool value = !IsQuaternion<Arg>::value;
	};


	template <class T>
	struct IsScalar {
		static constexpr bool value = !IsMatrix<T>::value && !IsVector<T>::value && !IsSwizzle<T>::value && !IsQuaternion<T>::value && !IsSubmatrix<T>::value;
	};

	// Dimension of an argument (add dynamically sized vectors later).
	template <class U, int Along = 0>
	struct DimensionOf {
		static constexpr int value = 1;
	};
	template <class T, int Dim, bool Packed>
	struct DimensionOf<Vector<T, Dim, Packed>, 0> {
		static constexpr int value = Dim;
	};
	template <class T, int... Indices>
	struct DimensionOf<Swizzle<T, Indices...>> {
		static constexpr int value = sizeof...(Indices);
	};

	// Sum dimensions of arguments.
	template <class... Rest>
	struct SumDimensions;

	template <class Head, class... Rest>
	struct SumDimensions<Head, Rest...> {
		static constexpr int value = DimensionOf<Head>::value > 0 ? DimensionOf<Head>::value + SumDimensions<Rest...>::value : DYNAMIC;
	};

	template <>
	struct SumDimensions<> {
		static constexpr int value = 0;
	};


	// Specialization for floats.
	template <class T>
	bool AlmostEqual(T d1, T d2, std::true_type) {
		if (d1 < 1e-38 && d2 < 1e-38) {
			return true;
		}
		if ((d1 == 0 && d2 < 1e-4) || (d2 == 0 && d1 < 1e-4)) {
			return true;
		}
		T scaler = pow(T(10), floor(std::log10(std::abs(d1))));
		d1 /= scaler;
		d2 /= scaler;
		d1 *= T(1000.0);
		d2 *= T(1000.0);
		return round(d1) == round(d2);
	}

	// Specialization for int, complex and custom types: simple equality.
	template <class T>
	bool AlmostEqual(T d1, T d2, std::false_type) {
		return d1 == d2;
	}

	// Check equivalence with tolerance.
	template <class T>
	bool AlmostEqual(T d1, T d2) {
		return AlmostEqual(d1, d2, std::integral_constant<bool, std::is_floating_point<T>::value>());
	}


	// Check if base class's pointer equals that of derived when static_casted.
	// This is supposedly not standard C++, so it's only enabled for compiler that like it.
	template <class Base, class Derived>
	class BasePtrEquals {
#if _MSC_VER > 1910
		static Derived instance;
		static constexpr void* base = static_cast<void*>(static_cast<Base*>(&instance));
	public:
		static constexpr bool value = base == static_cast<void*>(&instance);
#else
	public:
		static constexpr bool value = true;
#endif
	};

#if _MSC_VER > 1910
	template <class Base, class Derived>
	Derived BasePtrEquals<Base, Derived>::instance;
#endif
}
}