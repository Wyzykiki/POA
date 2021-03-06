.SUFFIXES:
.SUFFIXES:      .cc .o .h

FMOD		= fmod/4.3
MFMOD		= fmod/Developer/FMOD\ Programmers\ API\ Mac/api/inc

O			= Labyrinthe.o Chasseur.o Gardien.o

LIBS		= $(FMOD)/libfmodex64.so -lglut -lGLU -lGL jpeg/libjpeg.a -lm
MLIBS		= -framework GLUT -framework OpenGL -framework Foundation jpeg/libmacjpeg.a \
			  /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib

CXX			= g++ -g
CXXFLAGS	= -I $(FMOD) -O0 -Wall

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

labh:	$(O)
	$(CXX) -o $@ $(O) OpenGL64.o $(LIBS)

maclabh:	$(O)
	$(CXX) -o $@ MacOpenGL.o $(O) $(MLIBS)
	install_name_tool -change ./libfmodex.dylib /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib $@

clean:
	@rm -f labh maclabh $(O)

Labyrinthe.o:	Labyrinthe.h Chasseur.h Gardien.h
Chasseur.o:		Chasseur.h
Gardien.o:		Gardien.h
