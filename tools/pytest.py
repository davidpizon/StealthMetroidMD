import json
import os
from subprocess import Popen

jfile = open("C:\\gamedev\\PlatformerMD\\res\\Levels\\LDtk_project\\world_levels.ldtk")
jdata = json.load(jfile)
outputfile = open("C:\\gamedev\\PlatformerMD\\tools\\parseroutput\\world_levels.txt","w")

outputfile.write("RES FILE LINES:\n\n\n\n")

levels = jdata['levels']
numlevels = len(levels)
#following code finds all indices in the final array and connects them to uid
idtoindex=[]
for l in range(numlevels):
    print(levels[l]['uid']) 
    idtoindex.append([levels[l]['uid'],l])
    path   = "C:\\gamedev\\PlatformerMD\\res\Levels\\LDtk_project\\world_levels"
    
    # pathfg = path+"\\png\\{}-AutoMainBG.png".format(levels[l]["identifier"])
    # pathbg = path+"\\png\\{}-AutoMainFG.png".format(levels[l]["identifier"])
    pathfg = path+"\\png\\{}-MAINBG.png".format(levels[l]["identifier"])
    pathbg = path+"\\png\\{}-MAINFG.png".format(levels[l]["identifier"])

    pathcol = path+"\\png\\{}-Col_layer.png".format(levels[l]["identifier"])
    pathinter=path+"\\png\\{}-output.ase".format(levels[l]["identifier"])
    pathout = path+"\\{}-AutoMain.png".format(levels[l]["identifier"])
    pathoutcol = path+"\\{}-col.png".format(levels[l]["identifier"])
    
    # process the images:
    # p = Popen("C:\\gamedev\\PlatformerMD\\tools\\aseprite_batch.bat {} {} {} {} {} {}".format(pathfg,pathbg,pathcol,pathinter,pathout,pathoutcol), cwd=r"C:\\gamedev\\PlatformerMD\\tools")
    # stdout, stderr = p.communicate()

    # create the RES file for these images
    name = levels[l]["identifier"]        
    resentry1 = 'TILESET {}_tileset "Levels/LDtk_project/world_levels/{}-AutoMain.png" BEST ALL'.format(name,name)
    resentry2 = 'MAP {}_map "Levels/LDtk_project/world_levels/{}-AutoMain.png" {}_tileset BEST 0'.format(name,name,name)
    resentry3 = 'MAP {}_colmap "Levels/LDtk_project/world_levels/{}-col.png" collision_tileset BEST 0'.format(name,name)
    outputfile.write(resentry1+"\n")
    outputfile.write(resentry2+"\n")
    outputfile.write(resentry3+"\n")
print(idtoindex)



#search entity layer index
layerind = 0
for l in range( len(levels[0]['layerInstances'] )):
    if (levels[0]['layerInstances'][l]["__identifier"]=="EntLayer"):
        layerind = l
        break


outputfile.write("\n\n\n\nLEVELS HEADER FILE LINES:\n\n\n")

outputfile.write('#define numLevels  {}\n'.format(len(levels)))
outputfile.write('const struct LevelDef ALLLEVELS[numLevels]={\n')

numents = 0
l = 0
allents=[]
entcount=0
for lvl in levels:

    #filling the entity array
    #ents = lvl['layerInstances'][5]['entityInstances']
    ents = lvl['layerInstances'][layerind]['entityInstances']  
    curentcount = entcount
    for n in ents:
        allents.append(n['__identifier'])  
        allents.append(n['px'][0])  
        allents.append(n['px'][1])  
        entcount += 3
        for f in n['fieldInstances']:
            if(f['__type']=='Bool'):
                if(f['__value']==True):
                    allents.append(1)
                else:
                    print(f['__value'])
                    allents.append(0)
            elif(f['__type']=='Point'):
                if(f['__value']!=None):                    
                    allents.append(f['__value']['cx'])
                    allents.append(f['__value']['cy'])
            else:
                allents.append(f['__value'])
                
            entcount +=1
        
    
    levelname = lvl["identifier"]
    outputfile.write('//Level: {} \n'.format(levelname))
    ents = lvl['layerInstances'][5]['entityInstances']
    #I should not rely on the numbering staying as is and make a loop to check the identifier of the layer
    
    #write to file level stuff
    for id,ind in idtoindex:
        if(id==levels[l]['uid']):
            lind = ind
            break
    outputfile.write('{{  {},{},{}, //id, x, y \n'.format(lind, lvl['worldX'], lvl['worldY']))
    outputfile.write('   {},{},{},{}, // w, h, start of entities, end of entities \n'.format(lvl['pxWid'], lvl['pxHei'], curentcount, entcount-1))
    outputfile.write('   { ')
    for i in range(8):
        try:
            neiid = lvl["__neighbours"][i]["levelUid"]
            neiind = -2
            for pair in idtoindex:
                if neiid == pair[0]:
                    neiind = pair[1]
            outputfile.write(' {} '.format(neiind))            
        except:
            outputfile.write(' -1 ')     
        if i!= 7:
            outputfile.write(' ,')    
    outputfile.write('}},\n     &{}_tileset, &{}_map, &bgb_tileset, &bgb_map, &{}_colmap \n'.format(levelname,levelname,levelname))

    if l == numlevels-1:
        outputfile.write('} \n ')
    else:
        outputfile.write('\n }, \n')
    l+=1
    numents += len(ents)
outputfile.write("};\n")

print("0---")
print(*allents, sep=',')
numents = len(allents)

#entities
outputfile.write('#define SIZEENTARRAY  {}\n'.format(numents))
print('totalEnts  {}\n'.format(numents))
outputfile.write('const int ALLENTS[SIZEENTARRAY]={')
outputfile.write("\n")


#parei aqui: só falta escrever no arquivo todos os ents
linebreakcounter = 0
for i in range(0,len(allents),1):
    outputfile.write('{}'.format(allents[i]))
    if(i!=len(allents)-1):
        outputfile.write(', ')
    linebreakcounter+=1
    if(linebreakcounter>10):
        outputfile.write('\n')
        linebreakcounter = 0



# for lvl in levels:
#     #ents = lvl['layerInstances'][5]['entityInstances']
#     ents = lvl['layerInstances'][layerind]['entityInstances']    
    
#     count = 0
#     i=0
#     for n in ents:
        
#         outputfile.write('{{ {},{},{} }}'.format(n['__identifier'],n['px'][0],n['px'][1]))        
#         outputfile.write(",\n")
#         i+=1



# #remove last comma
# outputfile.seek(outputfile.tell() - 3, os.SEEK_SET) 
# outputfile.write(" ")
outputfile.write("\n};\n")