"""

CtrlFreek.py is a script that allows the user to automate the process of compiling with 100% customization.

If this script is to be used "out of the box" without any editing, it assumes one or two things:

    1. Python 3 or above is installed and being used to run this script
    2. The proper folder structure is being used (for an example of this file structure, refer to the README.MD)
    3. Oracle's JVM and JRM are installed and PATH'd correctly
    4. If it is a Windows system, Visual Studio and its C++ compiling tools are installed and PATH'd correctly
    5. If it is a Mac system, Xcode and its commandlinetools as well as C++ compilling tools are installed and PATH'd correctly
 
 If these are not the case then you will have to edit the script accordingly before running it. (If you break it, not my fault.)

ATTENTION: The call to CMake in compileCpp() points to CMake as it is installed on my USB drive. Change this to point to your
           CMake install path (or just use the command if it is added to your PATH)

I AM NOT RESPONSIBLE FOR ANYTHING YOU CHOOSE TO DO WITH THIS SCRIPT.

CtrlFreek - 0.1.5
Anthony Mesa 

"""

import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
from subprocess import call
from os import remove
from os import rmdir
from shutil import rmtree
from os import mkdir
from os.path import exists
import time
import platform
import glob

# Initialize tKinter
root = tk.Tk()

# Set variables for Java run/compile
applicationName = "Application"
MANIFEST_NAME = "Manifest.txt"
destinationFolder = "build/"

compileSourcesList = []
compileLibrariesList = []
runningLibrariesList = []
manifestLibList = []
manifestFolders = []
librariesList = []
sourcesList = []

def appBegin(projectLocation):

    """
    Sets up variables before Java compiling begins
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """

    # Sets variable representing folder of libraries we want to glob
    libFolder = projectLocation + "/lib/*.jar"
    global librariesList
    librariesList = glob.glob(libFolder)

    # Sets variable representing folder of source code we want to glob
    srcFolder = projectLocation + "/src/*.java"
    global sourcesList 
    sourcesList = glob.glob(srcFolder)

    # Depending on the OS, seperate list of bin's with : or ;
    if platform.system() == "Windows":
        print("\n", "Formatting for Windows OS", "\n")
        osBreak = ";"
    elif platform.system() == "Darwin":
        print("\n", "Formatting for Mac OS", "\n")
        osBreak = ":"
    else:
        print("\n", "Cannont format, Unknown System", "\n")
    
    # Create source list and library lists
    global compileSourcesList
    compileSourcesList = osBreak.join(sourcesList)
    global compileLibrariesList
    compileLibrariesList = osBreak.join(librariesList)
    global runningLibrariesList
    runningLibrariesList = compileLibrariesList + osBreak + projectLocation + "/build/bin"
    
    # Display source and libraries
    print("Source Code: ", "\n\n\t", "\n\t".join(sourcesList), "\n")
    print("Compiling Libraries: ", "\n\n\t",  "\n\t".join(librariesList), "\n")
    print("Running Libraries: ", runningLibrariesList, "\n")

def compileSourceCode(projectLocation):
    
    """
    Compile Java source code by calling Javac
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """

    # If bin directory from prior compile exists, delete
    print("deleting old bin directory...")
    start_time = time.time()
    if exists("bin/" + applicationName + ".class"):
        rmtree("bin/")
    print("deleted bin in %f" % (time.time() - start_time))

    # Sets sources and classes (for readability's sake)
    sources = compileSourcesList
    classes = compileLibrariesList

    # Creates bin folder if there is none and calls javac
    if not exists("bin/"):
        mkdir("bin/")
    call(["javac", sources, "-classpath", classes, "-d", "bin/"], cwd=projectLocation)

def run(projectLocation):
    
    """
    Run previously compiled Java program by calling command of same name
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """

    # If the main class exists, run the Java class
    input("Press enter to run...")
    if exists("bin/" + applicationName + ".class"):
        call(["java", "-cp", runningLibrariesList, applicationName], cwd=projectLocation)
    else:
        print("\nMain class wasnt compiled, end of script.\n")

