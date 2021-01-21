import json
import os

def ReadLevel(data, accumnavblock):
    jdata = json.load(data)

    #tiled layers need to follow this order: 1- image, 2- floor 3- wall 4-...
    floorlayer = jdata['layers'][1]['objects']
    print(jdata['layers'][1]['objects'][2])
    walllayer = jdata['layers'][2]['objects']
    roomlayer = jdata['layers'][3]['objects']
    numFloors = len(floorlayer)
    print(numFloors)
    numWalls = len(walllayer)
    numRooms = len(roomlayer)
    rooms=[]
    navblocks = []

    #moveblocks: type, x, y, length
    for i in range(numFloors):
        t = 0
        print(floorlayer[i]['type'])
        if (floorlayer[i]['type']== 'oneway'):
            t=1
        x = floorlayer[i]['x']#/16.0
        y = (floorlayer[i]['y'])#/56.0) -1.0
        w = floorlayer[i]['width']#/16.0 
        h = floorlayer[i]['height']#/16.0 
        newfloor = [t, x, y, w, h]
        navblocks.append(newfloor)
    
    for i in range(numWalls):
        x = walllayer[i]['x']/16.0
        y = (walllayer[i]['y']/56.0) -1.0
        w = walllayer[i]['width']/16.0 -1
        newwall = [1, x, y, w]
        navblocks.append(newwall)
    
    organizednavblock=[]
    curblockind = 0
    lwidth = 0
    lheight = 0
    for i in range(numRooms):
        x = roomlayer[i]['x']/16.0
        y = ((roomlayer[i]['y']-224)/56.0) 
        curw = x + 20
        curh = y + 4
        if (curw > lwidth):
            lwidth = curw
        if (curh > lheight):
            lheight = curh

        blocklength = 0
        for mb in navblocks:
            if(mb[1]>=x and mb[1] <= x+15 and mb[2]>=y and mb[2] <= y+15 ):
                organizednavblock.append(mb)
                blocklength+=1

        #room: x, y, blockind, blocklength
        print(y)
        newroom = [x, y, curblockind+accumnavblock, blocklength]
        rooms.append(newroom)
        curblockind += blocklength
    
    if(len(navblocks)!= len(organizednavblock)):
        print("WARNING: one or more blocks are inside more than one room")
    
    assortedlevelinfo=[lwidth, lheight] 

    return [assortedlevelinfo]+[navblocks]+[rooms]

    

listOfLevels=[
    "mapTest.json"
]

allnavblocks=[]
allrooms=[]
alllevels=[]
#all entities later

levelind = 0
roomind=0
navblockind=0
totalnum_navblock=0


print(os.getcwd())
print('HHHHHHHHHHHHHH')
os.chdir('res\Levels')
print(os.getcwd())
for level in listOfLevels:
    jfile = open(level)
    print('processing level '+level)
    #curlevel: 0- basic info, 1-navblocks, 2-rooms
    curlevel = ReadLevel(jfile, totalnum_navblock)
    
    levelinfo = curlevel[0]
    levelnavblocks = curlevel[1]
    levelrooms = curlevel[2]

    totalnum_navblock += len(levelnavblocks)

    allnavblocks += curlevel[1]
    allrooms += curlevel[2]

    #level next level, map ind, room ind, room len, ent ind, ent len
    curlevelentry = [levelind + 1, levelind, levelinfo[0], levelinfo[1], roomind, len(levelrooms),0,0]
    alllevels.append(curlevelentry)
    roomind += len(levelrooms)
    levelind+=1
    jfile.close()





#write to file
outputfile = open("output.txt","w")
#write navblocks
print("writing navblocks")
outputfile.write('const NavBlock ALLNAVBLOCKS[{}]={{'.format(len(allnavblocks)))
outputfile.write("\n")
count = 0
for n in allnavblocks:
    outputfile.write('{{ {},{},{},{},{} }}'.format(
            int(round(n[0])),int(round(n[1])),int(round(n[2])),int(round(n[3])),int(round(n[4]))
            ))
    #outputfile.write('{{{},{},{} }}'.format(int(round(n[1])),int(round(n[2])),int(round(n[3]))))
    count +=1 
    if count != len(allnavblocks):
        outputfile.write(",\n")
    print(n)
outputfile.write("\n};\n")

print("writing rooms")
outputfile.write('const Room ALLROOMS[{}]={{'.format(len(allrooms)))
outputfile.write("\n")
count = 0
for n in allrooms:
    outputfile.write('{{ {},{},{},{} }}'.format(int(round(n[0])),int(round(n[1])),int(round(n[2])),int(round(n[3]))))
    count +=1 
    if count != len(allrooms):
        outputfile.write(",\n")
    print(n)
outputfile.write("\n};\n")

print("writing levels")
outputfile.write('const Level ALLLEVELS[{}]={{'.format(len(alllevels)))
outputfile.write("\n")
count = 0
for n in alllevels:
    outputfile.write('{{ {},{},{},{},{},{},{},{} }}'.format(int(round(n[0])),int(round(n[1])),int(round(n[2])),int(round(n[3])),int(round(n[4])),int(round(n[5])),int(round(n[6])),int(round(n[7]))))
    count +=1 
    if count != len(alllevels):
        outputfile.write(",\n")
    print(n)
outputfile.write("\n};\n")