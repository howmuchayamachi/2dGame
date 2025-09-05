/*==============================================================================

   �G�t�F�N�g�`�� [effect.h]
														 Author : Harada Ren
														 Date   : 2025/07/04
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef EFFECT_H
#define EFFECT_H

#include <DirectXMath.h>

void Effect_Initialize();
void Effect_Finalize();

void Effect_Update();
void Effect_Draw();

void Effect_Create(const DirectX::XMFLOAT2& position);

#endif //EFFECT_H