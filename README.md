# Iago

### by Md Abedin & Yiduo Ke (pd 5)

### Description

We made a version of Othello the board game that allows two players to play against each other and chat at the same time. Our game highlights each player's legal moves during their turn, uses terminal colors, has an instant chat, and is nonblocking.

## Required Libraries

None

## Instructions

0. Navigate to the root of the repo, and run

```
$ make
```

1. Make sure your terminal window is maxed, and zoom is at its default (important so graphical errors don't occur), and create 3 tabs with ctrl + Shift + T (do not use tmux because it causes graphical glitches too)

2. In one of your terminal tabs, run 

```
$ ./server
```

In the other two tabs, run 

```
$ ./iago
```

3. Use arrow keys to navigate the board, and press space to place a piece. If it's your turn, your legal moves will be highlighted in yellow. Use Alt + <terminal tab #> to switch tabs. If you have no legal moves, your turn will be skipped. If the board is filled, the winner is the player with more pieces. If a player loses all their pieces, they lose.

- To chat, press c -- even if it's not your turn! Once you start typing a chat, there's no turning back. You must send or quit.

- To quit, press ctrl + c.

## Class Topics Covered

- Allocating memory: for storing user input, loading files into buffers, etc.
- Working with files: reading and displaying the board and chat
- Finding information about files: used stat to determine when chat has been updated
- Signals: sighandler used to catch rage quits and normal quits, cleans up resources used before quitting
- Pipes: named pipes for initial server-client connection, unnamed pipes for transferring move data once game starts
- Shared memory: used to keep track of whose turn it is and let each client know when to start reading for a move from the server

## Known Bugs

- Graphical errors can occur when using terminal settings not specified in instructions
- On some machines, a small weird character is printed below the board
