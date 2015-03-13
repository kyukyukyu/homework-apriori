LIST(APPEND project_SRCS "main.cpp")

SET(project_BIN ${PROJECT_NAME})

ADD_EXECUTABLE(${project_BIN} ${project_SRCS})
