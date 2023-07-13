/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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

#include "ecological_rule_mgr_service_napi.h"

namespace OHOS {
namespace EcologicalRuleMgrServiceNapi {
EXTERN_C_START
napi_value EcologicalRuleMgrServiceExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor eRMServiceDesc[] = {
        DECLARE_NAPI_FUNCTION("setAppEnhancedData", NAPI_SetAppEnhancedData),
        DECLARE_NAPI_FUNCTION("setRuleInfo", NAPI_SetRuleInfo),

        DECLARE_NAPI_FUNCTION("setSceneExperience", NAPI_SetSceneExperience),
        DECLARE_NAPI_FUNCTION("exemptExperience", NAPI_ExemptExperience),
        DECLARE_NAPI_FUNCTION("getVersion", NAPI_GetVersion),
        DECLARE_NAPI_FUNCTION("getSceneCode", NAPI_GetSceneCode),
        DECLARE_NAPI_FUNCTION("getInitialSceneCode", NAPI_GetInitialSceneCode),
        DECLARE_NAPI_FUNCTION("getSelfSceneCode", NAPI_GetSelfSceneCode),
        DECLARE_NAPI_FUNCTION("getAdsVerificationVersion", NAPI_GetAdsVerificationVersion)
    };
    NAPI_CALL(env,
        napi_define_properties(env, exports, sizeof(eRMServiceDesc) / sizeof(eRMServiceDesc[0]), eRMServiceDesc));
    return exports;
}
EXTERN_C_END

static napi_module ecologicalrulemgrservice_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = EcologicalRuleMgrServiceExport,
    .nm_modname = "ecologicalrulemgrservice",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void EcologicalRuleMgrServiceRegister()
{
    napi_module_register(&ecologicalrulemgrservice_module);
}
} // namespace EcologicalRuleMgrServiceNapi
} // namespace OHOS