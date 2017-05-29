
include_directories("${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/test")

link_directories("${PROJECT_BINARY_DIR}/src")

set (TEST_APP_LIBS "-lmgncs${mGNCS_LIBSUFFIX} -lmgutils${mGNCS_LIBSUFFIX} ${MGNCS_APP_LIBRARIES}")

message(STATUS "The Test Applications link ${TEST_APP_LIBS}")

list(APPEND simple_targets
    static ledstatic radiogroup container progressbar combobox
    spinbox spinner mainwnd image propsheet 
    commrdrtest listbox trackbar rectangle 
    groupbox button checkbutton timer iconview sledit listview scrollview
    scrollbar mledit toolbar menubutton 
    animation monthcalendar
    name2value separator colorbutton 
    bindtest 
    staticds 
    textedit 
    resdump list list_group
    )

foreach(_simple_target  ${simple_targets})
    add_executable(${_simple_target} "${_simple_target}.c")
    target_link_libraries(${_simple_target} "${TEST_APP_LIBS}")
endforeach(_simple_target)


