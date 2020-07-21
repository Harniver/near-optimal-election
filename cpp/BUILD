cc_library(
    name = "frac",
    hdrs = ["frac.hpp"],
    srcs = ['frac.cpp'],
    visibility = [
        '//visibility:public',
    ],
)

cc_library(
    name = "func",
    hdrs = ["func.hpp"],
    srcs = ['func.cpp'],
    deps = [
        "//cpp:frac",
        "//cpp:max_deque",
        "//cpp:sq2",
    ],
    visibility = [
        '//visibility:public',
    ],
)

cc_library(
    name = "max_deque",
    hdrs = ["max_deque.hpp"],
    srcs = ['max_deque.cpp'],
    visibility = [
        '//visibility:public',
    ],
)

cc_library(
    name = "sq2",
    hdrs = ["sq2.hpp"],
    srcs = ['sq2.cpp'],
    visibility = [
        '//visibility:public',
    ],
)
