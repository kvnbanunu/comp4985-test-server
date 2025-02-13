build: clean
	@mkdir -p build
	@gcc src/server.c src/setup.c src/asn.c -o build/server

clean:
	@rm -rf build

run:
	@./build/server
