CC = gcc
CFLAGS = -O2 -Wall -lpthread
SRC = main_coordinator.c src/state_controller.c src/file_one_voip_ingress.c src/file_two_filtration.c src/file_three_voip_egress.c
OBJ = engine_system

all:
	$(CC) $(SRC) $(CFLAGS) -o $(OBJ)

clean:
	rm -f $(OBJ) *.bin *.log
