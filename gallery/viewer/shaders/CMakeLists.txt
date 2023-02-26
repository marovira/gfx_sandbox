file(GLOB _SHADER_LIST CONFIGURE_DEPENDS 
    "*.vert"
    "*.tesc"
    "*.tese"
    "*.geom"
    "*.frag"
    "*.comp"
    )
set(SHADER_LIST ${_SHADER_LIST} PARENT_SCOPE)
