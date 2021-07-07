from PIL import Image 
import random
import copy

print(random.uniform(0.0, 10.0))

distThresh = 5.0
# creating a image object 
#im = Image.open(r"C:/gamedev/PlatformerMD/res/gfx/simpleplatforms_back.png") 
im = Image.open(r"C:/gamedev/PlatformerMD/res/Levels/concepts/city_tilesetconversion.png") 


px = im.load() 
width, height = im.size

print (width) 
print (height) 
#px[4, 4] = (0) 

def MakeTile(x0, y0):
    returntile = []
    for y in range(y0, y0+8, 1):
        curline=[]
        for x in range(x0, x0+8, 1):
            curline += [px[x,y]]
        
        returntile.append(curline)
    # print(returntile)
    # exit()
    return returntile

def CombTileMinimize(t1, t2, n1, n2):
    smallestd = 1000.0
    bestTile = []
    i=0
    while i<100 :
        i+=1
        curTile = CombineTile1(t1, t2, n1, n1)
        d1 = MatrixSim2(t1, curTile)
        d2 = MatrixSim2(t2, curTile)
        curd = (d1+d2)/2.0
        if(curd < smallestd):
            smallestd = curd
            bestTile = curTile
    return bestTile




def CombineTile1(t1, t2, n1, n2):
    returntile = []
    for y in range(0, 8, 1):
        curline=[]
        for x in range(0, 8, 1):
            if(n1>n2):
                ht = t1
                lt = t2
                maxn = n1
                minn = n2
            else:
                ht = t2
                lt = t1
                maxn = n2
                minn = n1
            if(n1==n2):
                diceroll = random.uniform(0.0,1.0)*1.0
                minn = 1.0
            else:
                diceroll = random.uniform(0.0,maxn*1.0)*1.0
                #print("maxn = {}, minn = {}, roll = {}".format(maxn, minn, diceroll))            
            
            if ( diceroll >= minn *0.5):
                t = ht
            else:
                t = lt
            

            curline += [t[y][x]]
        
        returntile.append(curline)
    return returntile

def FlipX(tile):
    returntile = []
    for y in range(0, 8, 1):
        curline=[]
        for x in range(7, -1, -1):
            curline += [tile[y][x]]
        
        returntile.append(curline)
    return returntile

def FlipY(tile):
    returntile = []
    for y in range(7, -1, -1): 
        #print(tile[y])       
        returntile.append(tile[y])
    return returntile

def PrintTile(tile):    
    for y in range(0,8,1): 
        print(tile[y])



#categorizing existing tiles
uniquetiles = []
timesTileAppears=[]
image =[]
tileind=0
for x in range(0, width-1, 8):
    for y in range(0, height-1, 8):
        curTile = MakeTile(x,y)
        #compare with current uniquetiles to see if it is unique
        fx = False
        fy = False
        existingTile=0 #to be filled in case this tile isnt unique
        isunique = True 
        for t in range(len(uniquetiles)):
            if(curTile== uniquetiles[t]):
                isunique = False
                existingTile = t
        if(isunique==True):
            #if still not found, flipx and try again
            flipxTile = FlipX(curTile)
            for t in range(len(uniquetiles)):
                if(flipxTile==uniquetiles[t]):
                    fx = True
                    isunique = False
                    existingTile = t
            
        if(isunique==True):
            #if stiiiiiill not found, flipy and try again
            flipyTile = FlipY(curTile)
            for t in range(len(uniquetiles)):
                if(flipyTile==uniquetiles[t]):
                    fy = True
                    isunique = False
                    existingTile = t
                
        if(isunique==True):
            #if stiiiiiiiiiiill not found, flipx and y and try again
            flipTile = FlipY(curTile)
            flipTile = FlipX(flipTile)
            for t in range(len(uniquetiles)):
                if(flipTile==uniquetiles[t]):
                    fx = True
                    fy = True
                    isunique = False
                    existingTile = t

        if(isunique):
            uniquetiles.append(curTile)

            imagepart=[tileind, False, False]
            tileind += 1
            image.append(imagepart)

            timesTileAppears.append(1)
        else:
            imagepart=[existingTile, fx, fy]
            image.append(imagepart)
            timesTileAppears[existingTile]+=1

       


#find similar tiles now
def MatrixSim1(t1, t2):
    total=0
    for y in range(0, 8, 1):
        for x in range(0, 8, 1):
            total += abs(t1[y][x] - t2[y][x])
    return total

def MatrixSim2(t1, t2):
    # print("teste")
    # print(t1)
    total=0
    for y in range(0, 8, 1):
        for x in range(0, 8, 1):
            t = t1[y][x] - t2[y][x]
            total += t*t
    return total**(0.5)


