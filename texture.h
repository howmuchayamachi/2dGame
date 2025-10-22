/*====================================================================================

�e�N�X�`���Ǘ�[texture.h]

																Author	: Harada Ren
																Date	: 2025/06/13
--------------------------------------------------------------------------------------

======================================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H


#include <d3d11.h>

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Texture_Finalize(void);

//�e�N�X�`���摜�̓ǂݍ���
//�߂�l�͊Ǘ��ԍ� (�ǂݍ��߂Ȃ������ꍇ-1)
int Texture_Load(const wchar_t* pFilename); //�����̓t�@�C����

//�e�N�X�`���̑S�J��
void Texture_AllRelease();

//�Ǘ��ԍ���^����ƃ|���S���Ƀe�N�X�`�����Z�b�g����
void Texture_SetTexture(int texid);

//�e�N�X�`���̕�/������Ԃ�
unsigned int Texture_Width(int texid);
unsigned int Texture_Height(int texid);

#endif //TEXTURE_H