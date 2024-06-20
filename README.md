# ASCIINAKE

## Build

```bash
g++ -Wall -Werror src/*.cpp -o asciinake -lncursesw -ltinfo
```

Or

```bash
chmod +x build.bash && ./build.bash
```

## Play/Install

```bash
chmod +x install.bash && sudo ./install.bash
source ~/.zshrc # or .bashrc
asciinake # to play!
```

## Controls

* ⬆️ **UP ARROW**: To move snake up.
* ⬇️ **DOWN ARROW**: To move snake down.
* ⬅️ **LEFT ARROW**: To move snake left.
* ➡️ **RIGHT ARROW**: To move snake right.
* ⏸️ **P**: To pause.
* 🔃 **R**: When the game is over or completed.
* ❎ **ESC**: To leave the game at any time.
