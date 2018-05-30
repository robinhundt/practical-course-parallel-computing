CC = g++

OSTYPE = $(shell uname -s)
ifeq ($(OSTYPE), Darwin)
  INCLUDES = -I/Library/Frameworks/SDL.framework/Versions/A/Headers/
  LIBS = -lSDLmain -lSDL
  FRAMEWORKS = -framework SDL -framework OpenGL -framework Cocoa
else
  INCLUDES =  `sdl-config --cflags`
  LIBS = `sdl-config --libs` -lGL -lGLU -lm
  FRAMEWORKS =
endif

default: visualize visualize_conduct

visualize: visualize.cpp
	$(CC) $< $(INCLUDES) $(FRAMEWORKS) $(LIBS) -o $@
	
visualize_conduct: visualize_conduct.cpp
	$(CC) $< $(INCLUDES) $(FRAMEWORKS) $(LIBS) -o $@ 

.PHONY: clean
clean:
	-rm visualize
	-rm visualize_conduct

