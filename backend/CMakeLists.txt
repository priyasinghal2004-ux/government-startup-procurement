cmake_minimum_required(VERSION 3.16)
project(gov_startup_backend CXX)

# INTERVIEW DEFENSE:
# We require modern C++17 for structured bindings, std::optional, and standard lambdas.
# find_package(Drogon) pulls in Drogon without external ORM wrappers like libpqxx.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)   # header-only JWT encode/decode
find_package(bcrypt CONFIG REQUIRED)    # password hashing

aux_source_directory(controllers CTRL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(utils UTIL_SRC)
aux_source_directory(ranking RANKING_SRC)

add_executable(${PROJECT_NAME}
    main.cc
    ${CTRL_SRC}
    ${FILTER_SRC}
    ${MODEL_SRC}
    ${PLUGIN_SRC}
    ${UTIL_SRC}
    ${RANKING_SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon jwt-cpp::jwt-cpp bcrypt::bcrypt)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.json
)
