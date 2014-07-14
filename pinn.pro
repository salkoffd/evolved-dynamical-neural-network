######################################################################
# Automatically generated by qmake (3.0) Fri May 23 02:13:49 2014
######################################################################

TEMPLATE = app
#TARGET = pinn
#INCLUDEPATH += . src/tasks src/testing src/brain src/evolution  ###DEL
INCLUDEPATH += .
CONFIG += c++11
QMAKE_CXXFLAGS += -pedantic


# Input
HEADERS += src/brain/Brain.hpp \
			 src/brain/Neuron.hpp \
			 src/tasks/MazeTask.hpp \
			 src/tasks/TaskInterface.hpp \
			 src/testing/MazeTest.hpp \
	src/testing/BrainTest.hpp \
	src/evolution/Evolution.hpp \
	src/testing/EvolutionTest.hpp \
	src/Globals.hpp \
	src/gamemaster/GameMaster.hpp \
		src/testing/GameMasterTest.hpp
	
SOURCES += src/main.cpp \
			 src/brain/Brain.cpp \
			 src/brain/Neuron.cpp \
			 src/tasks/MazeTask.cpp \
			 src/testing/MazeTest.cpp \
	src/testing/BrainTest.cpp \
	src/evolution/Evolution.cpp \
	src/testing/EvolutionTest.cpp \
	src/Globals.cpp \
	src/gamemaster/GameMaster.cpp \
		src/testing/GameMasterTest.cpp
