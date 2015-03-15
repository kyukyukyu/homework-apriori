FILE (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)

SET(project_BIN ${PROJECT_NAME})

ADD_EXECUTABLE(${project_BIN} ${project_SRCS})
