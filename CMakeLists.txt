cmake_minimum_required (VERSION 2.6)

project (mGNCS)

# use for mgncsconfig.h
add_definitions (-D__CMAKE_PROJECT__)
#define the mgncs library flags
add_definitions (-D__MGNCS_LIB__)

set (MGNCS_MAJOR_VERSION 1)
set (MGNCS_MINOR_VERSION 0)
set (MGNCS_MICRO_VERSION 9)

set (MGNCS_NAME "mgncs")
set (MGNCS_LIBSUFFIX "")
set (MGNCS_VERSION ${MGNCS_MAJOR_VERSION}.${MGNCS_MINOR_VERSION}.${MGNCS_MICRO_VERSION})
set (MGNCS_SOVERSION ${MGNCS_VERSION})
set (MGNCS_INCLUDE_LIST "")
set (MGNCS_LIBRARIES_LIST "")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#include cmake file
include (common)


mg_declare_option_and_set_value(_MGNCS_RDR_SKIN     rdr_skin    BOOL ON "include skin renderer")
mg_declare_option_and_set_value(_MGNCS_RDR_FASHION  rdr_fashion BOOL ON "include the fashion renderer")
mg_declare_option_and_set_value(_MGNCS_RDR_FLAT     rdr_flat    BOOL ON "include the flat renderer")
mg_declare_option(ctrl_suffix STRING "_" "set the control suffix")
mg_set_value(_MGNCS_CLASS_SUFFIX ${ctrl_suffix})
mg_declare_option_and_set_value(_MGNCS_USE_DBEDIT   dbuffer_edit BOOL OFF "enable the double buffer for edit")
mg_declare_option_and_set_value(_MGNCS_GUIBUILDER_SUPPORT guibuilder_support BOOL OFF "used to support guibuilder")
mg_declare_option_and_set_value(_MGNCSCTRL_OLD_MLEDIT   ctrl_oldedit BOOL OFF "support the old multiline editor")
mg_declare_option_and_set_value(_MGNCSCTRL_TEXTEDITOR   ctrl_newtexteditor BOOL ON "support the new texteditor")
mg_declare_option_and_set_value(_MGNCSCTRL_TEXTEDITTITLE ctrl_texteditor_title BOOL OFF "support the title of new texteditor")
mg_declare_option_and_set_value(_MGNCS_CHARSET_ICONV charset_iconv BOOL OFF "use iconv as charset convertor")




############### LIB SUFFIX and etc file ################
mg_declare_option(with_libsuffix STRING "" "Configure the suffix of mGNCS")
if (NOT with_libsuffix STREQUAL "")
    set (MGNCS_LIBSUFFIX "_${with_libsuffix}")
    set (mGNCS_LIBSUFFIX "${MGNCS_LIBSUFFIX}")
endif (NOT with_libsuffix STREQUAL "")

mg_set_value (MGNCS_ETCFILENAME "mgncs.cfg")
##########################################################

mg_declare_option_and_set_value(_MGNCSDB_DATASOURCE db_datasource BOOL ON "support the datasource")
mg_declare_option_and_set_value(_MGNCSDB_SQLITE     db_sqlite BOOL OFF "support the sqlite datasource")
if (_MGNCSDB_SQLITE)
set (_MGNCSDB_DATASOURCE ON)
endif(_MGNCSDB_SQLITE)

mg_declare_option_and_set_value(_MGNCSDB_XML        db_xml BOOL OFF "support the xml datasource")
if (_MGNCSDB_XML)
set (_MGNCSDB_DATASOURCE ON)
endif(_MGNCSDB_XML)

mg_declare_option_and_set_value(_MGNCSDB_STATIC db_static BOOL ON "enable staticdatasource support")
if (_MGNCSDB_STATIC)
set (_MGNCSDB_DATASOURCE ON)
endif(_MGNCSDB_STATIC)

mg_declare_option_and_set_value(_MGNCSDB_TEXT db_text BOOL ON "enable textdatasource support")
if (_MGNCSDB_TEXT)
set (_MGNCSDB_DATASOURCE ON)
endif(_MGNCSDB_TEXT)

mg_declare_option_and_set_value(_MGNCSDB_INI db_ini BOOL ON "enable inidatasource support")
if (_MGNCSDB_INI)
set (_MGNCSDB_DATASOURCE ON)
endif(_MGNCSDB_INI)

