/*==============================================================================

   2D�`��p�s�N�Z���V�F�[�_�[ [shader_pixel_2d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 color: COLOR0;
    float2 uv   : TEXCOORD0;
};

Texture2D tex;//�e�N�X�`��
SamplerState samp; //�e�N�X�`���T���v��

float4 main(PS_IN pi) : SV_TARGET
{
    //uv�̍��W�̃T���v���[�̃e�N�X�`���̐F��Ԃ�
    return tex.Sample(samp, pi.uv) * pi.color; 
}
