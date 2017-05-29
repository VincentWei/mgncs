
## append the source
list(APPEND mgncs_srcs 
    mobject.c mcomponent.c mwidget.c mresmanager.c 
    mstatic.c mimage.c mrect.c mgroupbox.c mbuttongroup.c 
    mbutton.c mcheckbutton.c mradiobutton.c 
    mctrls.c mscroll_widget.c mpanel.c mprogressbar.c 
    mitem.c mitem_manager.c mitemview.c mlistbox.c 
    mcombobox.c mspinbox.c mspinner.c mslider.c 
    mtrackbar.c mcontainer.c mpage.c mpropsheet.c 
    mcommon.c mmainwnd.c mdialog.c minvisible-component.c 
    mem-slab.c mtimer.c medit.c msledit.c mscrollview.c 
    miconview.c mlistcolumn.c mlistitem.c mlistview.c 
    mmledit.c  mscrollbar.c 
    mmonthcalendar.c 
    mpopmenumgr.c 
    mtoolitem.c 
    mtoolbar.c 
    mmenubutton.c 
    mledstatic.c
    mcolorbutton.c
    manimate.c manimateframes.c mgifanimateframes.c mbmparrayanimateframes.c mmemanimateframes.c
    mseparator.c
    mdatabinding.c
    mdatasource.c
    mtype.c
    mime-wordsel.c 
    mnode.c mradionode.c mchecknode.c mabstractlist.c mlist.c mlist_layout.c
    mrdr.c hashtable.c
    mbtree.c mtextedit.c
    )

mg_add_source_files(${mgncs_srcs})


mg_add_sub_source_dirs (
    pieces
    renderer
    ime
    datasource
    )


include_directories ("${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/pieces"
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/renderer/classic"
    "${PROJECT_SOURCE_DIR}/renderer/fashion"
    "${PROJECT_SOURCE_DIR}/renderer/skin"
    "${PROJECT_SOURCE_DIR}/renderer/flat"
    "${PROJECT_SOURCE_DIR}/ime"
    "${PROJECT_SOURCE_DIR}/datasource"
    )

set (MGNCS_LIBNAME ${MGNCS_NAME}${mGNCS_LIBSUFFIX})

add_library(${MGNCS_LIBNAME} SHARED ${_source_list_})
set_target_properties (${MGNCS_LIBNAME} PROPERTIES VERSION ${MGNCS_VERSION} SOVERSION ${MGNCS_SOVERSION})
set_target_properties (${MGNCS_LIBNAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if (_STATICLIB_OUTPUT)
    add_library(${MGNCS_LIBNAME}_static ${_source_list_})
    if (NOT WIN32)
        set_target_properties (${MGNCS_LIBNAME}_static PROPERTIES OUTPUT_NAME ${MGNCS_LIBNAME})
    endif (NOT WIN32)
    set_target_properties (${MGNCS_LIBNAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif(_STATICLIB_OUTPUT)


################################## target to install #########################
install (TARGETS ${MGNCS_LIBNAME}
    DESTINATION lib
    #  LIBRARY DESTINATION lib  # for .so 
    #  RUNTIME DESTINATION lib  # for .dll
    #  ARCHIVE DESTINATION lib  # for .lib
    )

if (_STATICLIB_OUTPUT)
    install (TARGETS ${MGNCS_LIBNAME}_static
        ARCHIVE DESTINATION lib
        )
endif (_STATICLIB_OUTPUT)