mg_declare_option_and_set_value(_MGNCSCTRL_ANIMATE ctrl_animate BOOL ON "enable animate support")
mg_declare_option_and_set_value(_MGNCSCTRL_COLORBUTTON ctrl_colorbutton BOOL ON "enable colorbutton support")
mg_declare_option_and_set_value(_MGNCSCTRL_CONTAINER ctrl_container BOOL ON "enable container support")
mg_declare_option_and_set_value(_MGNCSCTRL_PAGE ctrl_page BOOL ON "enable page support")
if (_MGNCSCTRL_PAGE)
set (_MGNCSCTRL_CONTAINER ON)
endif(_MGNCSCTRL_PAGE)

mg_declare_option_and_set_value(_MGNCSCTRL_PROPSHEET ctrl_propsheet BOOL ON "enable propsheet support")
if (_MGNCSCTRL_PROPSHEET)
set (_MGNCSCTRL_CONTAINER ON)
set (_MGNCSCTRL_PAGE ON)
endif(_MGNCSCTRL_PROPSHEET)

mg_declare_option_and_set_value(_MGNCSCTRL_DIALOGBOX ctrl_dialogbox BOOL ON "enable dialogbox support")

mg_declare_option_and_set_value(_MGNCSCTRL_ICONVIEW ctrl_iconview BOOL ON "enable iconview support")
mg_declare_option_and_set_value(_MGNCSCTRL_IMAGEBUTTON ctrl_imagebutton BOOL ON "enable imagebutton support")
mg_declare_option_and_set_value(_MGNCSCTRL_LEDLABEL ctrl_ledlabel BOOL ON "enable ledlabel support")
mg_declare_option_and_set_value(_MGNCSCTRL_LIST ctrl_list BOOL ON "enable list support")
mg_declare_option_and_set_value(_MGNCSCTRL_LISTBOX ctrl_listbox BOOL ON "enable listbox support")
mg_declare_option_and_set_value(_MGNCSCTRL_COMBOBOX ctrl_combobox BOOL ON "enable combobox support")
if (_MGNCSCTRL_COMBOBOX)
set (_MGNCSCTRL_LISTBOX ON)
endif(_MGNCSCTRL_COMBOBOX)

mg_declare_option_and_set_value(_MGNCSCTRL_LISTVIEW ctrl_listview BOOL ON "enable listview support")
mg_declare_option_and_set_value(_MGNCSCTRL_MENUBUTTON ctrl_menubutton BOOL ON "enable menubutton support")
mg_declare_option_and_set_value(_MGNCSCTRL_PROGRESSBAR ctrl_progressbar BOOL ON "enable progressbar support")
mg_declare_option_and_set_value(_MGNCSCTRL_RECTANGLE ctrl_rectangle BOOL ON "enable rectangle support")
mg_declare_option_and_set_value(_MGNCSCTRL_SPINNER ctrl_spinner BOOL ON "enable spinner support")
mg_declare_option_and_set_value(_MGNCSCTRL_SPINBOX ctrl_spinbox BOOL ON "enable spinbox support")
if (_MGNCSCTRL_SPINBOX)
set (_MGNCSCTRL_SPINNER ON)
endif(_MGNCSCTRL_SPINBOX)

mg_declare_option_and_set_value(_MGNCSCTRL_MONTHCALENDAR ctrl_monthcalendar BOOL ON "enable monthcalendar support")
if (_MGNCSCTRL_MONTHCALENDAR)
set (_MGNCSCTRL_SPINNER ON)
set (_MGNCSCTRL_SPINBOX ON)
endif(_MGNCSCTRL_MONTHCALENDAR)

mg_declare_option_and_set_value(_MGNCSCTRL_SLIDER ctrl_slider BOOL ON "enable slider support")
mg_declare_option_and_set_value(_MGNCSCTRL_SCROLLBAR ctrl_scrollbar BOOL ON "enable scrollbar support")
if (_MGNCSCTRL_SCROLLBAR)
set (_MGNCSCTRL_SLIDER ON)
endif(_MGNCSCTRL_SCROLLBAR)

mg_declare_option_and_set_value(_MGNCSCTRL_TRACKBAR ctrl_trackbar BOOL ON "enable trackbar support")
if (_MGNCSCTRL_TRACKBAR)
set (_MGNCSCTRL_SLIDER ON)
endif(_MGNCSCTRL_TRACKBAR)

mg_declare_option_and_set_value(_MGNCSCTRL_TOOLBAR ctrl_toolbar BOOL ON "enable toolbar support")
mg_declare_option_and_set_value(_MGNCSENGINE_IME engine_ime BOOL ON "enable imeengine support")
mg_declare_option_and_set_value(_MGNCSCTRL_IMWORDSEL ctrl_imwordsel BOOL ON "enable imwordsel support")
if (_MGNCSCTRL_IMWORDSEL)
set (_MGNCSENGINE_IME ON)
endif (_MGNCSCTRL_IMWORDSEL)

