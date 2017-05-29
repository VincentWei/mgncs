
LEX=flex

vaargs_trans:vaargs_trans.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

vaargs_trans.c:trans.l
	$(LEX) -o $@ $<

clean:
	$(RM) -f *.o vaargs_trans.c vaargs_trans


