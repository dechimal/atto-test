#include <vector>
#include <tuple>
#include "boost/fusion/include/make_vector.hpp"
#include "boost/fusion/include/equal_to.hpp"

#include "atto_test/atto_test.hpp"

ATTO_TEST(hoge) {
    ATTO_ASSERT_EQUAL(1, 1);
    ATTO_ASSERT_NOT_EQUAL(2, 1);
    ATTO_ASSERT_EQUAL(std::make_tuple(1, 2, 3), std::make_tuple(1, 2, 3));
    auto x = std::make_tuple(1, std::vector<int>{2, 3, 4}, 3);
    ATTO_ASSERT_EQUAL(x, std::make_tuple(1, std::vector<int>{2, 3, 4}, 3));
    ATTO_ASSERT_EQUAL(boost::fusion::make_vector(1, 2, 3), boost::fusion::make_vector(1, 2, 3));
}

struct hoge : private std::vector<int> {
    typedef std::vector<int> base;
    using base::begin;
    using base::end;
    hoge(std::initializer_list<int> l) : base(l) {}
    friend bool operator==(hoge const & x, hoge const & y) {
        return static_cast<base const &>(x) == static_cast<base const &>(y);
    }
};

namespace ns {
struct fuga {
    fuga(std::initializer_list<int> l) : base(l) {}
    friend std::vector<int>::iterator begin(fuga & x) {
        return x.base.begin();
    }
    friend std::vector<int>::iterator end(fuga & x) {
        return x.base.end();
    }
    friend bool operator==(fuga const & x, fuga const & y) {
        return x.base == y.base;
    }
    std::vector<int> base;
};
}

namespace ns2 {
enum struct piyo { a, b };
std::string to_string(piyo x) {
    switch (x) {
    case piyo::a: return "piyo::a";
    case piyo::b: return "piyo::b";
    default: return "piyo::???";
    }
}
}

namespace ns3 {
struct piyo {
    bool x;
    std::string to_string() const {
        return x ? "piyo.x is true" : "piyo.x is false";
    }
};
bool operator==(piyo x, piyo y) {
    return x.x == y.x;
}
std::string to_string(piyo x) {
    return "this function never called in ATTO_ASSERT_*";
}
}

ATTO_TEST(fuga) {
    auto y = hoge{1, 2, 3};
    ATTO_ASSERT_EQUAL((hoge{1, 2, 3}), y);
    ATTO_ASSERT_EQUAL((ns::fuga{1, 2, 3}), (ns::fuga{1, 2, 3}));
    ATTO_ASSERT_EQUAL(ns2::piyo::a, ns2::piyo::a);
    ATTO_ASSERT_EQUAL(ns3::piyo{true}, ns3::piyo{true});
}

void equal(int x, int y) {
    ATTO_ASSERT_EQUAL(x, y);
}

ATTO_TEST_PARAM(equal, 1, 1);
ATTO_TEST_PARAM(equal, 2, 2);
