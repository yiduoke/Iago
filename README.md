# Iago

### by Md Abedin & Yiduo Ke (pd 5)

### Description

We made a version of Othello the board game that allows two players to play against each other and chat at the same time. Our game highlight's each player's legal moves during their turn, uses terminal colors, has an instant chat, and there's no blocking.

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

3. Use arrow keys to navigate the board, and press enter to place a piece. If it's your turn, your legal moves will be highlighted in yellow. Use Alt + <terminal tab #> to switch tabs.

4. To chat, press c -- even if it's not your turn! Once you start typing a chat, there's no turning back. You must send or quit.

5. If you have no legal moves, your turn will be skipped. If the board is filled, the winner is the player with more pieces. If a player loses all their pieces, they lose.

6. To quit, press ctrl + c.
