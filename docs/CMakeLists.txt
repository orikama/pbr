# with help of:
# https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/


find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)


# Find all the public headers
get_target_property(PBR_PUBLIC_HEADER_DIR pbr_lib INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE PBR_PUBLIC_HEADERS ${PBR_PUBLIC_HEADER_DIR}/*.hpp)


# ---------------------------------------------------------------------------------------
# Doxygen
# ---------------------------------------------------------------------------------------
set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${PBR_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating documentations with Doxygen"
                   VERBATIM)

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})


# ---------------------------------------------------------------------------------------
# Sphinx
# ---------------------------------------------------------------------------------------
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD "${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx")
set(SPHINX_INDEX_FILE "${SPHINX_BUILD}/index.html")

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND ${SPHINX_EXECUTABLE} -b html
                           # Tell Breathe where to find the Doxygen output
                           -Dbreathe_projects.PBR=${DOXYGEN_OUTPUT_DIR}/xml
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   # Other docs files you want to track should go here (or in some variable)
                   DEPENDS ${SPHINX_SOURCE}/index.rst
                           ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})
