
matrix ViewMatrix;
matrix ViewProjMatrix;

vector AmbientMtrl;
vector DiffuseMtrl;

vector LightDirection;

vector DiffuseLightIntensity = { 0.f, 0.f, 1.f, 1.f };
vector AmbientLightIntensity = { 0.f, 0.f, 0.2f, 1.f };

struct VS_INPUT
{
    vector position : POSITION;
    vector normal : NORMAL;
};
struct VS_OUTPUT
{
    vector position : SV_Position;
    vector diffuse : COLOR; 
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.position = mul(input.position, ViewProjMatrix);
    
   // LightDirection.w = 0.0f;
    vector _LightDirection = LightDirection;
    _LightDirection.w = 0.f;
    
    input.normal.w = 0.0f;
    _LightDirection = mul(_LightDirection, ViewMatrix);
    input.normal = mul(input.normal, ViewMatrix);
    
    float s = dot(_LightDirection, input.normal);
    
    if (s < 0.0f)
        s = 0.0f;
    
    output.diffuse = (AmbientMtrl * AmbientLightIntensity)
    + (s * (DiffuseLightIntensity * DiffuseMtrl));
 
   
    
    return output;
}