newUniqueTiles =[]
nui = 0
replacementList=[]
skipthese =[]
newimage= copy.deepcopy(image)
for t0 in range(0, len(uniquetiles), 1):
    
    #print("starting thing")
    nextt0=False
    for s in skipthese:
        if (t0==s):
            #print("already merged this s0")
            nextt0 = True
            break
    if(nextt0):
        continue

    #ind  distance, flip x, flip y
    replacementCand = [0, 10000.0, False, False]
    
    for t1 in range(t0+1, len(uniquetiles),1):
        #check if t1 belongs to skipthese
        nextt1 = False
        for s in skipthese:
            if (t1==s):
                #print("already merged this s1")
                nextt1 = True
                break
        if(nextt1):
            continue

        d = MatrixSim2(uniquetiles[t0], uniquetiles[t1])
        if(d < replacementCand[1]):
            replacementCand[0] = t1
            replacementCand[1] = d
            replacementCand[2] = False
            replacementCand[3] = False            
        #repeat with flipped x tile
        tt1 = FlipX(uniquetiles[t1])
        d = MatrixSim2(uniquetiles[t0], tt1)
        if(d < replacementCand[1]):
            replacementCand[0] = t1
            replacementCand[1] = d
            replacementCand[2] = True
            replacementCand[3] = False      
        #repeat with flipped y tile
        tt1 = FlipY(uniquetiles[t1])
        d = MatrixSim2(uniquetiles[t0], tt1)
        if(d < replacementCand[1]):
            replacementCand[0] = t1
            replacementCand[1] = d
            replacementCand[2] = False
            replacementCand[3] = True    
        #repeat with flipped X and y tile
        tt1 = FlipY(uniquetiles[t1])
        tt1 = FlipX(tt1)
        d = MatrixSim2(uniquetiles[t0], tt1)
        if(d < replacementCand[1]):
            replacementCand[0] = t1
            replacementCand[1] = d
            replacementCand[2] = True
            replacementCand[3] = True    

    
    if(replacementCand[1] < distThresh):
        #merge tiles etc
        
        # mergedtile = CombineTile1(uniquetiles[t0], uniquetiles[replacementCand[0]], 
        #             timesTileAppears[t0], timesTileAppears[replacementCand[0]])
        tt1 = copy.deepcopy( uniquetiles[replacementCand[0]])
        if(replacementCand[2]):
            tt1 = FlipX(tt1)
        if(replacementCand[3]):
            tt1 = FlipY(tt1)
        mergedtile = CombTileMinimize(uniquetiles[t0], tt1, 
                    timesTileAppears[t0], timesTileAppears[replacementCand[0]])
        

        newUniqueTiles += [mergedtile]
        # replacemententry0 = [t0, nui, False, False]
        # replacemententry1 = [replacementCand[0], nui, replacementCand[2], replacementCand[3]]
        # replacementList.append(replacemententry0)
        # replacementList.append(replacemententry1)

        skipthese.append(replacementCand[0])
        
        #update newimage
        for p in range(len(image)):
            imgpart = copy.deepcopy(image[p])
            npart = newimage[p]         
            if(imgpart[0] == t0):
                npart[0] = nui  
                npart[1] = imgpart[1]
                npart[2] = imgpart[2]
            if(imgpart[0]== replacementCand[0]):
                npart[0] = nui 
                if(imgpart[1] ^ replacementCand[2]):
                    npart[1] = True                
                else:
                    npart[1] = False
                if(imgpart[2] ^ replacementCand[3]):
                    npart[2] = True               
                else:
                    npart[2] = False
                

    else:                
        newUniqueTiles += [uniquetiles[t0]]
        for p in range(len(newimage)):
            if(image[p][0] == t0):
                newimage[p][0] = nui                 
                newimage[p][1] = image[p][1]
                newimage[p][2] = image[p][2]

    #print("incrementing nui")
    nui += 1


def WriteTileToImage(x0,y0,tile):    
    for x in range(0,8, 1):
        for y in range(0,8, 1):
            px[x+x0,y+y0] = tile[y][x]
            

#save image
x=0
y=0
for imgelem in range(len(newimage)):
    # print("current tile {}".format(newimage[imgelem][0]))
    # PrintTile(newUniqueTiles[newimage[imgelem][0]])
    # #print("newUniqueTiles len {}".format(len(newUniqueTiles)))

    tile = copy.deepcopy( newUniqueTiles[newimage[imgelem][0]])    
    fx = newimage[imgelem][1]    
    fy = newimage[imgelem][2]    
    if(fx):
        tile = FlipX(tile)
    if(fy):
        tile = FlipY(tile)
    
    WriteTileToImage(x,y, tile)
    
    y+= 8
    if(y==height ): #end of line of tiles, next line now.
        x += 8
        y=0

im.save("tools/tilecompressionoutput/output.png")

print("previous unique tiles: {}".format(len(uniquetiles)))
print("unique tiles now: {}".format(len(newUniqueTiles)))

print("saving original copy to test stuff")
x=0
y=0
for imgelem in range(len(newimage)):
    # print("current tile {}".format(newimage[imgelem][0]))
    # PrintTile(newUniqueTiles[newimage[imgelem][0]])
    # #print("newUniqueTiles len {}".format(len(newUniqueTiles)))

    tile = copy.deepcopy( uniquetiles[image[imgelem][0]])    
    fx = image[imgelem][1]    
    fy = image[imgelem][2]    
    if(fx):
        tile = FlipX(tile)
    if(fy):
        tile = FlipY(tile)
    
    WriteTileToImage(x,y, tile)
    
    y+= 8
    if(y==height ): #end of line of tiles, next line now.
        x += 8
        y=0

im.save("tools/tilecompressionoutput/outputoriginal.png")

print("saving all tiles")
#clear image
for x in range(width):
    for y in range(height):
        px[x,y] = 0

x=0
y=0
for t in newUniqueTiles:
    
    WriteTileToImage(x,y, t)
    y+= 8
    if(y==height ): #end of line of tiles, next line now.
        x += 8
        y=0

im.save("tools/tilecompressionoutput/output_tiles.png")