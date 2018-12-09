all: dirs client server

dirs:
	mkdir -p bin

server: dirs 
	make -C server all

client: dirs
	make -C client all

clean:
	rm -rf */*.o bin 
