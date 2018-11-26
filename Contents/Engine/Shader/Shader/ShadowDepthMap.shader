##Count : 2
##ShaderType : 1
##HlslPath : ../../../../../../../Contents/Engine/Shader/HLSL/LightShadowPass_vs.hlsl
##InputLayout
##Count : 2
POSITION 0 6 0 0 0 0
TEXCOORD 0 16 0 12 0 0
##CBuffers
##Count : 1
##BufferName : PassBuffer
##VarCount : 1
##Var : wvpMatrix 0 15
##Texture2D
##Count : 0
##SamplerState
##Count : 0
##ShaderType : 0
##HlslPath : ../../../../../../../Contents/Engine/Shader/HLSL/LightShadowPass_ps.hlsl
##InputLayout
##Count : 0
##CBuffers
##Count : 1
##BufferName : LightMatrixBuffer
##VarCount : 3
##Var : LightViewProjMatrix 0 15
##Var : InvCameraMatrix 16 31
##Var : InvProjMatrix 32 47
##Texture2D
##Count : 2
T_Pos_Depth
T_ShadowTexture
##SamplerState
##Count : 2
21 3 3 3 0 1 8 0 0 0 0 0 3.40282e+38 
148 1 1 1 0 1 8 0 0 0 0 0 3.40282e+38 
