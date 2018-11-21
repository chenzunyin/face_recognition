CXX = g++
CFLAGS = -Wall -I./include -std=gnu++11
LDFLAGS_CV += `pkg-config --cflags --libs opencv`  -lopencv_highgui

FACE_DB = ./database/czy_face.db
OUTPUT = ./output/
FUNC = ./func/

SINGLE = $(FUNC)add_single_person.cpp
SINGLE_OUT = $(OUTPUT)add_single_person
COMPARE = $(FUNC)compare_feature.cpp
COMPARE_OUT = $(OUTPUT)compare_feature
IDENTIFY = $(FUNC)identify_from_database.cpp
IDENTIFY_OUT = $(OUTPUT)identify_from_database
READFILE = $(FUNC)read_dirent.cpp
READFILE_OUT = $(OUTPUT)read_dirent
CYCLE = $(FUNC)cycle_register_person.cpp
CYCLE_OUT = $(OUTPUT)cycle_register_person

all: single compare identify read cycle

single:
	$(CXX) $(CFLAGS) -o $(SINGLE_OUT) $(SINGLE) -L ./lib -lReadFace -lopenblas $(LDFLAGS_CV)
compare:
	$(CXX) $(CFLAGS) -o $(COMPARE_OUT) $(COMPARE) -L ./lib -lReadFace -lopenblas $(LDFLAGS_CV)
identify:
	$(CXX) $(CFLAGS) -o $(IDENTIFY_OUT) $(IDENTIFY) -L ./lib -lReadFace -lopenblas $(LDFLAGS_CV)
read:
	$(CXX) $(CFLAGS) -o $(READFILE_OUT) $(READFILE) -L ./lib -lReadFace -lopenblas $(LDFLAGS_CV)
cycle:
	$(CXX) $(CFLAGS) -o $(CYCLE_OUT) $(CYCLE) -L ./lib -lReadFace -lopenblas $(LDFLAGS_CV)

.PHONY:clean

clean:
	-rm -rf $(FACE_DB) $(OUTPUT)*
