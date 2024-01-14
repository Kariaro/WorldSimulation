add_library(glad)
target_sources(glad PUBLIC glad/glad.c)
target_include_directories(glad PUBLIC .)
