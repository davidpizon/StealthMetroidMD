import json

jfile = open("C:\\gamedev\\PlatformerMD\\res\\Levels\\LDtk_project\\world_levels.ldtk")
jdata = json.load(jfile)

ents = jdata['levels'][0]['layerInstances'][2]['entityInstances']
numents = len(ents)



#write to file
outputfile = open("C:\\gamedev\\PlatformerMD\\res\\entities.txt","w")
outputfile.write('#define numLevelEnts  {}\n'.format(numents))
outputfile.write('const struct LevelEnt ALLENTS[numLevelEnts]={')
outputfile.write("\n")
count = 0
for n in ents:
    print(n['__identifier'])    
    outputfile.write('{{ {},{},{} }}'.format(n['__identifier'],n['px'][0],n['px'][1]))
    outputfile.write(",\n")
outputfile.write("\n};\n")