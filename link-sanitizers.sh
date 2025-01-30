#!/usr/bin/env bash

# Function to display script usage
usage()
{
    echo "Usage: $0 <target_file>"
    echo "  <target_file> - The file to which sanitizers.txt will point"
    exit 1
}

# Check if exactly one argument is provided
if [ "$#" -ne 1 ]; then
    usage
fi

target_path=$1

# Verify that the target directory exists
if [ ! -f "$target_path" ]; then
    echo "Error: Target file '$target_path' does not exist."
    exit 1
fi

# If a symbolic link or file already exists as sanitizers.txt, remove it
if [ -L "sanitizers.txt" ]; then
    echo "Removing existing symbolic link sanitizers.txt"
    rm .flags
elif [ -f "sanitizers.txt" ]; then
    echo "Error: A file named sanitizers.txt already exists. Please remove or rename it before proceeding."
    exit 1
fi

# Create the symbolic link
ln -s "$target_path" sanitizers.txt

echo "Symbolic link created: sanitizers.txt -> $target_path"
