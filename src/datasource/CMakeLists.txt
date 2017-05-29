list (APPEND datasource_srcs
    comm_parser.c
    mstaticds.c 
    mtextds.c 
    minids.c 
    msqlds.c 
    mxmlds.c xquery.c
    )

mg_add_source_files(${datasource_srcs})

mg_commit_source_files()

