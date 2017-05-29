
list (APPEND fashion_rdr_srcs
    fashion_common.c 
	fashion.c 
	fashion_widget.c 
	fashion_groupbox.c 
	fashion_combobox.c 
	fashion_propsheet.c fashion_listview.c 
	fashion_boxpieces.c	
    )

mg_add_source_files(${fashion_rdr_srcs})

mg_commit_source_files()


