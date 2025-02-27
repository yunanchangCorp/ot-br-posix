/*
 *  Copyright (c) 2024, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#define OTBR_LOG_TAG "CTRLR"

#include "thread_controller.hpp"

#include <openthread/logging.h>
#include <openthread/openthread-system.h>

#include "lib/spinel/coprocessor_type.h"

#include "rcp_host.hpp"

namespace otbr {
namespace Ncp {

std::unique_ptr<ThreadController> ThreadController::Create(const char                      *aInterfaceName,
                                                           const std::vector<const char *> &aRadioUrls,
                                                           const char                      *aBackboneInterfaceName,
                                                           bool                             aDryRun,
                                                           bool                             aEnableAutoAttach)
{
    CoprocessorType                   coprocessorType;
    otPlatformCoprocessorUrls         urls;
    std::unique_ptr<ThreadController> host;
    otLogLevel                        level = ConvertToOtLogLevel(otbrLogGetLevel());

    VerifyOrDie(aRadioUrls.size() <= OT_PLATFORM_CONFIG_MAX_RADIO_URLS, "Too many Radio URLs!");

    urls.mNum = 0;
    for (const char *url : aRadioUrls)
    {
        urls.mUrls[urls.mNum++] = url;
    }

    VerifyOrDie(otLoggingSetLevel(level) == OT_ERROR_NONE, "Failed to set OT log Level!");

    coprocessorType = otSysInitCoprocessor(&urls);

    if (coprocessorType == OT_COPROCESSOR_RCP)
    {
        host = MakeUnique<RcpHost>(aInterfaceName, aRadioUrls, aBackboneInterfaceName, aDryRun, aEnableAutoAttach);
    }
    else
    {
        // TODO: add NCP type
        DieNow("Unknown coprocessor type!");
    }

    return host;
}

otLogLevel ThreadController::ConvertToOtLogLevel(otbrLogLevel aLevel)
{
    otLogLevel level;

    switch (aLevel)
    {
    case OTBR_LOG_EMERG:
    case OTBR_LOG_ALERT:
    case OTBR_LOG_CRIT:
        level = OT_LOG_LEVEL_CRIT;
        break;
    case OTBR_LOG_ERR:
    case OTBR_LOG_WARNING:
        level = OT_LOG_LEVEL_WARN;
        break;
    case OTBR_LOG_NOTICE:
        level = OT_LOG_LEVEL_NOTE;
        break;
    case OTBR_LOG_INFO:
        level = OT_LOG_LEVEL_INFO;
        break;
    case OTBR_LOG_DEBUG:
    default:
        level = OT_LOG_LEVEL_DEBG;
        break;
    }

    return level;
}

} // namespace Ncp
} // namespace otbr
