/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "EcologicalRileMgrServiceStubFuzzer.h"

#include "iremote_stub.h"
#include "ipc_skeleton.h"
#include "message_parcel.h"
#include "system_ability.h"
#include "ecological_rule_mgr_service_interface.h"
#include "ecological_rule_mgr_service_stub.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "tokenid_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace EcologicalRuleMgrService {

const std::string PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST = "ohos.permission.MANAGE_ECOLOGICAL_RULE";
const std::string PERMISSION_GET_SCENE_CODE_TEST = "ohos.permission.GET_SCENE_CODE";
const std::u16string ERMS_INTERFACE_TOKEN =
    u"ohos.cloud.ecologicalrulemgrservice.EcologicalRuleMgrServiceInterface";

void NativeTokenGet(const char *perms[], int size)
{
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = size,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "EcologicalRuleMgrServiceStubFuzzTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

int32_t EcologicalRuleMgrServiceStubOnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    /*
    sptr<EcologicalRuleMgrServiceStub> ecologicalRuleMgrServiceStub = new EcologicalRuleMgrServiceStub();
    if (ecologicalRuleMgrServiceStub == nullptr) {
        return EcologicalRuleMgrServiceInterface::ERR_FAILED;
    }
    */
    return DelayedSingleton<EcologicalRuleMgrService>::GetInstance()->OnRemoteRequest(code, data, reply, option);
}
/*
bool QueryFreeInstallExperienceFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }
    Want want = static_cast<Want>(*data);
    CallerInfo callerInfo = static_cast<CallerInfo>(*data);

    if (!dataParcel.WriteParcelable(&want)) {
        return false;
    }

    if (!dataParcel.WriteParcelable(&callerInfo)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::QUERY_FREE_INSTALL_EXPERIENCE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool QueryStartExperienceFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }
    Want want = static_cast<Want>(*data);
    CallerInfo callerInfo = static_cast<CallerInfo>(*data);

    if (!dataParcel.WriteParcelable(&want)) {
        return false;
    }

    if (!dataParcel.WriteParcelable(&callerInfo)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::QUERY_START_EXPERIENCE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool IsSupportPublishFormFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    int32_t wantSize = *reinterpret_cast<const int32_t *>(data);
    if (!dataParcel.WriteInt32(wantSize)) {
        return false;
    }
    for (int i = 0; i < wantSize; i++) {
        Want want = static_cast<Want>(*data);
        if (!dataParcel.WriteParcelable(&want)) {
           return false;
        }
    }

    CallerInfo callerInfo = static_cast<CallerInfo>(*data);

    if (!dataParcel.WriteParcelable(&callerInfo)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::IS_SUPPORT_PUBLISH_FORM_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool EvaluateResolveInfosFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    Want want = static_cast<Want>(*data);
    CallerInfo callerInfo = static_cast<CallerInfo>(*data);

    int32_t type = *reinterpret_cast<const int32_t *>(data);

    if (!dataParcel.WriteParcelable(&want)) {
        return false;
    }

    if (!dataParcel.WriteParcelable(&callerInfo)) {
        return false;
    }

    if (!dataParcel.WriteInt32(type)) {
       return false;
    }

    int32_t abilityInfosSize = *reinterpret_cast<const int32_t *>(data);

    if (!dataParcel.WriteInt32(abilityInfosSize)) {
        return false;
    }

    for (int i = 0; i < wantSize; i++) {
        AbilityInfo abilityInfo = static_cast<AbilityInfo>(*data);
        if (!dataParcel.WriteParcelable(&abilityInfo)) {
            return false;
        }
    }

    MessageOption option = { MessageOption::TF_SYNC };

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::EVALUATE_RESOLVE_INFO_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}
*/

bool SetAppEnhancedDataFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }
    int32_t operType = *reinterpret_cast<const int32_t *>(data);
    std::string appData((const char*) data, size);
    if (!dataParcel.WriteInt32(operType)) {
        return false;
    }

    if (!dataParcel.WriteString(appData)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::SET_APP_ENHANCED_DATA_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool SetRuleInfoFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    std::string ruleInfo((const char*) data, size);

    if (!dataParcel.WriteString(ruleInfo)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::SET_RULE_INFO_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool ExemptExperienceFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    std::string targetBundleName((const char*) data, size);

    int32_t rule = *reinterpret_cast<const int32_t *>(data);

    int32_t timestamp = *reinterpret_cast<const int32_t *>(data);

    if (!dataParcel.WriteString(targetBundleName)) {
        return false;
    }

    if (!dataParcel.WriteInt32(rule)) {
        return false;
    }

    if (!dataParcel.WriteInt32(timestamp)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::SET_EXEMPT_EXPERIENCE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool SetSceneExperienceFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    std::string ruleConfig((const char*) data, size);
    std::string sceneExperience((const char*) data, size);
    if (!dataParcel.WriteString(ruleConfig)) {
        return false;
    }

    if (!dataParcel.WriteString(sceneExperience)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_MANAGE_ECOLOGICAL_RULE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::SET_SCENE_EXPERIENCE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool GetSceneCodeFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    std::string bundleName((const char*) data, size);

    if (!dataParcel.WriteString(bundleName)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_GET_SCENE_CODE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::GET_SCENE_CODE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

bool GetInitialSceneCodeFuzzTest(const uint8_t *data, size_t size) // 返回值类型需要和被测函数保持一致
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(ERMS_INTERFACE_TOKEN)) {
        return false;
    }

    std::string bundleName((const char*) data, size);

    if (!dataParcel.WriteString(bundleName)) {
        return false;
    }

    MessageOption option = { MessageOption::TF_SYNC };

    const char *perms[] = {PERMISSION_GET_SCENE_CODE_TEST.c_str()};
    NativeTokenGet(perms, 1);

    int32_t ret = EcologicalRuleMgrServiceStubOnRemoteRequest(EcologicalRuleMgrServiceInterface::GET_ORIGINAL_SCENE_CODE_CMD, dataParcel, replyParcel, option);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}

using TestFuncDef = bool (*)(const uint8_t *data, size_t size);

/*
    QueryFreeInstallExperienceFuzzTest,
    QueryStartExperienceFuzzTest,
    IsSupportPublishFormFuzzTest,
    EvaluateResolveInfosFuzzTest,
*/
TestFuncDef g_allTestFunc[] = {
    SetAppEnhancedDataFuzzTest,
    SetRuleInfoFuzzTest,
    ExemptExperienceFuzzTest,
    SetSceneExperienceFuzzTest,
    GetSceneCodeFuzzTest,
    GetInitialSceneCodeFuzzTest,
};

bool DoSomethingInterestingWithMyAPI(const uint8_t *rawData, size_t size)
{
    if (size < sizeof(int)) {
        return false;
    }
    const int index = *(static_cast<const uint8_t *>(rawData));
    rawData += sizeof(int);
    size -= sizeof(int);
    int funcCount = sizeof(g_allTestFunc) / sizeof(g_allTestFunc[0]);

    auto func = g_allTestFunc[index % funcCount];
    if (func != nullptr) {
        auto ret = func(rawData, size);
        return ret;
    }
    return false;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
} // namespace EcologicalRuleMgrService
} // namespace OHOS