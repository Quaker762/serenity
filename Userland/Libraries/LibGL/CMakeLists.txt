set(SOURCES
    GLColor.cpp
    GLMat.cpp
    GLState.cpp
    GLUtils.cpp
    GLVert.cpp
    SoftwareGLContext.cpp
)

serenity_lib(LibGL gl)
target_link_libraries(LibGL LibM LibCore)
