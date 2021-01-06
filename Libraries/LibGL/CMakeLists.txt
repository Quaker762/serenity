
set(SOURCES
    GLColor.cpp
    GLMat.cpp
    GLState.cpp
    GLUtils.cpp
    GLVert.cpp
)

serenity_lib(LibGL GL)
target_link_libraries(LibGL LibCore LibCpp)
