// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include "log/Log.h"

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�


// �궨��
#define NORET
#define NOOP

#define ASSERT(p) assert(p)

#define ASSERT_RET(p, r) { \
	if(!(p)) { \
	assert(false); \
	return r; \
	} \
}

#define ASSERT_OP(p, op) { \
	if(!(p)) { \
	assert(false); \
	op; \
	} \
}