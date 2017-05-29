list (APPEND skin_rdr_srcs
            skin.c
			skin_widget.c
			skin_boxpieces.c
			skin_combobox.c
			skin_groupbox.c
			skin_propsheet.c skin_listview.c
            )

mg_add_source_files(${skin_rdr_srcs})

mg_commit_source_files()


