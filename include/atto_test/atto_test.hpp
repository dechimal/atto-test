#if !defined ATTO_TEST_INCLUDED_
#define      ATTO_TEST_INCLUDED_

#include <string>
#include <type_traits>
#include <functional>
#include "boost/fusion/include/is_sequence.hpp"
#include "boost/fusion/include/fold.hpp"
#include "boost/preprocessor/cat.hpp"

#define ATTO_TEST(name) ATTO_TEST_I(name, BOOST_PP_CAT(atto_test_test, name))
#define ATTO_TEST_I(name, type) \
    struct type { \
        static int init() { \
            atto_test::add_test(test, ATTO_TEST_TO_STR(name)); \
            return 0; \
        } \
        static int x; \
        static void test(); \
    }; \
    int type::x = type::init(); \
    void type::test()

#define ATTO_TEST_PARAM(name, ...) ATTO_TEST_PARAM_I(name, BOOST_PP_CAT(atto_test_testcase, name), ATTO_TEST_GET_COUNTER(__LINE__)::value, name(__VA_ARGS__))
#define ATTO_TEST_PARAM_I(name, type, counter, call) \
    template<std::size_t, std::size_t> struct type; \
    template<> struct type<__LINE__, counter> { \
        static int init() { \
            atto_test::add_test(test, ATTO_TEST_TO_STR(call)); \
            return 0; \
        } \
        static int x; \
        static void test(); \
    }; \
    int type<__LINE__, counter>::x = type::init(); \
    void type<__LINE__, counter>::test() { \
        call; \
    } \
    ATTO_TEST_INC_COUNTER(__LINE__)

#define ATTO_ASSERT_BASE(expr, msg) \
    atto_test::assert_(expr, __FILE__, __LINE__, [&] { return msg; })

#define ATTO_ASSERT_BINARY_OP(op, x, y) \
    ATTO_ASSERT_BASE(x op y, ATTO_TEST_TO_STR(x op y) " | " + ATTO_TEST_SHOW(x) + ", " + ATTO_TEST_SHOW(y))

#define ATTO_ASSERT_BINARY_FUN(f, x, y) \
    ATTO_ASSERT_BASE(f(x, y), ATTO_TEST_TO_STR(f(x, y)) " | " + ATTO_TEST_SHOW(x) + ", " + ATTO_TEST_SHOW(y))

#define ATTO_ASSERT_EQUAL(x, y) ATTO_ASSERT_BINARY_OP(==, x, y)
#define ATTO_ASSERT_NOT_EQUAL(x, y) ATTO_ASSERT_BINARY_OP(!=, x, y)
#define ATTO_ASSERT_TRUE(x) ATTO_ASSERT_BASE(bool(x), "expected true | " + ATTO_TEST_SHOW(x))
#define ATTO_ASSERT_FALSE(x) ATTO_ASSERT_BASE(!bool(x), "expected false | " + ATTO_TEST_SHOW(x))
#define ATTO_ASSERT_FAIL() ATTO_ASSERT_BASE(false, "reached to failure code path")

#define ATTO_TEST_SHOW(expr) (ATTO_TEST_TO_STR(expr) ": " + atto_test::to_string_(expr))

#define ATTO_TEST_TO_STR(...) ATTO_TEST_TO_STR_I(__VA_ARGS__)
#define ATTO_TEST_TO_STR_I(...) # __VA_ARGS__

#define ATTO_TEST_REQUIRE(...) \
    typename std::enable_if<(__VA_ARGS__)>::type *& = atto_test::enabler

