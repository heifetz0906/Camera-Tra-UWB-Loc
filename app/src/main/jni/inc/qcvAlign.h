#ifndef qcv_Align_h__
#define qcv_Align_h__

#include <cassert>

namespace qcv
{

namespace details
{
    template<typename T> struct is_pointer      { static const bool value = false; };
    template<typename T> struct is_pointer<T*>  { static const bool value = true;  };
}

// align pointer to certain byte boundary
struct Aligner
{
    // make sure align is >= 1.
    static size_t       normalizeAlignment(size_t alignment) { return alignment==0? 1 : alignment; }

    // align the number to next closest number of multiples of alignment.
    static size_t       align(size_t x, size_t alignment) { return alignNormalized(x, normalizeAlignment(alignment)); }

    // align the pointer to next closest number (in bytes) of multiples of alignment.
    // e.g., int* pa = align<int*>(p, 4); // p is any type of pointer.
    // e.g., const int* pa = align<const int*>(p, 4); // p is any type of pointer.
    template<typename T, typename T1> static T align(T1* p, size_t alignment) { static_assert(details::is_pointer<T>::value, "Must align to a pointer type"); 
            return numberToPointer<T>( align(pointerToNumber(p), alignment) ); }

    // [head, paddings, data]. head needs no alignment. data must be aligned. return the size big enough to hold the head and data even for worst head address.
    static size_t       getSafeAllocSize(size_t headSize, size_t dataSize, size_t dataAlign) { return headSize + normalizeAlignment(dataAlign)-1 + dataSize; }

    // nominal type change
    template<typename T> static size_t  pointerToNumber(T* p) { return reinterpret_cast<size_t>(p); }
    template<typename T> static T       numberToPointer(size_t p) { static_assert(details::is_pointer<T>::value, "Must convert to a pointer type");  return reinterpret_cast<T>(p); }

private:
    // alignment must be normalized!
    static size_t       alignNormalized(size_t x, size_t alignment) { assert(alignment>0); return ( x + alignment - 1 ) / alignment * alignment; }

    // align the pointer to next closest address of multiples of alignment.
    //static void*        alignPointer(void* p, size_t alignment) { return numberToPointer( align(pointerToNumber(p), alignment) ); }
    //static const void*  alignPointer(const void* p, size_t alignment) { return numberToPointer( align(pointerToNumber(p), alignment) ); }
    //static size_t       pointerToNumber(void* p) { return reinterpret_cast<size_t>(p); }
    //static size_t       pointerToNumber(const void* p) { return reinterpret_cast<size_t>(p); }
};


}   // namespace qcv

#endif //qcv_Align_h__
