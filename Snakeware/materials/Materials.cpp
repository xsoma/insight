#include "Materials.h"

void Materials::Setup() {
	std::ofstream("csgo/materials/vitality_metallic.vmt") << R"#("VertexLitGeneric" {
    "$basetexture" "vgui/white_additive"
    "$ignorez" "0"
    "$envmap" "env_cubemap"
     "$normalmapalphaenvmapmask" "1"
     "$envmapcontrast"  "1"
     "$nofog" "1"
     "$model" "1"
     "$nocull" "0"
      "$selfillum" "1"
      "$halflambert" "1"
       "$znearer" "0"
       "$flat" "1" 

        
})#";

	// https://developer.valvesoftware.com/wiki/Category:List_of_Shader_Parameters
	std::ofstream("csgo/materials/glowOverlay.vmt") << R"#("VertexLitGeneric" {
     
    	"$additive" "1"
    	"$envmap"
        "models/effects/cube_white"
    	"$envmapfresnel" "1"
    	"$envmapfresnelminmaxexp" "[0 1 2]"
    	"$alpha" "0.71"

    })#";

}

void Materials::Remove() {
	std::remove("csgo/materials/glowOverlay.vmt");
	std::remove("csgo/materials/vitality_metallic.vmt");

}


