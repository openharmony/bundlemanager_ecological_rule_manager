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

#ifndef INTERFACES_KITS_JS_NAPI_ECOLOGICALRULEMANAGERSERVICE_H
#define INTERFACES_KITS_JS_NAPI_ECOLOGICALRULEMANAGERSERVICE_H

#include <string>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "js_native_api.h"

namespace OHOS {
namespace EcologicalRuleMgrServiceNapi {
namespace {
const int32_t NO_ERROR = 0;
const int32_t ERROR = -1;
const int32_t MAX_BUFFER_SIZE = 10240;
const int32_t ARGS_ONE = 1;
const int32_t ARGS_TWO = 2;
const int32_t ARGS_THREE = 3;
const int32_t PARAM_ONE = 0;
const int32_t PARAM_TWO = 1;
const int32_t PARAM_THREE = 2;
} // namespace

typedef struct AsyncCheckUpdateContext {
    napi_env env = nullptr;
    napi_async_work work = nullptr;
    napi_ref callbackRef = nullptr;
    int32_t errCode = NO_ERROR;
    bool hasNewVersion = false;
    std::string appUpdateInfo = "";
} AsyncCheckUpdateContext;

napi_value NAPI_SetAppEnhancedData(napi_env env, napi_callback_info info);
napi_value NAPI_SetRuleInfo(napi_env env, napi_callback_info info);
napi_value NAPI_SetSceneExperience(napi_env env, napi_callback_info info);
napi_value NAPI_ExemptExperience(napi_env env, napi_callback_info info);
napi_value NAPI_GetVersion(napi_env env, napi_callback_info info);

napi_value NAPI_GetSceneCode(napi_env env, napi_callback_info info);
napi_value NAPI_GetInitialSceneCode(napi_env env, napi_callback_info info);

napi_value NAPI_GetSelfSceneCode(napi_env env, napi_callback_info info);
napi_value NAPI_GetAdsVerificationVersion(napi_env env, napi_callback_info info);
} // namespace EcologicalRuleMgrServiceNapi
} // namespace OHOS

#endif // INTERFACES_KITS_JS_NAPI_ECOLOGICALRULEMANAGERSERVICE_H