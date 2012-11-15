#include "boost/preprocessor/repetition/repeat.hpp"
#include "boost/preprocessor/facilities/intercept.hpp"
#include "attotest/attotest.hpp"

#define F(z, n, data) ATTOTEST_PARAM(f, 0);

void f(int) {}
BOOST_PP_REPEAT(256, F, ~); ATTOTEST_PARAM(f, 0);
