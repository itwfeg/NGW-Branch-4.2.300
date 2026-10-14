
# this function is lifted from Qt cmake code (05/2022) by EMC
# it's available from Qt 5.15 onward, but not on earlier versions, e.g. 5.5

# if the macro is already defined, we can just return here immediately
if(COMMAND QT5_ADD_BIG_RESOURCES)
return()
endif()

message("qt5_add_big_resources NOT found, using local version")

function(QT5_ADD_BIG_RESOURCES outfiles )

set(options)
set(oneValueArgs)
set(multiValueArgs OPTIONS)

cmake_parse_arguments(_RCC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

set(rcc_files ${_RCC_UNPARSED_ARGUMENTS})
set(rcc_options ${_RCC_OPTIONS})

if("${rcc_options}" MATCHES "-binary")
    message(WARNING "Use qt5_add_binary_resources for binary option")
endif()

foreach(it ${rcc_files})
    get_filename_component(outfilename ${it} NAME_WE)
    get_filename_component(infile ${it} ABSOLUTE)
    set(tmpoutfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}tmp.cpp)
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.o)

    _QT5_PARSE_QRC_FILE(${infile} _out_depends _rc_depends)
    set_source_files_properties(${infile} PROPERTIES SKIP_AUTORCC ON)
    add_custom_command(OUTPUT ${tmpoutfile}
                       COMMAND ${Qt5Core_RCC_EXECUTABLE} ${rcc_options} --name ${outfilename} --pass 1 --output ${tmpoutfile} ${infile}
                       DEPENDS ${infile} ${_rc_depends} "${out_depends}" VERBATIM)
    add_custom_target(big_resources_${outfilename} ALL DEPENDS ${tmpoutfile})
    add_library(rcc_object_${outfilename} OBJECT ${tmpoutfile})
    set_target_properties(rcc_object_${outfilename} PROPERTIES AUTOMOC OFF)
    set_target_properties(rcc_object_${outfilename} PROPERTIES AUTOUIC OFF)
    add_dependencies(rcc_object_${outfilename} big_resources_${outfilename})
    # The modification of TARGET_OBJECTS needs the following change in cmake
    # https://gitlab.kitware.com/cmake/cmake/commit/93c89bc75ceee599ba7c08b8fe1ac5104942054f
    add_custom_command(OUTPUT ${outfile}
                       COMMAND ${Qt5Core_RCC_EXECUTABLE}
                       ARGS ${rcc_options} --name ${outfilename} --pass 2 --temp $<TARGET_OBJECTS:rcc_object_${outfilename}> --output ${outfile} ${infile}
                       DEPENDS rcc_object_${outfilename}
                       VERBATIM)
   list(APPEND ${outfiles} ${outfile})
endforeach()
set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()