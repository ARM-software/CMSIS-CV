target_sources(stream PRIVATE 
    stream.c
    stream_ocv.c
    cv_{{name}}_scheduler.cpp 
    cv_{{name}}_ocv_scheduler.cpp    
    )

target_include_directories(stream PUBLIC .)
