/*==============================================================================

   2D�`��p�s�N�Z���V�F�[�_�[ [shader_pixel_2d.hlsl]
														 Author : Harada Ren
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/

struct PS_IN
{
    float4 posH : SV_POSITION; //System Value SV_Position
    float4 color: COLOR0;
    float2 uv   : TEXCOORD0;
};

Texture2D tex;//�e�N�X�`��
SamplerState samp; //�e�N�X�`���T���v��
//�T���v���[

float4 main(PS_IN pi) : SV_TARGET
{
    //a*b
    //a.r*b.r  a.g*b.g  a.b*b.b  a.a*b.a
    return tex.Sample(samp, pi.uv) * pi.color; //uv�̍��W�̃T���v���[�̃e�N�X�`���̐F��Ԃ�
}
