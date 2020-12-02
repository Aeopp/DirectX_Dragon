

extern matrix WorldViewMatrix;
extern matrix WorldViewProjMatrix;

extern vector Color;
extern vector LightDirection;

struct VS_INPUT
{
    vector position : POSITION;
    vector normal : NORMAL; 
};

struct VS_OUTPUT
{
    vector position : SV_Position;
    float2 uvCoords : TEXCOORD;
    vector diffuse : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    // 정점 위치를 뷰 스페이스로 이동.
    output.position = mul(
    input.position,
    WorldViewProjMatrix);
   
    vector _LightDirection = LightDirection;
    
    _LightDirection.w = 0.0f;
    input.normal.w = 0.f;
    
    //// 빛의 방향도 뷰로 이동
    _LightDirection = mul(
    _LightDirection,
    WorldViewMatrix);
    
    input.normal = mul(input.normal,
    WorldViewMatrix);
    
    float u = dot(_LightDirection,
        input.normal);
    
    if (u < 0.0f)
        u = 0.0f; 
    
    float v = 0.5f;
    
    output.uvCoords.x = u;
    output.uvCoords.y = v;
    
    output.diffuse = Color;
    
    return output;
}