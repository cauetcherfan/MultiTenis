ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\Allegro
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: MultiTenis.exe

MultiTenis.exe: MultiTenis.o
	gcc -o MultiTenis.exe MultiTenis.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

MultiTenis.o: MultiTenis.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c MultiTenis.c	
	
clean:
	del MultiTenis.o
	del MultiTenis.exe