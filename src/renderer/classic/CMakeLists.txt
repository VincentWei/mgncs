
list (APPEND classic_rdr_srcs
    classic.c 
    classic_widget.c 
    classic_combobox.c 
    classic_groupbox.c 
    classic_propsheet.c 
    classic_listview.c 
    classic_boxpieces.c)

mg_add_source_files(${classic_rdr_srcs})

mg_commit_source_files()


