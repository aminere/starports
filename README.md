## Starports Space Defense

![Thumbnail](https://franticsoftware.com/public/starports/4.jpg)
![Thumbnail2](https://franticsoftware.com/public/starports/3.jpg)

This is the full C++ source code to <a href="https://franticsoftware.com/#/starports">Starports Space Defense</a>, a mobile 3D tower defense released in 2015.

In addition to the game source, this includes the full game pipeline (engine, editor, and tools). 
This engine is a continuation of <a href="https://franticsoftware.com/#/vlad">VLAD Heavy Strike</a>'s engine.

## How To Build (Windows 10)

* Requires Visual Studio 2019 :warning:
* Run `BuildEditor.bat` or `BuildGame.bat`
* Or build from the solution files `EditorLauncher.sln` or `GameLauncher.sln`  

## How To Run (Windows 10)
* Run `LaunchEditor.bat` to launch the editor
  * Open `Game/data/menu/touchtostartscene.xml` to play the game in the editor
* Run `LaunchGame.bat` to launch the game
  
## How To Debug (Windows 10)
* From `EditorLauncher.sln`
  * Ensure the working directory is `Game`
  * Hit Play
  
* From `GameLauncher.sln`
  * Ensure `Shoot\tools\FileSystemGenerator\vc\FileSystemGenerator.sln` was built
  * Ensure game data was built by running `Game/FileSystemGenerator.bat`
  * Ensure the working directory is `Game`
  * Hit Play

## Engine Features

Low Level:

+ Cross platform targets (PC / iOS / Android / Windows Phone 8)<br>
+ 3D rendering using OpenGL, GLES2.0, and DirectX11<br>
+ Memory allocation tracking (full leak & allocation reporting)<br>
+ 3D Math library<br>
+ File system abstraction<br>
+ Network socket abstraction<br>
+ Multi-threading abstraction<br>
+ Touch and Key input abstraction<br>
+ FBX and PNG support through fbxsdk and libpng<br>
+ OGG and WAV support through cricket audio<br>
+ Event system<br>

High Level:

+ Object Model framework, with serialization, ref. counting, weak handles, and polymorphism support<br>
+ Entity Component System (ECS)<br>
+ Resource abstraction model
+ Generic entity hierarchy handling allowing to have 3D or 2D hierarchies, or both in the same tree<br>
+ Quad-tree partitioning<br>
+ Collision handling<br>
+ Context stacking and switching<br>

Tool Side:
+ WYSIWYG Game Editor for Game and Level design<br>
+ The game can be played in the editor and game contexts can be inspected at run-time<br>
+ ObjectViewer tool, allows to inspect all alive objects at any given time<br>
+ DependencyViewer tool that displays reference and dependency information across objects<br>
+ Data packaging tool for optimal data loading at runtime<br>