namespace atto_test {
namespace here = ::atto_test;

using boost::fusion::traits::is_sequence;

extern void * enabler;
void add_test(void(*)(), char const *);
void assert_(bool cond, std::string const & file, std::size_t line, std::function<std::string()> msg);

template<std::size_t N>
using size_t = std::integral_constant<std::size_t, N>;

#define ATTO_TEST_GET_COUNTER(id) decltype(atto_test_counter(atto_test::size_t<id>{}, atto_test::max_count{}))
#define ATTO_TEST_INC_COUNTER(id) atto_test::succ<ATTO_TEST_GET_COUNTER(id)> atto_test_counter(atto_test::size_t<id>, atto_test::succ<ATTO_TEST_GET_COUNTER(id)>)

template<std::size_t N>
struct count : count<N-1> {
    static constexpr std::size_t value = N;
};
template<>
struct count<0> {
    static constexpr std::size_t value = 0;
};

using max_count = count<256>;

template<typename N>
using succ = count<N::value+1>;

namespace traits {
template<typename T, typename Enable = void> struct to_string : std::false_type {};
}


struct not_found_via_adl {};

not_found_via_adl begin(...);
not_found_via_adl end(...);
not_found_via_adl to_string(...);

template<typename T> struct sfinae_test : decltype(T::test(0)) {};

template<typename T>
struct has_begin_end_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(std::begin(*p), (void)0, std::end(*p), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using has_begin_end = sfinae_test<has_begin_end_test<T>>;

template<typename T>
struct has_adl_begin_end_test {
    // "template<typename U = T> auto test(int) -> decltype(begin(*p), (void)0, end(*p), (void)0, std::true_type{});" causes compile error in g++, if begin/end not found.
    // I don't know that this behavior and/or below solution is right.
    template<typename U = T, U const * p = nullptr,
             ATTO_TEST_REQUIRE(!has_begin_end<U>::value),
             ATTO_TEST_REQUIRE(!std::is_same<decltype(begin(*p)), not_found_via_adl>::value),
             ATTO_TEST_REQUIRE(!std::is_same<decltype(end(*p)), not_found_via_adl>::value)>
    static auto test(int) -> std::true_type;
    static auto test(...) -> std::false_type;
};

template<typename T> using has_adl_begin_end = sfinae_test<has_adl_begin_end_test<T>>;

template<typename T>
struct has_to_string_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(p->to_string(), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using has_to_string = sfinae_test<has_to_string_test<T>>;
template<typename T>
struct has_adl_to_string_test {
    template<typename U = T, U const * p = nullptr,
             ATTO_TEST_REQUIRE(!has_to_string<U>::value),
             ATTO_TEST_REQUIRE(!std::is_same<decltype(to_string(*p)), not_found_via_adl>::value)>
    static auto test(int) -> std::true_type;
    static auto test(...) -> std::false_type;
};
template<typename T> using has_adl_to_string = sfinae_test<has_adl_to_string_test<T>>;

std::string to_string_(int);
std::string to_string_(unsigned int);
std::string to_string_(long);
std::string to_string_(unsigned long);
std::string to_string_(long long);
std::string to_string_(unsigned long long);
std::string to_string_(double);
std::string to_string_(bool);
std::string to_string_(std::string const &);
std::string to_string_(std::string &&);
std::string to_string_(char const *);
template<typename ...Elems>
std::string to_string_(std::tuple<Elems...> const &);
template<typename T, typename U>
std::string to_string_(std::pair<T, U> const &);
template<typename T>
std::string to_string_(T const &);

template<typename T, ATTO_TEST_REQUIRE(traits::to_string<T>::value)>
std::string to_string_traits(T const & x);
template<typename T, ATTO_TEST_REQUIRE(!traits::to_string<T>::value)>
std::string to_string_traits(T const & x);

template<typename T, ATTO_TEST_REQUIRE(has_to_string<T>::value)>
std::string to_string_memfun(T const &);
template<typename T, ATTO_TEST_REQUIRE(!has_to_string<T>::value)>
std::string to_string_memfun(T const &);

template<typename T, ATTO_TEST_REQUIRE(has_adl_to_string<T>::value)>
std::string to_string_adl(T const &);
template<typename T, ATTO_TEST_REQUIRE(!has_adl_to_string<T>::value)>
std::string to_string_adl(T const &);

template<typename Range, ATTO_TEST_REQUIRE(has_begin_end<Range>::value || has_adl_begin_end<Range>::value), ATTO_TEST_REQUIRE(!is_sequence<Range>::value)>
std::string to_string_range(Range const &);
template<typename T, ATTO_TEST_REQUIRE(!(has_begin_end<T>::value || has_adl_begin_end<T>::value) || is_sequence<T>::value)>
std::string to_string_range(T const &);

template<typename Seq, ATTO_TEST_REQUIRE(is_sequence<Seq>::value)>
std::string to_string_fuseq(Seq const & x);
template<typename T, ATTO_TEST_REQUIRE(!is_sequence<T>::value)>
std::string to_string_fuseq(T const & x);

std::string to_string_fallback();

template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(!Index::value && Index::value < Size::value)>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);
template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(Index::value && Index::value < Size::value)>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);
template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(!(Index::value < Size::value))>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);

