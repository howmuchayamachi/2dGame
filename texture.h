/*====================================================================================

テクスチャ管理[texture.h]

																Author	: Harada Ren
																Date	: 2025/06/13
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H


#include <d3d11.h>

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Texture_Finalize(void);

//テクスチャ画像の読み込み
//戻り値は管理番号 (読み込めなかった場合-1)
int Texture_Load(const wchar_t* pFilename); //引数はファイル名

//テクスチャの全開放
void Texture_AllRelease();

//管理番号を与えるとポリゴンにテクスチャをセットする
void Texture_SetTexture(int texid);

//テクスチャの幅/高さを返す
unsigned int Texture_Width(int texid);
unsigned int Texture_Height(int texid);

#endif //TEXTURE_H