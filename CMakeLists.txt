
#Sfaui cmake example
#Code that has not been commented out is considered as example code
#cmake_minimum_required (VERSION 3.8)


#if (POLICY CMP0141)
  #cmake_policy(SET CMP0141 NEW)
  #set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()

#project ("Sfaui")


#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)


#file(GLOB IMGUI_FILES
   # "${CMAKE_SOURCE_DIR}/imgui/*.h"
   # "${CMAKE_SOURCE_DIR}/imgui/*.cpp"
#)

file(GLOB SFAUI_FILES
    "${CMAKE_SOURCE_DIR}/Sfaui/*.h"
    "${CMAKE_SOURCE_DIR}/Sfaui/*.cpp"
    "${CMAKE_SOURCE_DIR}/Sfaui/*.hpp"
)


#set(MAIN_FILES
 #   "main.cpp"
#)

add_executable (Sfaui 
    #${MAIN_FILES}
    #${IMGUI_FILES}
    ${SFAUI_FILES}
  )


target_compile_options(Sfaui PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>
)


target_include_directories(#Sfaui PRIVATE
   # ${CMAKE_SOURCE_DIR}
   # ${CMAKE_SOURCE_DIR}/imgui
     ${CMAKE_SOURCE_DIR}/Sfaui
)


#if (WIN32)
#target_link_libraries(Sfaui PRIVATE
   # d3d11
   # d3dcompiler
   # dxgi
   # dxguid
   # kernel32
   # user32
   # gdi32
#)
#endif()


#set_target_properties(Sfaui PROPERTIES
  #  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
#)