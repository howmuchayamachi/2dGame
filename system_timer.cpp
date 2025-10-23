/*==============================================================================

   �V�X�e���^�C�}�[ [system_timer.cpp]
                                                         Author : Harada Ren
                                                         Date   : 2025/06/17
--------------------------------------------------------------------------------

==============================================================================*/
#include <Windows.h>

static bool g_bTimerStopped = true; // �X�g�b�v�t���O
static LONGLONG g_TicksPerSec = 0;  // �P�b�Ԃ̌v������
static LONGLONG g_StopTime;         // �X�g�b�v��������
static LONGLONG g_LastElapsedTime;  // �Ō�ɋL�^�����X�V����
static LONGLONG g_BaseTime;         // ��{����


// ��~���Ă���Β�~���ԁA�����łȂ���Ό��݂̎��Ԃ̎擾
static LARGE_INTEGER GetAdjustedCurrentTime(void);

// �V�X�e���^�C�}�[�̏�����
void SystemTimer_Initialize(void)
{
    g_bTimerStopped = true;
    g_TicksPerSec = 0;
    g_StopTime = 0;
    g_LastElapsedTime = 0;
    g_BaseTime = 0;

    // ������\�p�t�H�[�}���X�J�E���^���g���̎擾
    LARGE_INTEGER ticksPerSec = { 0 };
    QueryPerformanceFrequency(&ticksPerSec);
    g_TicksPerSec = ticksPerSec.QuadPart;
}

// �V�X�e���^�C�}�[�̃��Z�b�g
void SystemTimer_Reset(void)
{
    LARGE_INTEGER time = GetAdjustedCurrentTime();

    g_BaseTime = g_LastElapsedTime = time.QuadPart;
    g_StopTime = 0;
    g_bTimerStopped = false;
}

// �V�X�e���^�C�}�[�̃X�^�[�g
void SystemTimer_Start(void)
{
    // ���݂̎��Ԃ��擾
    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);

    // ���܂Ōv�����X�g�b�v���Ă�����
    if( g_bTimerStopped ) {
        // �~�܂��Ă������Ԃ����������Ċ�{���Ԃ��X�V
        g_BaseTime += time.QuadPart - g_StopTime;
    }

    g_StopTime = 0;
    g_LastElapsedTime = time.QuadPart;
    g_bTimerStopped = false;
}

// �V�X�e���^�C�}�[�̃X�g�b�v
void SystemTimer_Stop(void)
{
    if( g_bTimerStopped ) return;

    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);

    g_LastElapsedTime = g_StopTime = time.QuadPart; // ��~���Ԃ��L�^
    g_bTimerStopped = true;
}

// �V�X�e���^�C�}�[��0.1�b�i�߂�
void SystemTimer_Advance(void)
{
    g_StopTime += g_TicksPerSec / 10;
}

// �v�����Ԃ��擾
double SystemTimer_GetTime(void)
{
    LARGE_INTEGER time = GetAdjustedCurrentTime();

    return (double)(time.QuadPart - g_BaseTime) / (double)g_TicksPerSec;
}

// ���݂̎��Ԃ��擾
double SystemTimer_GetAbsoluteTime(void)
{
    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);

    return time.QuadPart / (double)g_TicksPerSec;
}

// �o�ߎ��Ԃ̎擾
double SystemTimer_GetElapsedTime(void)
{
    LARGE_INTEGER time = GetAdjustedCurrentTime();

    double elapsed_time = (double)(time.QuadPart - g_LastElapsedTime) / (double)g_TicksPerSec;
    g_LastElapsedTime = time.QuadPart;

    if( elapsed_time < 0.0 ) {
        elapsed_time = 0.0;
    }

    return elapsed_time;
}

// �V�X�e���^�C�}�[���~�܂��Ă��邩�H
bool SystemTimer_IsStoped(void)
{
    return g_bTimerStopped;
}

// ���݂̃X���b�h��1�̃v���Z�b�T�i���݂̃X���b�h�j�ɐ���
void LimitThreadAffinityToCurrentProc(void)
{
    HANDLE hCurrentProcess = GetCurrentProcess();

    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;

    if( GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask) != 0 && dwProcessAffinityMask ) {
        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        HANDLE hCurrentThread = GetCurrentThread();
        if( INVALID_HANDLE_VALUE != hCurrentThread ) {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }

    CloseHandle(hCurrentProcess);
}

// ��~���Ă���Β�~���ԁA�����łȂ���Ό��݂̎��Ԃ̎擾
LARGE_INTEGER GetAdjustedCurrentTime(void)
{
    LARGE_INTEGER time;
    if( g_StopTime != 0 ) {
        time.QuadPart = g_StopTime;
    }
    else {
        QueryPerformanceCounter(&time);
    }

    return time;
}
