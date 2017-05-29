list (APPEND flat_rdr_srcs
    flat.c
	flat_boxpieces.c
	flat_widget.c
	flat_combobox.c
	flat_listview.c
	flat_propsheet.c
	flat_groupbox.c
    )

mg_add_source_files(${flat_rdr_srcs})

mg_commit_source_files()

