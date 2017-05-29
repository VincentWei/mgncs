list ( APPEND ime_srcs
    digit_ime.c mime.c mpinyin-ime.c mpti-ime.c)

mg_add_source_files(${ime_srcs})

mg_commit_source_files()

