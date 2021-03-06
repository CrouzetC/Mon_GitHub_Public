###########################################################################
#                                                                         #
#                           Simulation                                    #
#                                                                         #
###########################################################################

### flags #################################################################

MY_INCLUDE = -I./src

GL_INCLUDE = -I../common/freeglut/include/

LINKER_FLAGS = -Wl,-R,./lib  # indicate to the linker where are the dynamic librairies (liblut.so(.3...))

LIB_FLAGS = -lglut -lGLU -lGL -lXrandr -lXxf86vm -lX11

###########################################################################

O = bin/o_files/

BASE   = src/base/
BASE_O = $(O)graphic.o $(O)graphic_elem.o $(O)picture.o $(O)complex.o $(O)input.o $(O)util.o

SIMULATION   = src/simulation/
SIMULATION_O = $(O)simulation_main.o $(O)simulation.o

###########################################################################

all: bin/Simulation
build: bin/Simulation

clean:
	rm -f bin/o_files/*.o
	rm -f bin/Simulation

run:
#	export LD_LIBRAIRY_PATH=$(LD_LIBRAIRY_PATH):./  # not necessary anymore, thanks to "-Wl,-R,./lib/"
	cd bin/ && ./Simulation

###########################################################################

simulation: bin/Simulation
Simulation: bin/Simulation

### final program #########################################################

bin/Simulation: $(BASE_O) $(SIMULATION_O)
	g++ -fPIC -o bin/Simulation $(SIMULATION_O) $(BASE_O) $(LINKER_FLAGS) $(MY_INCLUDE) $(GL_INCLUDE) $(LIB_FLAGS) -L./bin/lib

### object files - bases ###################################################

base: $(BASE_O)

$(O)graphic.o:           $(BASE)graphic.cpp
	g++ $(MY_INCLUDE) -c $(BASE)graphic.cpp       -o $(O)graphic.o

$(O)graphic_elem.o:      $(BASE)graphic_elem.cpp
	g++ $(MY_INCLUDE) -c $(BASE)graphic_elem.cpp  -o $(O)graphic_elem.o

$(O)picture.o:           $(BASE)picture.cpp
	g++ $(MY_INCLUDE) -c $(BASE)picture.cpp       -o $(O)picture.o

$(O)complex.o:           $(BASE)complex.cpp
	g++ $(MY_INCLUDE) -c $(BASE)complex.cpp       -o $(O)complex.o

$(O)input.o:             $(BASE)input.cpp
	g++ $(MY_INCLUDE) -c $(BASE)input.cpp         -o $(O)input.o

$(O)util.o:              $(BASE)util.cpp
	g++ $(MY_INCLUDE) -c $(BASE)util.cpp          -o $(O)util.o

### object files - simulation #############################################

$(O)simulation_main.o:   $(SIMULATION)simulation_main.cpp
	g++ $(MY_INCLUDE) -c $(SIMULATION)simulation_main.cpp -o $(O)simulation_main.o  $(GL_INCLUDE)

$(O)simulation.o:        $(SIMULATION)simulation.cpp
	g++ $(MY_INCLUDE) -c $(SIMULATION)simulation.cpp      -o $(O)simulation.o

###########################################################################
