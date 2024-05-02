#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zlib" for configuration "Release"
set_property(TARGET zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zlib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/libzlib.a"
  )

list(APPEND _cmake_import_check_targets zlib )
list(APPEND _cmake_import_check_files_for_zlib "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/libzlib.a" )

# Import target "libopenjp2" for configuration "Release"
set_property(TARGET libopenjp2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libopenjp2 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/liblibopenjp2.a"
  )

list(APPEND _cmake_import_check_targets libopenjp2 )
list(APPEND _cmake_import_check_files_for_libopenjp2 "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/liblibopenjp2.a" )

# Import target "libprotobuf" for configuration "Release"
set_property(TARGET libprotobuf APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libprotobuf PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/liblibprotobuf.a"
  )

list(APPEND _cmake_import_check_targets libprotobuf )
list(APPEND _cmake_import_check_files_for_libprotobuf "${_IMPORT_PREFIX}/lib/opencv4/3rdparty/liblibprotobuf.a" )

# Import target "opencv_core" for configuration "Release"
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_core.a"
  )

list(APPEND _cmake_import_check_targets opencv_core )
list(APPEND _cmake_import_check_files_for_opencv_core "${_IMPORT_PREFIX}/lib/libopencv_core.a" )

# Import target "opencv_flann" for configuration "Release"
set_property(TARGET opencv_flann APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_flann PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_flann.a"
  )

list(APPEND _cmake_import_check_targets opencv_flann )
list(APPEND _cmake_import_check_files_for_opencv_flann "${_IMPORT_PREFIX}/lib/libopencv_flann.a" )

# Import target "opencv_imgproc" for configuration "Release"
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_imgproc.a"
  )

list(APPEND _cmake_import_check_targets opencv_imgproc )
list(APPEND _cmake_import_check_files_for_opencv_imgproc "${_IMPORT_PREFIX}/lib/libopencv_imgproc.a" )

# Import target "opencv_photo" for configuration "Release"
set_property(TARGET opencv_photo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_photo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_photo.a"
  )

list(APPEND _cmake_import_check_targets opencv_photo )
list(APPEND _cmake_import_check_files_for_opencv_photo "${_IMPORT_PREFIX}/lib/libopencv_photo.a" )

# Import target "opencv_dnn" for configuration "Release"
set_property(TARGET opencv_dnn APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_dnn PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_dnn.a"
  )

list(APPEND _cmake_import_check_targets opencv_dnn )
list(APPEND _cmake_import_check_files_for_opencv_dnn "${_IMPORT_PREFIX}/lib/libopencv_dnn.a" )

# Import target "opencv_features2d" for configuration "Release"
set_property(TARGET opencv_features2d APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_features2d PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_features2d.a"
  )

list(APPEND _cmake_import_check_targets opencv_features2d )
list(APPEND _cmake_import_check_files_for_opencv_features2d "${_IMPORT_PREFIX}/lib/libopencv_features2d.a" )

# Import target "opencv_calib3d" for configuration "Release"
set_property(TARGET opencv_calib3d APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_calib3d PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_calib3d.a"
  )

list(APPEND _cmake_import_check_targets opencv_calib3d )
list(APPEND _cmake_import_check_files_for_opencv_calib3d "${_IMPORT_PREFIX}/lib/libopencv_calib3d.a" )

# Import target "opencv_objdetect" for configuration "Release"
set_property(TARGET opencv_objdetect APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_objdetect PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_objdetect.a"
  )

list(APPEND _cmake_import_check_targets opencv_objdetect )
list(APPEND _cmake_import_check_files_for_opencv_objdetect "${_IMPORT_PREFIX}/lib/libopencv_objdetect.a" )

# Import target "opencv_video" for configuration "Release"
set_property(TARGET opencv_video APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_video PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libopencv_video.a"
  )

list(APPEND _cmake_import_check_targets opencv_video )
list(APPEND _cmake_import_check_files_for_opencv_video "${_IMPORT_PREFIX}/lib/libopencv_video.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
