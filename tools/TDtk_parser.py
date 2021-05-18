import json
import os
from subprocess import Popen

jfile = open("C:\\gamedev\\PlatformerMD\\res\\Levels\\LDtk_project\\world_levels.ldtk")
jdata = json.load(jfile)
outputfile = open("C:\\gamedev\\PlatformerMD\\res\\world_levels.txt","w")

outputfile.write("RES FILE LINES:\n\n\n\n")

levels = jdata['levels']
numlevels = len(levels)
#following code finds all indices in the final array and connects them to uid
idtoindex=[]
for l in range(numlevels):
    print(levels[l]['uid']) 
    idtoindex.append([levels[l]['uid'],l])
    path   = "C:\\gamedev\\PlatformerMD\\res\Levels\\LDtk_project\\world_levels"
    pathfg = path+"\\png\\{}-AutoMainBG.png".format(levels[l]["identifier"])
    pathbg = path+"\\png\\{}-AutoMainFG.png".format(levels[l]["identifier"])
    pathcol = path+"\\png\\{}-Col_layer.png".format(levels[l]["identifier"])
    pathinter=path+"\\png\\{}-output.ase".format(levels[l]["identifier"])
    pathout = path+"\\{}-AutoMain.png".format(levels[l]["identifier"])
    pathoutcol = path+"\\{}-col.png".format(levels[l]["identifier"])
    
    # process the images:
    p = Popen("C:\\gamedev\\PlatformerMD\\tools\\aseprite_batch.bat {} {} {} {} {} {}".format(pathfg,pathbg,pathcol,pathinter,pathout,pathoutcol), cwd=r"C:\\gamedev\\PlatformerMD\\tools")
    stdout, stderr = p.communicate()

    # create the RES file for these images
    name = levels[l]["identifier"]        
    resentry1 = 'TILESET {}_tileset "Levels/LDtk_project/world_levels/{}-AutoMain.png" BEST ALL'.format(name,name)
    resentry2 = 'MAP {}_map "Levels/LDtk_project/world_levels/{}-AutoMain.png" {}_tileset BEST 0'.format(name,name,name)
    resentry3 = 'MAP {}_colmap "Levels/LDtk_project/world_levels/{}-col.png" collision_tileset BEST 0'.format(name,name)
    outputfile.write(resentry1+"\n")
    outputfile.write(resentry2+"\n")
    outputfile.write(resentry3+"\n")
print(idtoindex)

outputfile.write("\n\n\n\nLEVELS HEADER FILE LINES:\n\n\n")

outputfile.write('#define numLevels  {}\n'.format(len(levels)))
outputfile.write('const struct LevelDef ALLLEVELS[numLevels]={\n')

numents = 0
l = 0
for lvl in levels:
    
    levelname = lvl["identifier"]
    outputfile.write('//Level: {} \n'.format(levelname))
    ents = lvl['layerInstances'][3]['entityInstances']
    
    
    #write to file level stuff
    for id,ind in idtoindex:
        if(id==levels[l]['uid']):
            lind = ind
            break
    outputfile.write('{{  {},{},{}, //id, x, y \n'.format(lind, lvl['worldX'], lvl['worldY']))
    outputfile.write('   {},{},{},{}, // w, h, start of entities, num of entities \n'.format(lvl['pxWid'], lvl['pxHei'], numents, len(ents)))
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
    



#entities
outputfile.write('#define totalEnts  {}\n'.format(numents))
outputfile.write('const struct LevelEnt ALLENTS[totalEnts]={')
outputfile.write("\n")

for lvl in levels:
    ents = lvl['layerInstances'][3]['entityInstances']
    
    
    count = 0
    i=0
    for n in ents:
        #print(n['__identifier'])    
        outputfile.write('{{ {},{},{} }}'.format(n['__identifier'],n['px'][0],n['px'][1]))        
        outputfile.write(",\n")
        i+=1



#remove last comma
outputfile.seek(outputfile.tell() - 3, os.SEEK_SET) 
outputfile.write(" ")
outputfile.write("\n};\n")