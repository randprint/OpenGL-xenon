/** Transformation **/
float4x4 modelview_matrix : register (c0);
float4x4 projection_matrix : register (c8);
float4x4 viewport_matrix : register (c16);

struct VertexShaderInput
{
    float4 Position : POSITION0;	// model xyzw
    float2 uv0: TEXCOORD0;			// texture 0 uv
	float2 uv1: TEXCOORD1;			// texture 1 uv
	float4 color: COLOR;			// color
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;	// model xyzw
    float2 uv0: TEXCOORD0;			// texture 0 uv
	float2 uv1: TEXCOORD1;			// texture 1 uv
	float4 color: COLOR;			// color
};


VertexShaderOutput vs_main(VertexShaderInput input)
{
    VertexShaderOutput output;
    // gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	float4x4 MVP = mul(projection_matrix, modelview_matrix);
    //output.Position = mul(input.Position, MVP);//mul(MVP, input.Position);
	//output.Position = input.Position;
	
	output.Position = mul(MVP, input.Position);
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;
	output.color = input.color;
	
	// reverse Z
	/*
	output.Position.z= 1 - output.Position.z;
	output.Position.w = 1;
	*/
    return output;
}

sampler2D texture0 : register(s0); 
sampler2D texture1 : register(s1); 

struct PixelShaderInput
{
	float2 uv0: TEXCOORD0;			// texture 0 uv
	float2 uv1: TEXCOORD1;			// texture 1 uv
	float4 color: COLOR;			// color
};

float4 ps_color(PixelShaderInput input): COLOR {
    return input.color;
}

float4 ps_modulate(PixelShaderInput input): COLOR {
    return input.color * tex2D(texture0, input.uv0);
}

float4 ps_texture(PixelShaderInput input): COLOR {
    return tex2D(texture0, input.uv0);
}
