# COMP4985 Test Server
This program acts as a 'server' to test packets sent from a chat client program.

This program will print the contents of the decoded packet as well as send a success or error response.

## Installation

Build using Make:
```sh
make build
```

Build using D'Arcy's build system:
1. Create links to his files
```sh
./create-links.sh <work/programming101dev/scripts/>
```
2. Generate the CMakefiles
```sh
./generate-cmakelists.sh
```
3. Build
```sh
./build-all.sh
```

## Usage

```sh
./build/server
```
