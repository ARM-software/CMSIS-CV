target_sources(stream PRIVATE 
    stream.c
    stream_ocv.c
    cv_gaussian_scheduler.cpp 
    cv_gaussian_ocv_scheduler.cpp    
    )

target_include_directories(stream PUBLIC .)
