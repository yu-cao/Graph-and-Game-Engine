cmake_minimum_required(VERSION 3.13)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 11)

# 添加头文件
set(GLEW_H /usr/local/Cellar/glew/2.1.0/include/GL)
set(GLFW_H /usr/local/Cellar/glfw/3.2.1/include/GLFW)
set(GLAD_H /usr/local/Cellar/glad)
set(GLM_H /usr/local/Cellar/glm) # 增加了glmath库的支持
set(ASSIMP_H /usr/local/Cellar/assimp/4.1.0/include/assimp)
include_directories(${GLEW_H} ${GLFW_H} ${GLAD_H} ${GLM_H} ${ASSIMP_H})

# 添加目标链接
set(GLEW_LINK /usr/local/Cellar/glew/2.1.0/lib/libGLEW.2.1.dylib)
# 推荐对GitHub上的glfw库clone然后进行本地源码编译安装，得到3.3的dylib，brew下载的库直接使用，可能会在执行glfwCreateWindow()时报错
# 参考情况：https://github.com/glfw/glfw/issues/908
# 安装方法：https://my.oschina.net/freeblues/blog/687630
set(GLFW_LINK /usr/local/lib/libglfw.3.3.dylib)
set(ASSIMP_LINK /usr/local/Cellar/assimp/4.1.0/lib/libassimp.4.1.0.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK} ${ASSIMP_LINK})

set(SOURCE_FILES "/usr/local/Cellar/glad/src/glad.c" "src/main.cpp" utils/InputStream.cpp utils/InputStream.h include/Vec3.h include/Vertex.cpp include/Vertex.h include/triangle.cpp include/triangle.h include/Material.cpp include/Material.h include/ModelSection.cpp include/ModelSection.h include/SubBlueprint.cpp include/SubBlueprint.h include/Blueprint.cpp include/Blueprint.h component/Component.h component/SpreaderComp.h)
add_executable(RenderEngine ${SOURCE_FILES})

if (APPLE)
    target_link_libraries(RenderEngine "-framework OpenGL")
endif()