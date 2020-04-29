NAME = MMAS
compile:
	g++ -latomic -pthread -std=c++17 *.cpp -o $(NAME)
debug:
	g++ -latomic -pthread -g -std=c++17 *.cpp -o $(NAME).g
sample_run:
	./$(NAME) -nmN 4 16 10 -ABGvQ 1 50 0.5 0.005 5 --tmin 0.25 --tmax 5 -M
best:
	tac mmas*.out | grep -B 10 -m 1 -P '\t'

