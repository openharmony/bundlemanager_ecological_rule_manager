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
#include "ecological_rule_mgr_service_napi.h"
#include <string>
#include <initializer_list>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "js_native_api.h"
#include "uv.h"
#include "ecological_rule_mgr_service_logger.h"
#include "ecological_rule_mgr_service_client.h"
#include "ecological_rule_mgr_service_interface.h"
#include "ecological_rule_mgr_service_context_container.h"
#include "ecological_rule_mgr_service_param.h"

namespace OHOS {
namespace EcologicalRuleMgrServiceNapi {
using namespace OHOS::EcologicalRuleMgrService;
#define TAG "js_ecologicalrulemgrservice"

napi_value NAPI_SetAppEnhancedData(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_TWO;
    napi_value args[ARGS_TWO];
    napi_value thisPointer = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    int32_t operType = 0;
    napi_get_value_int32(env, args[PARAM_ONE], &operType);

    char *str = nullptr;
    size_t strLen = 0;
    napi_get_value_string_utf8(env, args[PARAM_TWO], nullptr, 0, &strLen);
    str = new char[strLen + 1];
    napi_get_value_string_utf8(env, args[PARAM_TWO], str, strLen + 1, &strLen);

    std::string appData(str);
    LOG_INFO("NAPI_SetAppEnhancedData called, operType = %{public}d, appData = %{public}s", operType, appData.c_str());

    EcologicalRuleMgrServiceClient::GetInstance()->SetAppEnhancedData(operType, appData);
    LOG_INFO("NAPI_SetAppEnhancedData end");
    return (napi_value)IEcologicalRuleMgrService::ERR_OK;
}

napi_value NAPI_SetRuleInfo(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    char *str = nullptr;
    size_t strLen = 0;
    napi_get_value_string_utf8(env, args[PARAM_ONE], nullptr, 0, &strLen);
    str = new char[strLen + 1];
    napi_get_value_string_utf8(env, args[PARAM_ONE], str, strLen + 1, &strLen);

    std::string ruleInfo(str);
    LOG_INFO("NAPI_SetRuleInfo called, ruleInfo = %{public}s", ruleInfo.c_str());

    EcologicalRuleMgrServiceClient::GetInstance()->SetRuleInfo(ruleInfo);
    LOG_INFO("NAPI_SetRuleInfo end");
    return (napi_value)IEcologicalRuleMgrService::ERR_OK;
}

napi_value NAPI_SetSceneExperience(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_TWO;
    napi_value args[ARGS_TWO];

    napi_value thisPointer = nullptr;

    LOG_INFO("JS SetSceneExperience called");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    char *str1 = nullptr;
    size_t str1Len = 0;
    napi_get_value_string_utf8(env, args[PARAM_ONE], nullptr, 0, &str1Len);
    str1 = new char[str1Len + 1];
    napi_get_value_string_utf8(env, args[PARAM_ONE], str1, str1Len + 1, &str1Len);

    char *str2 = nullptr;
    size_t str2Len = 0;
    napi_get_value_string_utf8(env, args[PARAM_TWO], nullptr, 0, &str2Len);
    str2 = new char[str2Len + 1];
    napi_get_value_string_utf8(env, args[PARAM_TWO], str2, str2Len + 1, &str2Len);

    std::string ruleConfig(str1);
    std::string sceneExperience(str2);

    LOG_INFO("NAPI_SetSceneExperience called, ruleConfig = %{public}s, sceneExperience = %{public}s",
        ruleConfig.c_str(), sceneExperience.c_str());

    EcologicalRuleMgrServiceClient::GetInstance()->SetSceneExperience(ruleConfig, sceneExperience);
    LOG_INFO("NAPI_SetSceneExperience end");
    return (napi_value)IEcologicalRuleMgrService::ERR_OK;
}

napi_value NAPI_ExemptExperience(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_THREE;
    napi_value args[ARGS_THREE];

    napi_value thisPointer = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    char *str = nullptr;
    size_t strLen = 0;
    napi_get_value_string_utf8(env, args[PARAM_ONE], nullptr, 0, &strLen);
    str = new char[strLen + 1];
    napi_get_value_string_utf8(env, args[PARAM_ONE], str, strLen + 1, &strLen);

    int32_t rule = 0;
    napi_get_value_int32(env, args[PARAM_TWO], &rule);

    int32_t timeStamp = 0;
    napi_get_value_int32(env, args[PARAM_THREE], &timeStamp);

    std::string targetBundleName(str);
    LOG_INFO(
        "NAPI_ExemptExperience called, targetBundleName = %{public}s, rule = %{public}d, timestamp = %{public}d",
        targetBundleName.c_str(), rule, timeStamp);

    EcologicalRuleMgrServiceClient::GetInstance()->ExemptExperience(targetBundleName, rule, timeStamp);
    LOG_INFO("NAPI_ExemptExperience end");
    return (napi_value)IEcologicalRuleMgrService::ERR_OK;
}

napi_value NAPI_GetVersion(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    LOG_INFO("NAPI_GetVersion called");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    std::string versionJson;
    EcologicalRuleMgrServiceClient::GetInstance()->GetVersion(versionJson);

    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, versionJson.c_str(), NAPI_AUTO_LENGTH, &res));