template<typename ...Elems>
std::string to_string_(std::tuple<Elems...> const & tup) {
    return here::to_string_tuple("(", tup, here::size_t<0>{}, std::tuple_size<std::tuple<Elems...>>{});
}
template<typename T, typename U>
std::string to_string_(std::pair<T, U> const & p) {
    return here::to_string_tuple("(", p, here::size_t<0>{}, std::tuple_size<std::pair<T, U>>{});
}
template<typename T>
std::string to_string_(T const & x) {
    return here::to_string_traits(x);
}

template<typename T, ATTO_TEST_REQUIRE(traits::to_string<T>::value)>
std::string to_string_traits(T const & x) {
    return traits::to_string<T>()(x);
}
template<typename T, ATTO_TEST_REQUIRE(!traits::to_string<T>::value)>
std::string to_string_traits(T const & x) {
    return here::to_string_memfun(x);
}

template<typename T, ATTO_TEST_REQUIRE(has_to_string<T>::value)>
std::string to_string_memfun(T const & x) {
    return x.to_string();
}
template<typename T, ATTO_TEST_REQUIRE(!has_to_string<T>::value)>
std::string to_string_memfun(T const & x) {
    return here::to_string_range(x);
}

template<typename Range, ATTO_TEST_REQUIRE(has_begin_end<Range>::value || has_adl_begin_end<Range>::value), ATTO_TEST_REQUIRE(!is_sequence<Range>::value)>
std::string to_string_range(Range const & range) {
    std::string str("[");
    bool trailing = false;
    for (auto e : range) {
        if (trailing) str += ", ";
        trailing = true;
        str += here::to_string_(e);
    }
    return str + "]";
}
template<typename T, ATTO_TEST_REQUIRE(!(has_begin_end<T>::value || has_adl_begin_end<T>::value) || is_sequence<T>::value)>
std::string to_string_range(T const & x) {
    return here::to_string_fuseq(x);
}

struct to_string_fuseq_t {
    typedef std::string result_type;
    template<typename T>
    std::string operator()(std::true_type, T const & x) const {
        return "(" + here::to_string_(x);
    }
    template<typename T>
    std::string operator()(std::string const & str, T const & x) const {
        return str + ", " + here::to_string_(x);
    }
};
template<typename Seq, ATTO_TEST_REQUIRE(is_sequence<Seq>::value)>
std::string to_string_fuseq(Seq const & x) {
    return boost::fusion::fold(x, std::true_type{}, to_string_fuseq_t()) + ")";
}
template<typename T, ATTO_TEST_REQUIRE(!is_sequence<T>::value)>
std::string to_string_fuseq(T const & x) {
    return here::to_string_adl(x);
}

template<typename T, ATTO_TEST_REQUIRE(has_adl_to_string<T>::value)>
std::string to_string_adl(T const & x) {
    return to_string(x);
}
template<typename T, ATTO_TEST_REQUIRE(!has_adl_to_string<T>::value)>
std::string to_string_adl(T const &) {
    return to_string_fallback();
}

template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(!Index::value && Index::value < Size::value)>
std::string to_string_tuple(std::string && str, Tuple const & tup, Index, Size size) {
    return here::to_string_tuple(str + here::to_string_(std::get<Index::value>(tup)),
                                 tup,
                                 here::size_t<Index::value+1>{},
                                 size);
}
template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(Index::value && Index::value < Size::value)>
std::string to_string_tuple(std::string && str, Tuple const & tup, Index, Size size) {
    return here::to_string_tuple(str + ", " + here::to_string_(std::get<Index::value>(tup)),
                                 tup,
                                 here::size_t<Index::value+1>{},
                                 size);
}
template<typename Tuple, typename Index, typename Size, ATTO_TEST_REQUIRE(!(Index::value < Size::value))>
std::string to_string_tuple(std::string && str, Tuple const & tup, Index, Size) {
    return str + ")";
}

}

template<std::size_t N>
atto_test::count<0> atto_test_counter(atto_test::size_t<N>, atto_test::count<0> const &);

#endif
