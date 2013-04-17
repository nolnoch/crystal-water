###########################################################
# Crystal-Water Makefile

CC = g++
CFLAGS = 
CFLAGS = -ggdb -g
#CFLAGS = -g
INCLUDE =
#INCLUDE = -I/lusr/X11/include -I/lusr/include
LIBDIR =
#LIBDIR = -L/lusr/X11/lib -L/lusr/lib
# Libraries that use native graphics hardware --
# appropriate for Linux machines in Taylor basement
#LIBS = -lglut -lGLU -lGL -lpthread -lm
LIBS = -lGLEW -lGL -lglut -lSOIL -lOpenCL -lassimp

###########################################################
# Options if compiling on Mac
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
CC = g++
CFLAGS = -Wall -g -D__MAC__
INCLUDE = -I/opt/local/include
LIBDIR = -L/lusr/X11/lib -L/opt/local/lib
LIBS = -framework OpenGL -framework GLUT -ljpeg
endif

###########################################################
# Uncomment the following line if you are using Mesa
#LIBS = -lglut -lMesaGLU -lMesaGL -lm

crystal: main.o quaternion.o shaderobj.o program.o mesh.o
	${CC} ${CFLAGS} $(INCLUDE) -o crystal main.o quaternion.o shaderobj.o program.o mesh.o ${LIBDIR} ${LIBS}

main.o: main.cpp program.hpp shaderobj.hpp quaternion.hpp helper.hpp
	${CC} ${CFLAGS} -c -o main.o $(INCLUDE) main.cpp
	
quaternion.o: quaternion.cpp quaternion.hpp
	${CC} ${CFLAGS} -c -o quaternion.o $(INCLUDE) quaternion.cpp
	
shaderobj.o: shaderobj.cpp shaderobj.hpp
	${CC} ${CFLAGS} -c -o shaderobj.o $(INCLUDE) shaderobj.cpp
	
program.o: program.cpp program.hpp
	${CC} ${CFLAGS} -c -o program.o $(INCLUDE) program.cpp

mesh.o: mesh.cpp mesh.hpp
	${CC} ${CFLAGS} -c -o mesh.o $(INCLUDE) mesh.cpp

clean:
	rm -f crystal *.o
	
# add the following lines
# myfile.o: myfile.cpp myfile.h # any additional dependencies
#	 ${CC} -c ${CFLAGS} $(INCLUDE) myfile.cpp
