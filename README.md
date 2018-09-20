# StarportsPublic

<b>What is this repository about?</b>

This is the full source code to <a href="http://www.franticsoftware.com/starports">Starports Space Defense</a>, a mobile 3D tower defense I made and released throughout 2014/2015

In addition to the game source, this includes the full game pipeline (engine, editor, and tools), which I developed from scratch on my free time, starting around January 2010. This engine is a continuation of <a href="http://www.franticsoftware.com/vlad">VLAD Heavy Strike</a>'s engine.

<b>Why make a game engine from scratch?</b>

I've been interested in game development technology for a long time, even more than playing or making games. I made my university's thesis project about a <a href="https://docs.google.com/viewer?a=v&pid=sites&srcid=ZGVmYXVsdGRvbWFpbnxhbWluZXJlaGlvdWl8Z3g6NzQxZDllNWVlMzNkOTIzNQ">Game Development Environment</a>. 

<b>Non exhaustive list of Engine Features</b>

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

