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

root = tk.Tk()

applicationName = "Application"
MANIFEST_NAME = "Manifest.txt"
destinationFolder = "build"

compileSourcesList = []
compileLibrariesList = []
runningLibrariesList = []
manifestLibList = []
manifestFolders = []
librariesList = []
sourcesList = []

# for java run
def appBegin():
    libFolder = "lib/*.jar"
    global librariesList
    librariesList = glob.glob(libFolder)

    srcFolder = "src/*.java"
    global sourcesList 
    sourcesList = glob.glob(srcFolder)

    if platform.system() == "Windows":
        print("\n", "Formatting for Windows OS", "\n")
        osBreak = ";"
    elif platform.system() == "Darwin":
        print("\n", "Formatting for Mac OS", "\n")
        osBreak = ":"
    else:
        print("\n", "Cannont format, Unknown System", "\n")
    
    global compileSourcesList
    compileSourcesList = osBreak.join(sourcesList)
    global compileLibrariesList
    compileLibrariesList = osBreak.join(librariesList)
    global runningLibrariesList
    runningLibrariesList = compileLibrariesList + osBreak + "build/bin"
    
    print("Source Code: ", "\n\n\t", "\n\t".join(sourcesList), "\n")
    print("Compiling Libraries: ", "\n\n\t",  "\n\t".join(librariesList), "\n")
    print("Running Libraries: ", runningLibrariesList, "\n")

def compileSourceCode():
    print("deleting old bin directory...")
    start_time = time.time()
    if exists("build/bin/" + applicationName + ".class"):
        rmtree("build/bin/")
    print("deleted /bin in %f" % (time.time() - start_time))

    sources = compileSourcesList
    classes = compileLibrariesList

    if not exists("build/bin/"):
        mkdir("build/bin/")
    call(["javac", sources, "-classpath", classes, "-d", destinationFolder + "/bin"])

def run():
    input("Press enter to run...")
    if exists("build/bin/" + applicationName + ".class"):
        call(["java", "-cp", runningLibrariesList, applicationName])
    else:
        print("\nMain class wasnt compiled, end of script.\n")

def createJar():
    
    input("\nPress enter to begin extracting jars...")
    for each in librariesList:
        cutoffJar = str(each)[:-4]
        jarName = cutoffJar[4:]
        direction = "out/" + jarName

        if exists(direction):
            rmtree(direction)
            mkdir(direction)
            print("Removing tree..")
            call(["tar", "xf", each, "-C" , direction])
        else:
            mkdir(direction)
            call(["tar", "xf", each, "-C" , direction])
        manifestFolders.append(jarName + "/")
    
    if exists(MANIFEST_NAME):
        remove(MANIFEST_NAME)
        f = open(MANIFEST_NAME, "wb+")
    else:
        f = open(MANIFEST_NAME, "wb+")

    manifestLibList = " ".join((manifestFolders))
    f.write("Main-Class: " + applicationName + "\n")
    f.write("Class-Path: " + "bin/ " + manifestLibList + "\r\n\r\n")
    f.flush()
    f.close()
    print("Manifest complete...")

    arguments = "-cvfm"
    jar_file_name = destinationFolder + "/" + applicationName + ".jar"
    manifest_file = MANIFEST_NAME
    entry_point = "bin/" + applicationName
    c_dir = "out/"
    call(["jar", arguments, jar_file_name, manifest_file, "-C", c_dir, "."])

# for compiling c++
def compileCpp(projectLocation):
    if platform.system() == "Windows":
        if exists("build/"):
            print("deleting build")
            rmtree("build/")
        call(["D:/compile/winCompile/cmake-3.16.0-rc2-win64-x64/bin/cmake.exe", "-B", "bin", "-G", "Visual Studio 16 2019"], cwd=projectLocation)
        call(["D:/compile/winCompile/cmake-3.16.0-rc2-win64-x64/bin/cmake.exe", "--build", "bin", "--config", "Release"], cwd=projectLocation)
        print("Build Complete")
    elif platform.system() == "Darwin":
        if exists("build/"):
            print("deleting build")
            rmtree("build/")
        call(["cmake -B bin"], cwd=projectLocation)
        call(["cmake --build bin --config Release"], cwd=projectLocation)
        print("Build Complete")
    else:
        print("\n", "Cannont format, Unknown System", "\n")

# the command that compiles the code based off of what radio button is clicked
def compile(projectLocation):
    if v.get() == 1:
        appBegin()
        compileSourceCode()
        run()
    elif v.get() == 2:
        appBegin()
        compileSourceCode()
        createJar()
    elif v.get() == 3:
        compileCpp(projectLocation)
    else:
        print("no choice made")

v = tk.IntVar()

a = """
_______ _______  ______        _______  ______ _______ _______ _     _
|          |    |_____/ |      |______ |_____/ |______ |______ |____/ 
|_____     |    |    \_ |_____ |       |    \_ |______ |______ |    \_
"""
a = a.strip()

style = ttk.Style()
style.configure("Fixed.TLabel", font='TkFixedFont')
headerLabel = ttk.Label(root, text=a, justify = tk.LEFT, style="Fixed.TLabel").pack()
radio1 = tk.Radiobutton(root, text="Java run", variable=v, value=1).pack()
radio2 = tk.Radiobutton(root, text="Java .jar", variable=v, value=2).pack()
radio3 = tk.Radiobutton(root, text="C++", variable=v, value=3).pack()
newDirectory = tk.StringVar()
newDirectory.set("Select Project Folder")
directoryLabel = ttk.Label(root, textvariable=newDirectory, justify = tk.CENTER, style="Fixed.TLabel").pack()

def askForDirectory():
    newDirectory.set(filedialog.askdirectory())

directoryButton = tk.Button(root, text="Select Project", justify = tk.CENTER, padx = 20, pady = 20, command=askForDirectory).pack()
compileButton = tk.Button(root, text="Compile", justify = tk.CENTER, padx = 20, pady = 20, command= lambda: compile(newDirectory)).pack()
root.mainloop()