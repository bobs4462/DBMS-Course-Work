CC := gcc 
CPPFLAGS := -I includes
BUILD_DEBUG := yes

vpath %.h includes
LIBS := -lpanel -lmenu -lform -lncurses

STLDFLAGS := $(LIBS)

OBJ := main.o gui.o

clinic: $(OBJ) $(LIBS)
	$(CC) -o $@ $(OBJ) $(STLDFLAGS)
	./$@
	

$(OBJ): gui.h

clean: 
	rm -f clinic $(OBJ) 

git: 
	git add ../.
	git commit
	git push

.PHONY: clean git
