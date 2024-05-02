target_sources(stream PRIVATE 
    stream.c
    stream_ocv.c
    cv_template_scheduler.cpp 
    cv_template_ocv_scheduler.cpp    
    )

target_include_directories(stream PUBLIC .)