mg_declare_option_and_set_value(_MGNCSENGINE_PINYIN engine_pinyin BOOL ON "enable pinyinengine support")
if (_MGNCSENGINE_PINYIN)
set (_MGNCSENGINE_IME ON)
endif(_MGNCSENGINE_PINYIN)

mg_declare_option_and_set_value(_MGNCSENGINE_DIGIT engine_digit BOOL ON "enable digitengine support")
if (_MGNCSENGINE_DIGIT)
set (_MGNCSENGINE_IME ON)
endif(_MGNCSENGINE_DIGIT)

mg_declare_option_and_set_value(_MGNCSENGINE_PTI engine_pti BOOL ON "enable ptiengine support")
if (_MGNCSENGINE_PTI)
set (_MGNCSENGINE_IME ON)
endif(_MGNCSENGINE_PTI)

mg_declare_option_and_set_value(_MGNCSENGINE_DIGITPTI engine_digitpti BOOL ON "enable digitptiengine support")
if (_MGNCSENGINE_DIGITPTI)
set (_MGNCSENGINE_IME ON)
set (_MGNCSENGINE_PTI ON)
set (_MGNCSENGINE_DIGIT ON)
endif(_MGNCSENGINE_DIGITPTI)

mg_declare_option_and_set_value(_MGNCSENGINE_DIGITPY engine_digitpy BOOL ON "enable digitpyengine support")
if (_MGNCSENGINE_DIGITPY)
set (_MGNCSENGINE_IME ON)
set (_MGNCSENGINE_DIGIT ON)
set (_MGNCSENGINE_PINYIN ON)
endif(_MGNCSENGINE_DIGITPY)

