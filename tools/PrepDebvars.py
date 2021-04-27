import fileinput
import subprocess

filename = "C:\\gamedev\\PlatformerMD\\out\\symbol.txt"




with fileinput.FileInput(filename, inplace = True, backup ='.bak') as f:
    for line in f:
        if("deb" in line):
            print(line)
        else:
            print("") 


# os.startfile("C:\\gamedev\\SGDK\\bin\\nm2wch.exe C:\\gamedev\\PlatformerMD\\out\\symbol.txt C:\\gamedev\\PlatformerMD\\out\\rom.wch")


subprocess.call(['C:\\gamedev\\SGDK\\bin\\nm2wch.exe', 'C:\\gamedev\\PlatformerMD\\out\\symbol.txt', 'C:\\gamedev\\PlatformerMD\\out\\rom.wch'])
subprocess.call(['C:\\games\\emulators\\gensKMod_073\\gens.exe', 'C:\\gamedev\\PlatformerMD\\out\\rom.bin'])
