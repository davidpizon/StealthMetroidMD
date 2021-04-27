//my stuff
IMAGE leveltestforeground "gfx/levels_test_foreground.png" BEST
IMAGE leveltestforeground2 "gfx/levels_test_foreforeground.png" BEST

//TILESET bga_tileset "Levels/mapTest_collapsed.png" BEST ALL
TILESET bga_tileset "Levels/LDtk_project/world_levels/0000-Playground--01-MainLayer_collapsed.png" BEST ALL
TILESET bgb_tileset "gfx/simpleplatforms_back.png" BEST ALL
TILESET collision_tileset "Levels/collision_tileset.png" BEST 0


MAP bga_map "Levels/LDtk_project/world_levels/0000-Playground--01-MainLayer_collapsed.png" bga_tileset BEST 0
MAP bgb_map "gfx/simpleplatforms_back.png" bgb_tileset BEST 0
MAP col_map "Levels/LDtk_project/world_levels/png/0000-Playground--02-Col_layer.png" collision_tileset BEST 0


IMAGE logo_bg_1 "gfx/logo_bg_1.bmp" BEST
