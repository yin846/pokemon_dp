////////////////////////////////////////////////////////////////////////
//
// isdbglib.h : �T�|�[�g���C�u����
//
// Copyright (C) 2003-2004 INTELLIGENT SYSTEMS Co.,Ltd. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef     ISDEBUGGER_LIBRARY_H
#define     ISDEBUGGER_LIBRARY_H

#ifdef  __cplusplus
extern "C" {
#endif 


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// �v�����g�f�o�b�O����
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///
/// �֐�����
/// 
///
/// [������o��]
/// void ISDPrint(const char* pBuf);
///      pBuf:  �\���p�L���[�o�b�t�@�ɕ������ǉ����܂��B
///
/// [�t�H�[�}�b�g�t��������o��]
/// void ISDPrintf(const char *pBuf, ...)
///      pBuf:  �\���p�L���[�o�b�t�@�Ƀt�H�[�}�b�g�����������ǉ����܂��B
///             �g�p���@�� ANSI��printf()�֐��Ɠ��l�ł��B
///
/// [�v���O�����������`�F�b�N�}�N��]
/// ISDASSERT( �]���� );
///      �]���� �ɂ́AC����̕]�������L�q���܂��B�]���� �̒l���^(ZERO��
///      �O)�Ȃ�Ύ��̃v���O���������s���܂��B�U(ZERO)�Ȃ�΁A
///      ISDASSERT()����`����Ă���\�[�X�t�@�C�����Ƃ��̃��C���i���o�[�A
///      �]���� ��\�����āA�v���O�����͒�~���܂��B
///
/// [�v���O�����������`�F�b�N�}�N��]
/// ISDWARNING( �]���� );
///      ISDASSERT()�Ɠ����g�p���@�ł��BISDASSERT()�Ƃ̈Ⴄ�_�́AASSERT
///      �́A�v���O�������~���ȍ~�̏����͍s��Ȃ��i�n���O�A�b�v��ԁj
///      ���Ƃɑ΂��AISDWARNNING()�͈��������v���O���������s����_�ł��B
///
/////////////////////////////////////////////////////////////////////////
/// �g�p��̒���
///
/// �EIS-CGB-EMULATOR �n�[�h�E�F�A�̓s����A�z�X�g���Ƀf�[�^�𑗂�Ԃɂ�
///   �ꎞ�I�Ɋ����݂��֎~�A�������E�F�C�g���ύX����܂��B
/// �E�o�׎���ROM�Ȃǂ̃����[�X�o�[�W�������쐬����ۂɂ́A NDEBUG ���`����
///   ���ƂŃv�����g�f�o�b�O�֐��E�}�N���͕K����菜���Ă��������B
///
/////////////////////////////////////////////////////////////////////////

#ifdef NDEBUG
    #define ISDPrint(pBuf)
    #define ISDPrintf(arg...)
    #define ISDAssert(file,line,info)
#else
    void ISDPrint(const char* pBuf);
    void ISDPrintf(const char *pBuf, ...);
    void ISDAssert(const char* pFile, int nLine, const char* pExpression, int nStopProgram);
#endif

/////////////////////////////////////////////////////////////////////////
///

#undef ISDASSERT
#ifdef NDEBUG
#define ISDASSERT(exp)
#else
#define ISDASSERT(exp)  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 1);
#endif

#undef ISDWARNING
#ifdef NDEBUG
#define ISDWARNING(exp)
#else
#define ISDWARNING(exp)  (exp) ? ((void)0) : ISDAssert(__FILE__, __LINE__, #exp, 0);
#endif


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// NITROToolAPI - �c�[������pAPI
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/// �X�g���[��API �f�[�^��(��)�M�T�C�Y�̍ő�o�C�g��
///#define      NITROSTM_SENDMAX            8192

/// MIDI-API �f�[�^��M�T�C�Y�̍ő�o�C�g��
///#define      NITROMID_RECVMAX            2048

enum {
    NITRODEVID_NULL,
    NITRODEVID_CGBEMULATOR,             // IS-CGB-EMULATOR
    NITRODEVID_NITEMULATOR,             // NITRO-EMULATOR
    NITRODEVID_NITUIC,                  // IS-NITRO-UIC CARTRIDGE
    NITRODEVID_AGBMIDI,                 // IS-AGB-MIDI CARTRIDGE
    NITRODEVID_MAX
};

enum {
    NITRORESID_POLL       = 0,
    NITRORESID_TIMER      = 1,
    NITRORESID_VBLANK     = 2,
    NITRORESID_CARTRIDGE  = 3,
    NITRORESID_MAX
};

#define NITROMASK_RESOURCE_POLL         (1 << NITRORESID_POLL     )
#define NITROMASK_RESOURCE_TIMER        (1 << NITRORESID_TIMER    )
#define NITROMASK_RESOURCE_VBLANK       (1 << NITRORESID_VBLANK   )
#define NITROMASK_RESOURCE_CARTRIDGE    (1 << NITRORESID_CARTRIDGE)

typedef  struct tagNITRODEVCAPS {
    u32         m_nSizeStruct;         // CAPS�\���̂̃T�C�Y
    u32         m_nDeviceID;           // �f�o�C�X����ID
    u32         m_dwMaskResource;      // ���̃f�o�C�X�𓮍삳����̂ɕK�v�Ȃ�\�[�X
} NITRODEVCAPS;
typedef void (*NITROSTREAMCALLBACKFUNC)( u32 dwUserData, u32 nChnStream, const void *pRecv, u32 dwRecvSize );

////////////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////////////
void         NITROToolAPIInit(void);

////////////////////////////////////////////////////////////////////////
// �ڑ�����Ă���f�o�C�X�\�͂̎擾
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIGetMaxCaps(void);
const NITRODEVCAPS  *NITROToolAPIGetDeviceCaps( int nCaps );


////////////////////////////////////////////////////////////////////////
// �f�o�C�X�̃I�[�v���A�N���[�Y
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIOpen( const NITRODEVCAPS* pCaps );
int          NITROToolAPIClose( void );

////////////////////////////////////////////////////////////////////////
// �X�g���[���ł̑���M
////////////////////////////////////////////////////////////////////////
int          NITROToolAPIStreamGetWritableLength( u32 *pnLength );
int          NITROToolAPIWriteStream( u32 nChnStream, const void *pSrc, u32 dwSize );
int          NITROToolAPISetReceiveStreamCallBackFunction( NITROSTREAMCALLBACKFUNC cbFunc, u32 dwUserData );

////////////////////////////////////////////////////////////////////////
// VBlank���荞�݂�^�C�}�[�ȂǁA����I�Ȋ��荞�݂̒��Ōp�����ČĂ�
////////////////////////////////////////////////////////////////////////
void         NITROToolAPIPollingIdle(void);             // EMULATOR�p
void         NITROToolAPITimerInterrupt(void);          // �^�C�}�[���荞��
void         NITROToolAPIVBlankInterrupt(void);         // �u�u�����N���荞��
void         NITROToolAPICartridgeInterrupt(void);      // �J�[�h���b�W���荞��


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// NITROMIDIAPI - MIDI�f�[�^�ǂݎ��API
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

typedef  struct tagNITROMIDICAPS {
    u32         m_nSizeStruct;          // CAPS�\���̂̃T�C�Y
    u32         m_nDeviceID;            // �f�o�C�X����ID (TOOLAPI�Ƌ���)
    u32         m_nMaxFIFO;             // ��M�o�b�t�@�T�C�Y
    u32         m_dwMaskResource;       // ���̃f�o�C�X�𓮍삳����̂ɕK�v�Ȃ�\�[�X(TOOLAPI�Ƌ���)
} NITROMIDICAPS;

////////////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPIInit(void);

////////////////////////////////////////////////////////////////////////
// MIDI�f�o�C�X�\�͂̎擾
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIGetMaxCaps(void);
const NITROMIDICAPS  *NITROMIDIAPIGetDeviceCaps( int nCaps );

////////////////////////////////////////////////////////////////////////
// MIDI�f�o�C�X�̃I�[�v���A�N���[�Y
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIOpen( const NITROMIDICAPS* pCaps );
int         NITROMIDIAPIClose( void );

////////////////////////////////////////////////////////////////////////
// �o�b�t�@�̃��Z�b�g
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPIReset(void);

////////////////////////////////////////////////////////////////////////
// �o�b�t�@�����O���̃f�[�^�����擾���܂�
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIGetReadableLength(void);

////////////////////////////////////////////////////////////////////////
// �o�b�t�@�����O���ꂽ�f�[�^��ǂ݂Ƃ�܂�
////////////////////////////////////////////////////////////////////////
int         NITROMIDIAPIRead( void *pBuf, u32 nBufSize );

////////////////////////////////////////////////////////////////////////
// �J�[�g���b�W���荞�ݓ��ŌĂ�ł�������
////////////////////////////////////////////////////////////////////////
void        NITROMIDIAPICartridgeInterrupt(void);

#ifdef  __cplusplus
}
#endif 
#endif      /* ISDEBUGGER_LIBRARY_H */

