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


clean: 
	rm -f clinic $(OBJ) 

git: clean
	git add ../.
	git commit
	git push

db: 
	database/redeploy.sh
#	sqlite3 database/clinic.db

.PHONY: clean git db
