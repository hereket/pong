# Software rendered Pong game
This is a pong game currently working only in macOS. It is software-rendered
from scratch. The reason for this is project is to practice 'raw' programming for
macOS.
![sss](/_images/game1.png)


## External code
Even though this project is mostly built from scratch it uses these to help in
development:
* SDL2 - for managing opening and closing windows in macOS.
* Ogg Vorbis audio decoder - v1.20 from Sean Barrett.
In the future, I will probably switch to direct window management in obj-c.


## How to use:
Run it with
```
$ cd src
$ ./build.sh
$ cd ../build
$ ./game
```

## Rebuild assets:
All assets are stored in the 'data' folder. In the same data folder, there is
the 'data.pack' file which is the only file used by the game to load assets. If
you change any resource files you will need to 'cook' it again (rebuild). To do
this, follow these steps from the project root folder.
```
$ cd ./build
$ ./cooker
```
That is it.


## Requirements
If you don't have SDL, then install it with
```
brew install SDL2
```

## Credit
This project is heavily based on Dan Zaidan's ["Making a game in C from
scratch"](https://www.youtube.com/playlist?list=PL7Ej6SUky1357r-Lqf_nogZWHssXP-hvH)
youtube videos. He has done a great job, go check them out. Unfortunately, this
project is not one-to-one mapping to his code for macOS. This is my
interpretation of his game design ideas and if you want to follow his videos but
on macOS, this code will not be useful. But you can take my macOS layer code and
his source code for the game and with little modification, it should work just
fine.
