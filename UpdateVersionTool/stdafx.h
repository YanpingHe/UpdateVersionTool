// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include "log/Log.h"

// TODO:  在此处引用程序需要的其他头文件


// 宏定义
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