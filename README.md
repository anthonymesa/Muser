# Intent

Muser is a Linux/Windows software that converts 3D shape into sound via .Wav file format (or in app playback). The end goal for this application is to provide a means of 'tactile sound synthesis' via 3D sculpting.

Currently, I am going through a refactoring for this project to switch it over to Visual Studio, where before I was editing CMakeList.txt and compiling this program by hand (that was the original driving factor behind creating the abandoned project 'ControlFreek' in my repository).

These are the features that will be available once the proof of concept is complete:

## Planned Features

* Loading of .obj models
* 3D Render multiple 'muse' objects simultaneously via direct OpenGl
* Wave sound export of topographical data
* PPM image export of topographical data
* OpenGL powered display of objects in 3D space
* HotKey based command input

Once these basic features are implemented in a working version, I would like to take the software further:

## Future UnPlanned Features

* GUI using ImGui
* Real time sculpting of 'muse' model
* Application of RGB material, where RGB hue affects sin wave type (sin, square, sawtooth), and RGB Brightness affects maximum Hz range.
* Move to Unity or Unreal Engine to take advantage of VR framework

## Libraries Used

* OpenGl
* GLFW
* GLEW
* math_3D
* stb_image
* AudioFile
* SFML (future)
