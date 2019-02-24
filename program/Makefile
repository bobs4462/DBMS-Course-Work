CC := gcc 
CPPFLAGS := -I includes -g
BUILD_DEBUG := yes

vpath %.h includes
LIBS := -lpanel -lmenu -lform -lncurses -lsqlite3

STLDFLAGS := $(LIBS)

OBJ := main.o patient.o doctor.o registry.o login.o

clinic: $(OBJ) $(LIBS)
	$(CC) -o $@ $(OBJ) $(STLDFLAGS) -g
	#./$@
	
%.o: %.h gui.h

#main.o: gui.h

#patient.o: gui.h patient.h

#doctor.o: doctor.h gui.h

#registry.o: registry.h gui.h

clean: 
	rm -f clinic $(OBJ) 

git: 
	git add ../.
	git commit
	git push

.PHONY: clean git
