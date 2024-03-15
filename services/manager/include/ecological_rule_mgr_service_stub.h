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

#ifndef SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_STUB_H
#define SERVICES_INCLUDE_ECOLOGICALRULEMANAGERSERVICE_STUB_H

#include <map>
#include <string>
#include <condition_variable>
#include <future>
#include "iremote_stub.h"
#include "ipc_skeleton.h"
#include "message_parcel.h"
#include "system_ability.h"
#include "ecological_rule_mgr_service_interface.h"
#include "access_token.h"
#include "accesstoken_kit.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace EcologicalRuleMgrService {
using namespace OHOS::AppExecFwk;

class EcologicalRuleMgrServiceStub : public IRemoteStub<EcologicalRuleMgrServiceInterface> {
public:
    using Want = OHOS::AAFwk::Want;
    using AbilityInfo = OHOS::AppExecFwk::AbilityInfo;

    EcologicalRuleMgrServiceStub();
    ~EcologicalRuleMgrServiceStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

protected:
    static void SendQueryFreeInstallExperienceResult(int32_t &code, bool &hasNewVersion, std::string &appUpdateInfo);
    static void SendEvaluateResolveInfosResult(int32_t &code, bool &hasNewVersion, std::string &appUpdateInfo);
    static void SendQueryStartExperienceResult(int32_t &code, bool &hasNewVersion, std::string &appUpdateInfo);
    static void SendIsSupportPublishFormResult(int32_t &code, bool &hasNewVersion, std::string &appUpdateInfo);

    bool updateServiceInit_ = false;
    std::condition_variable waitInitCondition_;
    std::mutex waitInitMutex_;

private:
    using EcologicalRuleManagerServiceFuncs = int32_t (EcologicalRuleMgrServiceStub::*)(MessageParcel &data,
        MessageParcel &reply);
    int32_t OnQueryInitialStatus(MessageParcel &data, MessageParcel &reply);

    int32_t OnQueryFreeInstallExperienceResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnEvaluateResolveInfosResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnQueryStartExperienceResult(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsSupportPublishFormResult(MessageParcel &data, MessageParcel &reply);

    bool EnforceInterceToken(MessageParcel &data);
    template <typename T> bool WriteParcelableVector(const std::vector<T> &parcelableVector, MessageParcel &reply);

    static std::mutex bmInstanceLock_;
    std::map<uint32_t, EcologicalRuleManagerServiceFuncs> memberFuncMap_;

    const int32_t MAX_WANT_SIZE = 15;
    const int32_t MAX_ABILITY_INFO_SIZE = 1000;

    bool VerifySystemApp();
};
} // EcologicalRuleMgrService
} // OHOS

#endif // SERVICES_INCLUDE_ECOLOGICALRULEMGRSERVICE_STUB_H