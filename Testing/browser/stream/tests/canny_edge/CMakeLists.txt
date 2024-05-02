target_sources(stream PRIVATE 
    stream.c
    stream_ocv.c
    cv_canny_edge_scheduler.cpp 
    cv_canny_edge_ocv_scheduler.cpp    
    )

target_include_directories(stream PUBLIC .)
