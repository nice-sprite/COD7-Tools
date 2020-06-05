# COD7-Tools
In-Game tool set featuring: 
* developer console (provides access to built-in debug overlays and outputs)
* map selector
* FPS counter
* FOV adjuster/slider
* game debug log
* aimbot
* esp
* rapidfire
* no recoil
* unlimited ammo. 

# Dependencies
VcPkg is used for all dependencies, it's awesome so I use it whenever I can. 
* JSON for Modern C++ https://github.com/nlohmann/json
* ImGui https://github.com/ocornut/imgui
* Capstone Disassembler https://github.com/aquynh/capstone

## Planned features ##
* Improved log window with dump, search, and color highlighting
* Improved console with tab completion, additional custom commands, and color highlighting
* Call Of Duty script (.gsc) toolset that would allow the modification and re-injection of .gsc scripts while still in the game
* Model/Asset viewer (?) may be implemented sometime in the distant future.
* Ports to other COD games! 
* DVAR editor
* Interface customization
* Keybind system to allow for even quicker use of the tools/hacks

# Screenshots
These images showcase the current state of the tool such as the FPS counter and graph 

![](/images/screenshot1.png)
![](/images/screenshot2.png)

# Code features
Just about everything that isn't dllmain.cpp can be reused or packaged as a standalone library for your own projects. 
* JSON for Modern C++ used to parse commands.json
* python(3.x) script included for formatting a list of commands into commands.json (there can be thousands of console commands)
* Generic Hooking library utilizing Capstone disassembler
* ImGui interface
* DirectX Hooking library
* Custom ImGuiConsole widget is modular, reusable, and easy to use. 
* CoD Engine structures and functions are included in their own files

# Sidenotes
Some features are still in progress, but I have a lot planned for this project and hope to take it in a direction that allows people to have fun with different games, better understand how they work, and easily extend the existing features for their own games. ImGui looks great and is easy to use, however it feels a little lazy to me to use something so "large" as a base for my own project. It makes my own code feel small in comparison. I have already started work on a GUI library of my own, but implementing a good looking, easy to use, high performance, customizable, and small (in terms of LoC) GUI Library is a time consuming and difficult task. I hope in the future to be able to write a tactile GUI library that is as good if not better than ImGui (at least in terms of looks) that does something unique and feels good while both writing it and using it. 

Thanks for checking out my project, I hope you enjoy looking through it!
