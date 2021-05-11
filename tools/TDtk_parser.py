import json
import os


jfile = open("C:\\gamedev\\PlatformerMD\\res\\Levels\\LDtk_project\\world_levels.ldtk")
jdata = json.load(jfile)
outputfile = open("C:\\gamedev\\PlatformerMD\\res\\world_levels.txt","w")



levels = jdata['levels']
numlevels = len(levels)
#following code finds all indices in the final array and connects them to uid
idtoindex=[]
for l in range(numlevels):
    print(levels[l]['uid'])
    idtoindex.append([levels[l]['uid'],l])
print(idtoindex)


outputfile.write('#define numLevels  {}\n'.format(len(levels)))
outputfile.write('const struct LevelDef ALLLEVELS[numLevels]={\n')

numents = 0
l = 0
for lvl in levels:
    
    levelname = lvl["identifier"]
    outputfile.write('//Level: {} \n'.format(levelname))
    ents = lvl['layerInstances'][3]['entityInstances']
    
    
    #write to file level stuff
    outputfile.write('{{  {},{},{}, //id, x, y \n'.format(lvl['uid'], lvl['worldX'], lvl['worldY']))
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
    if l == numlevels-1:
        outputfile.write('} \n } \n')
    else:
        outputfile.write('} \n }, \n')
    l+=1
    numents += len(ents)
outputfile.write("};\n")
    



#entities
outputfile.write('#define numEnts  {}\n'.format(numents))
outputfile.write('const struct LevelEnt ALLENTS[numEnts]={')
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