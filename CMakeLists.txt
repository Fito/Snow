cmake_minimum_required(VERSION 2.8)

set(ITK_DIR "/Users/socialchorus/Documents/CSC621/InsightToolkit-4.9.0-Bin-x64")
project(Segmentation)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
    find_package(VTK REQUIRED)
      include(${VTK_USE_FILE})
    endif()

    add_executable(Segmentation MACOSX_BUNDLE Segmentation.cpp)

    if( "${ITK_VERSION_MAJOR}" LESS 4 )
        target_link_libraries(Segmentation ITKReview ${ITK_LIBRARIES})
    else( "${ITK_VERSION_MAJOR}" LESS 4 )
        target_link_libraries(Segmentation ${ITK_LIBRARIES})
    endif( "${ITK_VERSION_MAJOR}" LESS 4 )
