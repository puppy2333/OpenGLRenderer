#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform int Stage;

int2 GetIndexForCurrStage(unsigned int InputIdx)
{
    // const unsigned int BatchSize = (int)exp2((float)Stage);
    int BatchSize = 1;
    for (int i = 0; i < Stage; i++)
    {
        BatchSize = BatchSize * 2;
    }
    
    int2 OutputIdx;
    if (InputIdx % BatchSize < BatchSize / 2)
    {
        OutputIdx = int2(InputIdx, InputIdx + BatchSize / 2);
    }
    else
    {
        OutputIdx = int2(InputIdx - BatchSize / 2, InputIdx);
    }
    return OutputIdx;
}

float2 GetWForCurrStage(int InputIdx)
{
    const float PI = 3.1415926;
    // const int BatchSize = (int)exp2((float)Stage);
    int BatchSize = 1;
    for (int i = 0; i < Stage; i++)
    {
        BatchSize = BatchSize * 2;
    }
    
    int CurrK = InputIdx % (BatchSize / 2);
    float2 W = float2(
        cos(2.0 * PI / (float)BatchSize * (float)CurrK),
        sin(2.0 * PI / (float)BatchSize * (float)CurrK));
    return W;
}

void main()
{
    int IndexX = int(TexCoords.x * 100);
    int IndexY = int(TexCoords.y * 100);
    
    // Row fft
    vec2 W = GetWForCurrStage(IndexX);
    ivec2 PrevIndexX = GetIndexForCurrStage(IndexX);
    vec2 EvenPart = GetFftBuffer(PrevIndexX.x, IndexY);
    vec2 OddPart = GetFftBuffer(PrevIndexX.y, IndexY);
    vec2 CurrValue;

    // const unsigned int BatchSize = (int)exp2((float)Stage);
    int BatchSize = 1;
    for (int i = 0; i < Stage; i++) {
        BatchSize = BatchSize * 2;
    }
    
    if (IndexX % BatchSize < BatchSize / 2) {
        CurrValue = float2(
            EvenPart.x + W.x * OddPart.x - W.y * OddPart.y,
            EvenPart.y + W.x * OddPart.y + W.y * OddPart.x);
    }
    else {
        CurrValue = float2(
            EvenPart.x - W.x * OddPart.x + W.y * OddPart.y,
            EvenPart.y - W.x * OddPart.y - W.y * OddPart.x);
    }
    FragColor = vec4(CurrValue, 0.0f, 0.0f, 1.0f);

    // if (exp2(Stage) == SpectrumNumCells.x)
    // {
    //     SetH(IndexX, IndexY, CurrValue.x);
    // }
    
    // FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