    LOG_INFO("NAPI_GetVersion end, versionJson = %{public}s", versionJson.c_str());
    return res;
}

napi_value NAPI_GetSceneCode(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    char *str = nullptr;
    size_t strLen = 0;
    napi_get_value_string_utf8(env, args[PARAM_ONE], nullptr, 0, &strLen);
    str = new char[strLen + 1];
    napi_get_value_string_utf8(env, args[PARAM_ONE], str, strLen + 1, &strLen);

    std::string bundleName(str);
    LOG_INFO("NAPI_GetSceneCode called, bundleName = %{public}s", bundleName.c_str());

    std::string sceneCode;
    EcologicalRuleMgrServiceClient::GetInstance()->GetSceneCode(bundleName, sceneCode);

    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, sceneCode.c_str(), NAPI_AUTO_LENGTH, &res));

    LOG_INFO("NAPI_GetSceneCode end, scenecode = %{public}s", sceneCode.c_str());
    return res;
}

napi_value NAPI_GetInitialSceneCode(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    char *str = nullptr;
    size_t strLen = 0;
    napi_get_value_string_utf8(env, args[PARAM_ONE], nullptr, 0, &strLen);
    str = new char[strLen + 1];
    napi_get_value_string_utf8(env, args[PARAM_ONE], str, strLen + 1, &strLen);

    std::string bundleName(str);
    LOG_INFO("NAPI_GetInitialSceneCode called, bundleName = %{public}s", bundleName.c_str());

    std::string sceneCode;
    EcologicalRuleMgrServiceClient::GetInstance()->GetInitialSceneCode(bundleName, sceneCode);

    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, sceneCode.c_str(), NAPI_AUTO_LENGTH, &res));

    LOG_INFO("NAPI_GetInitialSceneCode end, scenecode = %{public}s", sceneCode.c_str());
    return res;
}

napi_value NAPI_GetSelfSceneCode(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    LOG_INFO("NAPI_GetSelfSceneCode called");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    std::string sceneCode;
    EcologicalRuleMgrServiceClient::GetInstance()->GetSelfSceneCode(sceneCode);

    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, sceneCode.c_str(), NAPI_AUTO_LENGTH, &res));

    LOG_INFO("NAPI_GetSelfSceneCode end, scenecode = %{public}s", sceneCode.c_str());
    return res;
}

napi_value NAPI_GetAdsVerificationVersion(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value args[ARGS_ONE];

    napi_value thisPointer = nullptr;

    LOG_INFO("NAPI_GetAdsVerificationVersion called");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisPointer, nullptr));

    int32_t version = 0;
    EcologicalRuleMgrServiceClient::GetInstance()->GetAdsVerificationVersion(version);

    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_int32(env, version, &res));

    LOG_INFO("NAPI_GetAdsVerificationVersion end, version = %{public}d", version);
    return res;
}
} // namespace EcologicalRuleMgrServiceNapi
} // namespace OHOS