all: dirs client server raw

dirs:
	mkdir -p bin

server: dirs 
	make -C server all

client: dirs
	make -C client all

raw: dirs
	make -C raw all

clean:
	rm -rf */*.o bin 
