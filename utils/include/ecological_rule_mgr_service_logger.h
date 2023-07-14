/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UTILS_NATIVE_INCLUDE_ECOLOGICALRULEMGRSERVICE_LOGGER_H
#define UTILS_NATIVE_INCLUDE_ECOLOGICALRULEMGRSERVICE_LOGGER_H

#include "hilog/log.h"

namespace OHOS {
static constexpr OHOS::HiviewDFX::HiLogLabel ERMS_LABEL = { LOG_CORE, 0xD0017D7, "ERMS" };

namespace EcologicalRuleMgrService {
#define ERMS_LOG(level, TAG, fmt, ...)                                                                                \
    OHOS::HiviewDFX::HiLog::level(ERMS_LABEL, "%{public}s::%{public}s::%{public}d " fmt, TAG, __FUNCTION__, __LINE__, \
        ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) ERMS_LOG(Debug, TAG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) ERMS_LOG(Info, TAG, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) ERMS_LOG(Warn, TAG, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) ERMS_LOG(Error, TAG, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) ERMS_LOG(Fatal, TAG, fmt, ##__VA_ARGS__)
} // namespace EcologicalRuleMgrService
} // namespace OHOS

#endif // UTILS_NATIVE_INCLUDE_ECOLOGICALRULEMGRSERVICE_LOGGER_H