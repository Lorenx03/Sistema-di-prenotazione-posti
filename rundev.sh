#!/bin/bash

# This script requires tmux to be installed.
# To install tmux, run the following command:
# sudo apt-get install tmux
# For MacOS -> brew install tmux

# Ensure tmux is installed
if ! command -v tmux &> /dev/null; then
    echo "Error: tmux is not installed. Install it and try again."
    exit 1
fi

# Check if a Makefile exists in the current directory
if [ ! -f "Makefile" ]; then
    echo "Error: No Makefile found in the current directory."
    exit 1
fi

# Compile the project using make
echo "Compiling the project using make..."
if ! make; then
    echo "Error: Compilation failed."
    exit 1
fi

# Ensure the executables exist
if [ ! -x "./server" ]; then
    echo "Error: ./server executable not found or not executable."
    exit 1
fi

if [ ! -x "./client" ]; then
    echo "Error: ./client executable not found or not executable."
    exit 1
fi

# Tmux session name
SESSION_NAME="server_client"

# Function to clean up tmux session on SIGINT
cleanup() {
    echo "Cleaning up..."
    # Kill all tmux processes in the session
    tmux send-keys -t $SESSION_NAME:0 C-c
    tmux send-keys -t $SESSION_NAME:1 C-c
    tmux kill-session -t $SESSION_NAME 2>/dev/null
    exit 0
}

# Trap SIGINT (Ctrl+C) to execute cleanup
trap cleanup SIGINT

# Kill any existing session with the same name
tmux kill-session -t $SESSION_NAME 2>/dev/null

# Create a new tmux session
tmux new-session -d -s $SESSION_NAME -n server "./server"

# Split the pane and run the client
tmux split-window -h -t $SESSION_NAME "./client"

# Resize panes: server occupies 1/3, client 2/3
tmux resize-pane -t $SESSION_NAME:0 -x $((2 * $(tmux display -p "#{window_width}") / 3))

# Attach to the tmux session
tmux attach-session -t $SESSION_NAME

# If the session ends or detaches, ensure cleanup is performed
cleanup
