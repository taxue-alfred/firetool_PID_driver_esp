idf_component_register(
        SRCS "firetool_PID_adaptor.c" "adaptor_low_layer.c"
        INCLUDE_DIRS "includes"
        PRIV_REQUIRES driver)