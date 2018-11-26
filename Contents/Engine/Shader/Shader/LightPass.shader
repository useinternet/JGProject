##Count : 2
##ShaderType : 1
##HlslPath : ../../../../../../../Contents/Engine/Shader/HLSL/LightPass_vs.hlsl
##InputLayout
##Count : 2
POSITION 0 6 0 0 0 0
TEXCOORD 0 16 0 12 0 0
##CBuffers
##Count : 1
##BufferName : PassBuffer
##VarCount : 2
##Var : wvpMatrix 0 15
##Var : CameraDir 16 19
##Texture2D
##Count : 0
##SamplerState
##Count : 0
##ShaderType : 0
##HlslPath : ../../../../../../../Contents/Engine/Shader/HLSL/LightPass_ps.hlsl
##InputLayout
##Count : 0
##CBuffers
##Count : 1
##BufferName : LightBuffer
##VarCount : 35
##Var : DirLight 0 15
##Var : PntLight0 16 23
##Var : PntLight1 24 31
##Var : PntLight2 32 39
##Var : PntLight3 40 47
##Var : PntLight4 48 55
##Var : PntLight5 56 63
##Var : PntLight6 64 71
##Var : PntLight7 72 79
##Var : PntLight8 80 87
##Var : PntLight9 88 95
##Var : PntLight10 96 103
##Var : PntLight11 104 111
##Var : PntLight12 112 119
##Var : PntLight13 120 127
##Var : PntLight14 128 135
##Var : PntLight15 136 143
##Var : PntLight16 144 151
##Var : PntLight17 152 159
##Var : PntLight18 160 167
##Var : PntLight19 168 175
##Var : SptLight0 176 187
##Var : SptLight1 188 199
##Var : SptLight2 200 211
##Var : SptLight3 212 223
##Var : SptLight4 224 235
##Var : SptLight5 236 247
##Var : SptLight6 248 259
##Var : SptLight7 260 271
##Var : SptLight8 272 283
##Var : SptLight9 284 295
##Var : DirectionLightCount 296 296
##Var : PointLightCount 297 297
##Var : SpotLightCount 298 298
##Var : TempCount2 299 299
##Texture2D
##Count : 4
T_Pos_Depth
T_Normal_SpecPow
T_Albedo
T_Specular
##SamplerState
##Count : 1
21 3 3 3 0 1 8 0 0 0 0 0 3.40282e+38 
