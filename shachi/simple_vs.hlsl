cbuffer cbScene : register(b0)
{
	matrix projection;
	matrix view;
}

cbuffer cbModel : register(b1)
{
	matrix world;
}

float4 main( float3 pos : POSITION ) : SV_POSITION
{
	float4 outpos = mul(float4(pos, 1.0F), world);
	outpos = mul(outpos, view);
	outpos = mul(outpos, projection);
	return outpos;
}
// EOF
