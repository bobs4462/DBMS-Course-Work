CC := gcc 
.INCLUDE_DIRS := ./includes
CPPFLAGS := -I $(.INCLUDE_DIRS) -g
BUILD_DEBUG := yes

LIBS := -lpanel -lmenu -lform -lncurses -lsqlite3

STLDFLAGS := $(LIBS)

OBJ := main.o patient.o doctor.o registry.o login.o

clinic: $(OBJ) $(LIBS)
	$(CC) -o $@ $(OBJ) $(STLDFLAGS) -g
	#./$@
	

%.o: %.c $(.INCLUDE_DIRS)/%.h $(INCLUDE_DIRS)gui.h
	$(CC) $< $(STLDFLAGS) $(CPPFLAGS) -o $@ 



	


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
