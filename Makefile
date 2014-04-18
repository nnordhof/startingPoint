CC=g++
CFLAGS=-g -c -DGL_GLEXT_PROTOTYPES -lGL -lGLU -lglfw
LDFLAGS=-DGL_GLEXT_PROTOTYPES -lGL -lGLU -lglfw
SOURCES=Rendering/GLSL_helper.cpp Modeling/CMeshLoaderSimple.cpp Modeling/CMesh.cpp GameObject.cpp Modeling/mesh.cpp Game.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=smorgasfjord

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -g $(LDFLAGS) $(OBJECTS) -o $@

&.cpp&.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *.o $(EXECUTABLE)

