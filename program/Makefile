CC := gcc 
CPPFLAGS := -I includes -g
BUILD_DEBUG := yes

vpath %.h includes
LIBS := -lpanel -lmenu -lform -lncursesw -lsqlite3

STLDFLAGS := $(LIBS)

OBJ := main.o gui.o sql_adapter.o

clinic: $(OBJ) $(LIBS)
	$(CC) -o $@ $(OBJ) $(STLDFLAGS) -g
	./$@
	

$(OBJ): gui.h sqlite_adapter.h

clean: 
	rm -f clinic $(OBJ) 

git: 
	git add ../.
	git commit
	git push

.PHONY: clean git