############## check header files ################
include (CheckIncludeFiles) 
check_include_files(alloca.h HAVE_ALLOCA_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(locale.h HAVE_LOCALE_H)
check_include_files(math.h HAVE_MATH_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(termio.h HAVE_TERMIO_H)
check_include_files(unistd.h HAVE_UNISTD_H)

############## check functions ################
include(CheckFunctionExists)
check_function_exists (alloca HAVE_ALLOCA)
check_function_exists (time HAVE_TIME)
check_function_exists (getpagesize HAVE_GETPAGESIZE)
check_function_exists (mktime HAVE_MKTIME)
check_function_exists (localtime HAVE_LOCALTIME)
check_function_exists (strdup HAVE_STRDUP)
check_function_exists (strcasecmp HAVE_STRCASECMP)
check_function_exists (strncasecmp HAVE_STRNCASECMP)
check_function_exists (strerror HAVE_STRERROR)
check_function_exists (getpt HAVE_GETPT)
check_function_exists (mmap HAVE_MMAP)
check_function_exists (setlocale HAVE_SETLOCALE)
check_function_exists (vprintf HAVE_VPRINTF)

#########################################################################
# find and add the path
macro(mgncs_find_add_path _file)
    find_path(_path_find ${_file})
    if (_path_find)
        add_definitions(-I${_path_find})
        set (MGNCS_INCLUDE_LIST "${MGNCS_INCLUDE_LIST} -I${_path_find}")
    else()
        message(FATAL_ERROR "cannot find the file ${_file}")
    endif(_path_find)
endmacro(mgncs_find_add_path)

if (_MGNCSDB_XML)
    mgncs_find_add_path(libxml/parse.h)
endif(_MGNCSDB_XML)

if (_MGNCSDB_SQLITE)
    mgncs_find_add_path(sqlite3.h)
endif(_MGNCSDB_SQLITE)


#########################################################################

include (CheckSymbolExists)
## check for test
macro(find_and_add_lib _libname  _error_msg)
    find_library(_lib_find ${_libname})
    if (NOT _lib_find)
        message(FATAL_ERROR ${_error_msg})
    endif (NOT _lib_find)
    if (MGNCS_APP_LIBRARIES)
        set (MGNCS_APP_LIBRARIES "${MGNCS_APP_LIBRARIES} -l${_libname}")
    else()
        set (MGNCS_APP_LIBRARIES "-l${_libname}")
    endif (MGNCS_APP_LIBRARIES)
endmacro(find_and_add_lib)

macro(check_and_add_lib _libappname _symbol _file _error_msg)
    unset(_sym_find CACHE)
    check_symbol_exists (${_symbol} "${_file}" _sym_find)
    #not found
    if (_sym_find)
        find_and_add_lib (${_libappname}  ${_error_msg})
    else ()
        message("-- cannot find ${_symbol} in file ${_file}")
    endif (_sym_find)
endmacro(check_and_add_lib)


######################### check minigui ######################
check_symbol_exists(_MGRM_THREADS "minigui/common.h" MINIGUI_THREAD)
check_symbol_exists(_MGRM_PROCESSES "minigui/common.h" MINIGUI_PROCESS)
check_symbol_exists(_MGRM_STANDALONE "minigui/common.h" MINIGUI_STANDALONE)

set (MINIGUI_LIB "minigui")
if (MINIGUI_THREAD)
    set (MINIGUI_LIB "minigui_ths")
    add_definitions(-D_REENTRANT)
    find_and_add_lib(pthread "cannot find the pthread libraries")
elseif (MINIGUI_PROCESS)
    set (MINIGUI_LIB "minigui_procs")
    check_and_add_lib(pthread _MGGAL_SHADOW "minigui/common.h" "cannot find the pthread need by shadow gal")
elseif (MINIGUI_STANDALONE)
    set (MINIGUI_LIB "minigui_sa")
else()
    message (FATAL_ERROR "No MiniGUI")
endif(MINIGUI_THREAD)

if (MGNCS_LIBSUFFIX STREQUAL "msd")
    set (MINIGUI_LIB "minigui_msd")
endif (MGNCS_LIBSUFFIX STREQUAL "msd")


set (MGNCS_APP_LIBRARIES "${MGNCS_APP_LIBRARIES} -l${MINIGUI_LIB}")

check_and_add_lib(m _HAVE_MATH_LIB "minigui/common.h" "cannot find the math libraries")
check_and_add_lib(png _MGIMAGE_PNG "minigui/common.h" "cannot find the png libraries")
check_and_add_lib(jpeg _MGIMAGE_JPG "minigui/common.h" "cannot find the png libraires")
check_and_add_lib(ttf _MGFONT_TTF "minigui/common.h" "canot find the font ttf libraires")
check_and_add_lib(freetype _MGFONT_FT2 "minigui/common.h" "cannot find the freetype2 libraires")
check_and_add_lib(dl _MGIAL_DLCUSTOM "minigui/common.h" "cannot find the dl libraires")

if (_MGNCS_RDR_FASHION)
    set (MGNCS_APP_LIBRARIES  "${MGNCS_APP_LIBRARIES} -lmgplus${mGNCS_LIBSUFFIX}")
endif(_MGNCS_RDR_FASHION)

add_subdirectory (src)

########################### test and samples ######################################
mg_declare_option(build_test BOOL ON "build test or not")
if (build_test)
    add_subdirectory (test)
endif(build_test)

mg_declare_option(build_samples BOOL ON "build samles or not")
if (build_samples)
    add_subdirectory (samples)
endif(build_samples)



############################################################################

###############################################################################
# mgncsconfig.h mgncs.pc
set (prefix ${CMAKE_INSTALL_PREFIX})
set (exec_prefix "${prefix}")
set (libdir "${exec_prefix}/lib")
set (includedir "${prefix}/include")
set (mGNCS_VERSION ${MGNCS_VERSION})

configure_file(
    "${PROJECT_SOURCE_DIR}/mgncs.pc.in"
    "${PROJECT_BINARY_DIR}/${MGNCS_NAME}.pc")
#configure header file of mgncsconfig to pass the CMake settings
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake_mgncsconfig.h.in"
    "${PROJECT_BINARY_DIR}/mgncsconfig.h")


##############################################################################

######################### files to install ####################################

set (etc_files "${PROJECT_SOURCE_DIR}/etc/${MGNCS_ETCFILENAME}")

# etc
install (FILES "${etc_files}" DESTINATION etc)

#include
install (FILES "${PROJECT_BINARY_DIR}/mgncsconfig.h"
    DESTINATION include/${MGNCS_NAME})

install (DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include/${MGNCS_NAME}
    FILES_MATCHING PATTERN "*.h"
                   PATTERN "*.c"
                   PATTERN ".svn" EXCLUDE
    )
               
#pkgconfig
install (FILES "${PROJECT_BINARY_DIR}/${MGNCS_NAME}.pc"
    DESTINATION lib/pkgconfig)

###############################################################################

################################### make dist package source #################
set (CPACK_PACKAGE_VERSION_MAJOR ${MGNCS_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${MGNCS_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${MGNCS_MICRO_VERSION})
set (CPACK_SOURCE_GENERATOR "TBZ2")

set (CPACK_SOURCE_IGNORE_FILES
    "/build/;/configs/;/test/;/doc/;/simples/;scripts/;
    .o$;~$;/.svn/;.swp$;Makefile.am$;makefile*;.edited$;
    .sh$;.old$;.tmpl$;configure.in$;config.log$;
    ${CPACK_SOURCE_IGNORE_FILES}")

set (CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${mGNCS_LIBSUFFIX}")


##############################################################################


