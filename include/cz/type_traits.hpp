namespace cz {

///////////////////////////////////////////////////////////////////////////////

template <class T>
struct Remove_CV_Struct {
    typedef T type;
};
template <class T>
struct Remove_CV_Struct<const T> {
    typedef T type;
};
template <class T>
struct Remove_CV_Struct<volatile T> {
    typedef T type;
};
template <class T>
struct Remove_CV_Struct<const volatile T> {
    typedef T type;
};

template <class T>
using Remove_CV = typename Remove_CV_Struct<T>::type;

///////////////////////////////////////////////////////////////////////////////

}