def createJar(projectLocation):

    """
    Create Java Executable JAR

    WILL NOT INCLUDE EXTERNAL C DEPENDENCIES SUCH AS JAVA NATIVE LIBRARY CODE
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """

    input("\nPress enter to begin extracting jars...")

    # Takes all libraries and 
    for each in librariesList:
        cutoffJar = str(each)[:-4]
        jarName = cutoffJar[4:]
        direction = "out/" + jarName

        if exists(direction):
            rmtree(direction)
            mkdir(direction)
            print("Removing tree..")
            call(["tar", "xf", each, "-C" , direction], cwd=projectLocation)
        else:
            mkdir(direction)
            call(["tar", "xf", each, "-C" , direction], cwd=projectLocation)
        
        # Add the libraries inside of the Executable JAR
        manifestFolders.append(jarName + "/")
    
    # Checks if Manifest file exists, deletes it and creates a new one
    if exists(MANIFEST_NAME):
        remove(MANIFEST_NAME)
        f = open(MANIFEST_NAME, "wb+")
    else:
        f = open(MANIFEST_NAME, "wb+")

    # Generates info for Manifest file
    manifestLibList = " ".join((manifestFolders))
    f.write("Main-Class: " + applicationName + "\n")
    f.write("Class-Path: " + "bin/ " + manifestLibList + "\r\n\r\n")
    f.flush()
    f.close()
    print("Manifest complete...")

    arguments = "-cvfm"
    jar_file_name = destinationFolder + "/" + applicationName + ".jar"
    manifest_file = MANIFEST_NAME
    c_dir = "out/"
    # entry_point = "bin/" + applicationName
    
    # Runs JAR command to archive project using arguments set above
    call(["jar", arguments, jar_file_name, manifest_file, "-C", c_dir, "."], cwd=projectLocation)

def compileCpp(projectLocation):

    """
    Compiles C++ code by running CMake
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """

    # If folders exist from previous build, delete
    if exists(projectLocation + "/build"):
        print("deleting build")
        rmtree(projectLocation + "/build")
    if exists(projectLocation + "/bin"):
        print("deleting bin")
        rmtree(projectLocation + "/bin")

    # Chooses appropriate operating system
    if platform.system() == "Windows":

        # call CMake
        call(["D:/compile/winCompile/cmake-3.16.0-rc2-win64-x64/bin/cmake.exe", "-B", "bin", "-G", "Visual Studio 16 2019"], cwd=projectLocation)
        call(["D:/compile/winCompile/cmake-3.16.0-rc2-win64-x64/bin/cmake.exe", "--build", "bin", "--config", "Release"], cwd=projectLocation)

    elif platform.system() == "Darwin":

        call(["cmake", "-B", "bin", "-G", "Xcode"], cwd=projectLocation)
        call(["cmake", "--build", "bin", "--config", "Release"], cwd=projectLocation)

    else:
        print("\n", "Cannont format, Unknown System", "\n")

def compile(projectLocation):
    
    """
    Compiles C++ code by running CMake
     
    Keyword Arguments:
    projectLocation -- String containing the desired project working directory
    """
    
    # If user has chosen run Java, compile and run
    if v.get() == 1:
        appBegin(projectLocation)
        compileSourceCode(projectLocation)
        run(projectLocation)

    # if user has chosen archive Java, compile and archive
    elif v.get() == 2:
        appBegin(projectLocation)
        compileSourceCode(projectLocation)
        createJar(projectLocation)

    # if user has chosen compile C++, compile and build
    elif v.get() == 3:
        compileCpp(projectLocation)

    else:
        print("no choice made")

#==================================================================================

"""
Where the program begins, Draws the tKinter window
"""

a = """
_______ _______  ______        _______  ______ _______ _______ _     _
|          |    |_____/ |      |______ |_____/ |______ |______ |____/ 
|_____     |    |    \_ |_____ |       |    \_ |______ |______ |    \_
"""
a = a.strip()
v = tk.IntVar()
style = ttk.Style()

# Configures the label to a monospace font
style.configure("Fixed.TLabel", font='TkFixedFont')
headerLabel = ttk.Label(root, text=a, justify = tk.LEFT, style="Fixed.TLabel").pack()

# Radio buttons choose which action to make
radio1 = tk.Radiobutton(root, text="Java run", variable=v, value=1).pack()
radio2 = tk.Radiobutton(root, text="Java .jar", variable=v, value=2).pack()
radio3 = tk.Radiobutton(root, text="C++", variable=v, value=3).pack()

def askForDirectory():

    """
    When directory button is pressed, prompts user for directory
    """
    newDirectory.set(filedialog.askdirectory())

# Initialize newDirectory variable and set directory label
newDirectory = tk.StringVar()
newDirectory.set("Select Project Folder")
directoryLabel = ttk.Label(root, textvariable=newDirectory, justify = tk.CENTER, style="Fixed.TLabel").pack()

# Directory and Compile buttons
directoryButton = tk.Button(root, text="Select Project", justify = tk.CENTER, padx = 20, pady = 20, command=askForDirectory).pack()
compileButton = tk.Button(root, text="Compile", justify = tk.CENTER, padx = 20, pady = 20, command= lambda: compile(newDirectory.get())).pack()

# tKinter UI loop
root.mainloop()

