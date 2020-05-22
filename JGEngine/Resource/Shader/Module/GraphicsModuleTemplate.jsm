

// ifdef , ifndef 
// input Data 어디 셰이더에 들어가는지
format : .hlsl
type   : graphics
{

	// VertexShader 
	vs
	{
		path    = "..";
		version = "vs_5_0"
		entry   = "main" 
		property
		{
			mesh = "Static"
		}
	} 
	hs
	{
		path    = "..";
		version = "hs_5_0"
		entry   = "main" 
	}
	ds
	{
		path    = "..";
		version = "ds_5_0"
		entry   = "main" 
	}
	ps 
	{
		path    = "..";
		version = "ps_5_0"
		entry   = "main" 
		property
		{
			mesh = "Static"
			flag = { "..",  "..."}
		}
	}
}
