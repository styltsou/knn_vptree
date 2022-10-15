CC = gcc

sequential: 
	$(CC) -O3 sequential.c  utils.c queue.c knn.c -lm -o sequential
	
parallel: 
	$(CC) -O3 parallel.c  utils.c queue.c knn.c -lm -lpthread -o parallel

clean:
	rm sequential parallel