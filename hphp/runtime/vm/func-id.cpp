/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#include <limits>

#include "hphp/runtime/vm/func-id.h"
#include "hphp/runtime/vm/func.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

FuncId FuncId::Invalid = FuncId::fromInt(std::numeric_limits<FuncId::Int>::max());
FuncId FuncId::Dummy   = FuncId::fromInt(std::numeric_limits<FuncId::Int>::max() - 1);

#ifdef USE_LOWPTR
FuncId::Int FuncId::toStableInt() const { return getFunc()->getStableId(); }
#endif

///////////////////////////////////////////////////////////////////////////////
}
