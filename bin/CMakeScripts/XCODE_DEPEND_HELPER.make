# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.Muser.Debug:
/Users/anthonymesa/Desktop/muser-build/build/mac/Debug/Muser:\
	/Users/anthonymesa/Desktop/muser-build/lib/GL/glew32s.lib\
	/Users/anthonymesa/Desktop/muser-build/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a
	/bin/rm -f /Users/anthonymesa/Desktop/muser-build/build/mac/Debug/Muser


PostBuild.Muser.Release:
/Users/anthonymesa/Desktop/muser-build/build/mac/Release/Muser:\
	/Users/anthonymesa/Desktop/muser-build/lib/GL/glew32s.lib\
	/Users/anthonymesa/Desktop/muser-build/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a
	/bin/rm -f /Users/anthonymesa/Desktop/muser-build/build/mac/Release/Muser


PostBuild.Muser.MinSizeRel:
/Users/anthonymesa/Desktop/muser-build/build/mac/MinSizeRel/Muser:\
	/Users/anthonymesa/Desktop/muser-build/lib/GL/glew32s.lib\
	/Users/anthonymesa/Desktop/muser-build/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a
	/bin/rm -f /Users/anthonymesa/Desktop/muser-build/build/mac/MinSizeRel/Muser


PostBuild.Muser.RelWithDebInfo:
/Users/anthonymesa/Desktop/muser-build/build/mac/RelWithDebInfo/Muser:\
	/Users/anthonymesa/Desktop/muser-build/lib/GL/glew32s.lib\
	/Users/anthonymesa/Desktop/muser-build/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a
	/bin/rm -f /Users/anthonymesa/Desktop/muser-build/build/mac/RelWithDebInfo/Muser




# For each target create a dummy ruleso the target does not have to exist
/Users/anthonymesa/Desktop/muser-build/lib/GL/glew32s.lib:
/Users/anthonymesa/Desktop/muser-build/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a:
